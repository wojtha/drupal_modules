<?php
// $Id: truncate_text.inc,v 1.1.2.2 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_truncate_text_form',
  'callback' => 'feeds_tamper_truncate_text_callback',
  'name'     => 'truncate_text',
  'multi'    => 'loop',
  'category' => 'Text',
);

function feeds_tamper_truncate_text_form($importer, $source, $settings) {
  $form = array();

  $form['num_char'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of characters to truncate text to.'),
    '#default_value' => isset($settings['num_char']) ? $settings['num_char'] : '',
    '#description' => t('The characters of characters a string can be.'),
  );

  $form['ellipses'] = array(
    '#type' => 'checkbox',
    '#title' => t('Ellipses'),
    '#default_value' => isset($settings['ellipses']) ? $settings['ellipses'] : FALSE,
    '#description' => t('Add ellipses (...) to the end of the truncated text.'),
  );

  return $form;
}

function feeds_tamper_truncate_text_callback($source, $item_key, $element_key, &$field, $values) {
  $field = substr($field, 0, $values['num_char']);

  if ($values['ellipses']) {
    $field .= '...';
  }
}
