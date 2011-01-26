<?php
// $Id$

class SyncNodesProcessor extends FeedsNodeProcessor {
  
  /**
   * Source config
   */
  protected $source_config = NULL;
  
  /**
   * Implementation of FeedsProcessor::process().
   */
  public function process(FeedsImportBatch $batch, FeedsSource $source) {
    $this->source_config = $source->getConfigFor($this);
    parent::process($batch, $source);
  }
  
  /**
   * Override parent::buildNode()
   */
  protected function buildNode($nid, $feed_nid) {
    $node = parent::buildNode($nid, $feed_nid);
    if ($this->source_config['author']) {
      $node->uid = $this->source_config['author'];
    }
    return $node;
  }
  
  /**
   * Expose source form
   */
  public function sourceForm($source_config) {
    $form = array();
    $form['author'] = array(
      '#type' => 'hidden',
      '#default_value' => isset($source_config['author']) ? $source_config['author'] : ''
    );
    return $form;
  }
  
  /**
   * Override parent::existingItemId()
   */
  protected function existingItemId(FeedsImportBatch $batch, FeedsSource $source) {
    foreach ($this->uniqueTargets($batch) as $target => $value) {
      switch ($target) {
        case 'nid':
          $nid = db_result(db_query("SELECT nid FROM {node} WHERE nid = %d", $value));
          break;
        case 'url':
          $nid = db_result(db_query("SELECT nid FROM {feeds_node_item} WHERE feed_nid = %d AND id = '%s' AND url = '%s'", $source->feed_nid, $source->id, $value));
          break;
        case 'guid':
          // Look at the structure of the URL
          if (strpos($value, url('', array('absolute' => TRUE, 'language' => ''))) !== FALSE) {
            // Get node id
            $nid = substr(strrchr($value, '/'), 1);
          }
          else {
            $nid = db_result(db_query("SELECT nid FROM {feeds_node_item} WHERE feed_nid = %d AND id = '%s' AND guid = '%s'", $source->feed_nid, $source->id, $value));
          }
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
   * Return available mapping targets.
   */
  public function getMappingTargets() {
    $targets = array(
      'title' => array(
        'name' => t('Title'),
        'description' => t('The title of the node.'),
       ),
     );
    // Include body field only if available.
    $type = node_get_types('type',  $this->config['content_type']);
    if ($type->has_body) {
      // Using 'teaser' instead of 'body' forces entire content above the break.
      $targets['body'] = array(
        'name' => t('Body'),
        'description' => t('The body of the node. The teaser will be the same as the entire body.'),
      );
    }
    $targets += array(
      'nid' => array(
        'name' => t('Node ID'),
        'description' => t('The nid of the node. NOTE: use this feature with care, node ids are usually assigned by Drupal.'),
        'optional_unique' => TRUE,
      ),
      'uid' => array(
        'name' => t('User ID'),
        'description' => t('The Drupal user ID of the node author.'),
      ),
      'status' => array(
        'name' => t('Published status'),
        'description' => t('Whether a node is published or not. 1 stands for published, 0 for not published.'),
      ),
      'created' => array(
        'name' => t('Published date'),
        'description' => t('The UNIX time when a node has been published.'),
      ),
      'url' => array(
        'name' => t('URL'),
        'description' => t('The external URL of the node. E. g. the feed item URL in the case of a syndication feed. May be unique.'),
        'optional_unique' => TRUE,
      ),
      'guid' => array(
        'name' => t('GUID'),
        'description' => t('The external GUID of the node. E. g. the feed item GUID in the case of a syndication feed. May be unique.'),
        'optional_unique' => TRUE,
      ),
    );

    // Let other modules expose mapping targets.
    self::loadMappers();
    // Expose sync_nodes mappers
    $path = drupal_get_path('module', 'sync_nodes').'/mappers';
    $files = drupal_system_listing('.*\.inc$', $path, 'name', 0);
    foreach ($files as $file) {
      if (strstr($file->filename, '/mappers/')) {
        require_once("./$file->filename");
      }
    }
    // Rebuild cache.
    module_implements('', FALSE, TRUE);
    drupal_alter('feeds_node_processor_targets', $targets, $this->config['content_type']);

    return $targets;
  }
    

}

