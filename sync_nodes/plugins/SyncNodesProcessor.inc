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

}

