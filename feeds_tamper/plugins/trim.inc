<?php
// $Id: trim.inc,v 1.1.2.5 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_trim_form',
  'callback' => 'feeds_tamper_trim_callback',
  'name'     => 'Trim',
  'multi'    => 'loop',
  'category' => 'Text',
);

function feeds_tamper_trim_form($importer, $source, $settings) {
  $form = array();

  $form['mask'] = array(
    '#type' => 'textfield',
    '#title' => t('Characters to trim'),
    '#default_value' => isset($settings['mask']) ? $settings['mask'] : '',
    '#description' => t('The characters to remove from the string. If blank, then whitespace will be removed.'),
  );

  $form['side'] = array(
    '#type' => 'radios',
    '#title' => t('Side'),
    '#default_value' => isset($settings['mask']) ? $settings['mask'] : 'trim',
    '#options' => array('trim' => t('Both'), 'ltrim' => t('Left'), 'rtrim' => t('Right')),
  );

  return $form;
}

function feeds_tamper_trim_callback($source, $item_key, $element_key, &$field, $values) {
  $field = $values['side']($field, $values['mask']);
}
