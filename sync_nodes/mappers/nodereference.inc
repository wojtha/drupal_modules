<?php

// $Id$

/**
 * @file
 * Mapper that exposes a node's references as mapping targets.
 */

/**
 * Implementation of hook_feeds_node_processor_targets_alter().
 *
 * @see FeedsNodeProcessor::getMappingTargets().
 */
function nodereference_feeds_node_processor_targets_alter(&$targets, $content_type) {
  $info = content_types($content_type);
  $fields = array();
  if (isset($info['fields']) && count($info['fields'])) {
    foreach ($info['fields'] as $k => $field) {
      $targets[$k] = array(
        'name' => $field['widget']['label'],
        'callback' => 'nodereference_feeds_set_target',
        'description' => t('The CCK !name field of the node.', array('!name' => $field['widget']['label'])),
      );
    }
  }
}

/**
 * Callback for mapping. Here is where the actual mapping happens.
 *
 * When the callback is invoked, $target contains the name of the field the
 * user has decided to map to and $value contains the value of the feed item
 * element the user has picked as a source.
 */
function nodereference_feeds_set_target($node, $target, $value) {

  $field = isset($node->$target) ? $node->$target : array();

  // Handle multiple value fields.
  if (is_array($value)) {
    $i = 0;
    foreach ($value as $v) {
      if (!is_array($v) && !is_object($v) && $result = db_fetch_object(db_query("SELECT nid FROM {feeds_node_item} WHERE guid = '%s'", $v))) {
        $field[$i]['nid'] = $result->nid;
      }
      $i++;
    }
  }
  else {
    // Get nid from guid
    $guid = $value;
    if ($result = db_fetch_object(db_query("SELECT nid FROM {feeds_node_item} WHERE guid = '%s'", $guid))) {
      $field[0]['nid'] = $result->nid;
    }
  }

  $node->$target = $field;
}
