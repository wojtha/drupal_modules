<?php
// $Id: views_plugin_style_atom_delete.inc,v 1.1.2.1 2010/12/31 22:07:37 hefox Exp $

/**
 * @file
 * Style plugin for an Atom feed using the Tombstone specification for entry deletion.
 *
 * http://tools.ietf.org/html/draft-snell-atompub-tombstones-06
 *
 * Note that as of this writing the Tombstone spec is still a draft.
 */

class views_plugin_style_atom_delete extends views_plugin_style {

  /**
   * Attach this view to another display as a feed.
   */
  function attach_to($display_id, $path, $title) {
  }


  function option_definition() {
    $options = parent::option_definition();
    $options['guid'] = array(
      'use_feed' => array('default' => array()), 
    );

    return $options;
  }


  /**
   * Provide a form for setting options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $options = parent::option_definition();

    $handlers = $this->display->handler->get_handlers('field');
    $form['guid'] = array(
      '#type' => 'fieldset',
      '#title' => t('GUID settings'),
      '#description' => t('Controls how the GUID is created.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    if (module_exists('feeds')) {
      $form['guid']['use_existing_from_feed'] = array(
        '#title' => t('Use existing GUID when available'),
        '#type' => 'checkbox',
        '#default_value' => !empty($this->options['guid']['use_existing_from_feed']),
        '#description' => t('If the node was imported into the site from another feed, use the original GUID.'),
      );
    }
    $form['guid']['#access'] = count(element_children($form['guid'])) ? TRUE : FALSE;
  }

}
