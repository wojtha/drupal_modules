<?php
// $Id$

class SyncNodesDeleteProcessor extends FeedsNodeProcessor {

  /**
   * Override parent::process()
   */
  public function process(FeedsImportBatch $batch, FeedsSource $source) {
    global $user;
    // Make sure nodes can be deleted
    $original_user = $user;
    $user = user_load(1);
    $processed = 0;
    
    while ($item = $batch->shiftItem()) {
      if (!empty($item['deleted'])) {
        $nid = $this->existingItemId($batch, $source);
        if (!empty($nid)) {
          node_delete($nid);
          continue;
        }
      }
    }
    
    // Restore original user
    $user = $original_user;
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
            // Select node based on guid only
            $nid = db_result(db_query("SELECT nid FROM {feeds_node_item} WHERE guid = '%s'", $value));
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
