<?php
// $Id: FeedsAtomRDFProcessor.inc,v 1.1.2.1 2010/12/31 23:51:23 hefox Exp $

/**
 * @file
 * Contains the feeds atom RDF processor class.
 */

/**
 * Creates nodes from feed items.
 */
class FeedsAtomRDFProcessor extends FeedsNodeProcessor {

  /**
   * Override process() method to add handling for entry deletion.
   */
  public function process(FeedsImportBatch $batch, FeedsSource $source) {

    // Keep track of processed items in this pass.
    $processed = 0;

    while ($item = $batch->shiftItem()) {

      // If the item already exists and we're flagged to delete it, do that instead.
      // This part is added from the parent class.
      if (!empty($item['deleted'])) {
        $nid = $this->existingItemIdGlobal($item, $source);
        if (!empty($nid)) {
          node_delete($nid);
          continue;
        }
      }

      // Create/update if item does not exist or update existing is enabled.
      $nid = empty($nid) ? $this->existingItemId($item, $source) : $nid;
      if (empty($nid) || $this->config['update_existing']) {

        $node = new stdClass();
        $hash = $this->hash($item);

        // If updating check whether item actually has changed and if so,
        // retrieve its nid and vid.
        if (!empty($nid)) {
          if ($hash == $this->getHash($nid)) {
            continue;
          }
          $node->nid = $nid;
          $node->vid = db_result(db_query("SELECT vid FROM {node} WHERE nid = %d", $nid));
        }
        else {
          $node->created = FEEDS_REQUEST_TIME;
        }

        // Populate and prepare node object.
        $node->type = $this->config['content_type'];
        $node->changed = FEEDS_REQUEST_TIME;
        $node->feeds_node_item = new stdClass();
        $node->feeds_node_item->hash = $hash;
        $node->feeds_node_item->id = $this->id;
        $node->feeds_node_item->imported = FEEDS_REQUEST_TIME;
        $node->feeds_node_item->feed_nid = $source->feed_nid;
        static $included;
        if (!$included) {
          module_load_include('inc', 'node', 'node.pages');
          $included = TRUE;
        }
        node_object_prepare($node);

        // Populate properties that are set by node_object_prepare().
        $node->log = 'Created/updated by FeedsNodeProcessor';
        $node->uid = 0;

        // Map and save nodes. If errors occur don't stop but report them.
        try {
          $this->map($item, $node, $source);
          node_save($node);
          if (!empty($nid)) {
            $batch->updated++;
          }
          else {
            $batch->created++;
          }
        }
        catch (Exception $e) {
          drupal_set_message($e->getMessage(), 'warning');
          watchdog('feeds', $e->getMessage(), array(), WATCHDOG_WARNING);
        }
      }

      $processed++;
      if ($processed >= variable_get('feeds_node_batch_size', FEEDS_NODE_BATCH_SIZE)) {
        return (1.0 / ($batch->total + 1)) * ($batch->updated + $batch->created); // Add + 1 to make sure that result is not 1.0 = finished.
      }
    }

    // Set messages.
    if ($batch->created) {
      drupal_set_message(format_plural($batch->created, 'Created @number @type node.', 'Created @number @type nodes.', array('@number' => $batch->created, '@type' => node_get_types('name', $this->config['content_type']))));
    }
    elseif ($batch->updated) {
      drupal_set_message(format_plural($batch->updated, 'Updated @number @type node.', 'Updated @number @type nodes.', array('@number' => $batch->updated, '@type' => node_get_types('name', $this->config['content_type']))));
    }
    else {
      drupal_set_message(t('There is no new content.'));
    }

    return FEEDS_BATCH_COMPLETE;
  }

  /**
   * Add handler to find an id globally
   *
   * Because our deletion feeds are not necessarily the same feed as the "create new"
   * feed, we need to check for existing items across all feeds, not just
   * the current feed.  As long as the GUID is really unique that should not
   * cause a problem.
   *
   * @todo Refactor this once http://drupal.org/node/828176 gets resolved.
   *
   * @see FeedsNodeProcessor
   */
  protected function existingItemIdGlobal($source_item, FeedsSource $source) {
    // Iterate through all unique targets and test whether they do already
    // exist in the database.
    foreach ($this->uniqueTargets($source_item) as $target => $value) {
      switch ($target) {
        case 'url':
          $nid = db_result(db_query("SELECT nid FROM {feeds_node_item} WHERE id = '%s' AND url = '%s'", $source->id, $value));
          break;
        case 'guid':
          $nid = db_result(db_query("SELECT nid FROM {feeds_node_item} WHERE id = '%s' AND guid = '%s'", $source->id, $value));
          break;
      }
      if ($nid) {
        // Return with the first nid found.
        return $nid;
      }
    }
    return 0;
  }

  /**
   * Override parent::map() to load all available add-on mappers.
   *
   * We also add a $source parameter that contains the FeedsSource object that
   * controls this feed.
   */
  protected function map($source_item, $target_item, $source) {
    self::loadMappers();

    // Static cache $targets as getMappingTargets() may be an expensive method.
    static $targets;
    if (!isset($targets[$this->id])) {
      $targets[$this->id] = $this->getMappingTargets();
    }
    $parser = feeds_importer($this->id)->parser;
    if (empty($target_item)) {
      $target_item = array();
    }

    // Set custom fields
    // http://groups.drupal.org/node/8796
    // http://www.stonemind.net/blog/index.php?s=cck
    // http://civicactions.com/blog/cck_import_and_update
    // http://www.lullabot.com/articles/quick-and-dirty-cck-imports
    foreach ($source_item['rdf'] as $key => $value ) {
      if (empty($key)) continue;
      $value = $source_item['rdf'][$key];
      $fname = drupal_substr($key, 0, 6);
      if ($fname == "field_") {
        // Build up a field value.
        $target_item->$key = $value;
      }
      else {
        // Set properties on the node.  There's a couple we know we don't want,
        // because their meaning is site-specific anyway.
        // @todo Replace this logic with the mapping engine for more flexibility.
        if (!in_array($key, array('nid', 'vid', 'revision_uid', 'log', 'created', 'changed', 'revision_timestamp', 'last_comment_timestamp'))) {
          $target_item->$key = $value;
        }
      }
    }

    // This is where the actual original atom mapping happens: For every mapping
    // we envoke the parser's getSourceElement() method to retrieve the value of
    // the source element and pass it to the processor's setTargetElement() to
    // stick it on the right place of the target item. If the mapping specifies
    // a callback method, use the callback instead of setTargetElement().
    foreach ($this->config['mappings'] as $mapping) {
      $value = $parser->getSourceElement($source_item, $mapping['source']);

      if (is_array($targets[$this->id][$mapping['target']]) && isset($targets[$this->id][$mapping['target']]['callback']) && function_exists($targets[$this->id][$mapping['target']]['callback'])) {
        $callback = $targets[$this->id][$mapping['target']]['callback'];
        $callback($target_item, $mapping['target'], $value);
      }
      else {
        $this->setTargetElement($target_item, $mapping['target'], $value);
      }
    }

    // Allow other modules to add additional mapping
    // Invokes hook_feeds_atom_rdf_map_alter().
    drupal_alter('feeds_atom_rdf_map', $target_item, $source_item, $source);

    return $target_item;
  }
}

/**
 * Specialized version of FeedsEnclosure to ensure uniqueness.
 *
 * This class will only ever download a file once per processing run.
 */
class FeedsEnclosureUnique extends FeedsEnclosure {

  /**
   * Tracking variable to keep track of what files we've already downloaded.
   *
   * @var array
   */
  protected static $downloaded = array();

  public function getFile() {
    $value = $this->getValue();
    if (empty(self::$downloaded[$value])) {
      // parent::getFile() will populate $this->file appropriately.
      parent::getFile();
      self::$downloaded[$value] = $this->file;
    }
    else {
      $this->file = self::$downloaded[$value];
    }
    return $this->file;
  }

  /**
   * Saves the file represented by this enclosure to disk.
   *
   * If the file already exists, based on its sha1() hash, then we will simply
   * reuse the existing file rather than saving a new one.
   *
   * @param $target_dir
   *   The directory to which to save the file.  Note that if the file has already
   *   been imported it is possible that it will not be in the requested directory,
   *   in which case this method returns the existing file info in its existing
   *   location.
   * @return
   *   The file info array as defined by filefield of the file that we just saved,
   *   or of the pre-existing file that should be used instead.
   */
  public function saveTo($target_dir) {
    $new_file_hash = sha1_file($this->file);

    $fid = db_result(db_query("SELECT fid FROM {feeds_atom_file_import} WHERE sha1 = '%s'", $new_file_hash));
    if ($fid) {
      // Pull the info for the existing file and return that. We won't save
      // the new file at all.
      $info = field_file_load($fid);

      return $info;
    }
    else {
      // Save the new file, and record its hash for later matching.
      $info = field_file_save_file($this->file, array(), $target_dir);
      db_query("INSERT INTO {feeds_atom_file_import} (fid, sha1) VALUES (%d, '%s')", $info['fid'], $new_file_hash);

      return $info;
    }
  }
}

