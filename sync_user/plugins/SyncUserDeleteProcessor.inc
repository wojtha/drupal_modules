<?php

// $Id$

/**
 * @file
 */
class SyncUserDeleteProcessor extends FeedsUserProcessor {
  
  /**
   * Implementation of FeedsProcessor::process().
   */
  public function process(FeedsImportBatch $batch, FeedsSource $source) {
    // Keep track of processed items in this pass.
    $processed = 0;

    while ($item = $batch->shiftItem()) {

      // If the item already exists and we're flagged to delete it, do that instead.
      // This part is added from the parent class.
      if (!empty($item['deleted'])) {
        $uid = $this->existingItemId($batch, $source);
        if (!empty($uid)) {
          user_delete(array(), $uid);
          continue;
        }
      }
    }
  }
}
