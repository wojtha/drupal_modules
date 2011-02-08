<?php

// $Id$

/**
 * @file
 * Mapper that exposes a node's translated guid as mapping target.
 */

/**
 * Implementation of hook_feeds_node_processor_targets_alter().
 *
 * @see FeedsNodeProcessor::getMappingTargets().
 */
function sync_nodes_feeds_node_processor_targets_alter(&$targets, $content_type) {
  if (module_exists('translation')) {
    $targets['tguid'] = array(
      'name' => t('Translated guid'),
      'callback' => 'tguid_feeds_set_target',
      'description' => t('The guid of the node from which this node is being translated')
    );
  }
}

/**
 * Callback for mapping. Here is where the actual mapping happens.
 *
 * When the callback is invoked, $target contains the name of the field the
 * user has decided to map to and $value contains the value of the feed item
 * element the user has picked as a source.
 */
function tguid_feeds_set_target($node, $target, $value) {

  $nid = _sync_nodes_guid_to_nid($value);
  if ($nid != 0) {
    $node->translation_source = node_load($nid);
    $node->tnid = $nid;
  }
}

