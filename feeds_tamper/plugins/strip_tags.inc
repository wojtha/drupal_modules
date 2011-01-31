<?php
// $Id: strip_tags.inc,v 1.1.2.2 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_strip_tags_form',
  'callback' => 'feeds_tamper_strip_tags_callback',
  'validate' => 'feeds_tamper_strip_tags_validate',
  'name'     => 'Strip Tags',
  'multi'    => 'loop',
  'category' => 'HTML',
);

function feeds_tamper_strip_tags_form($importer, $source, $settings) {
  $form = array();

  $form['allowed_tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed tags'),
    '#default_value' => isset($settings['allowed_tags']) ? $settings['allowed_tags'] : '',
    '#description' => t('A list of allowed tags, such as %a, %b.', array('%a' => '<a>', '%b' => '<em>')),
  );

  return $form;
}

function feeds_tamper_strip_tags_validate(&$values) {
  $values['allowed_tags'] = trim($values['allowed_tags']);
}

function feeds_tamper_strip_tags_callback($source, $item_key, $element_key, &$field, $values) {
  $field = strip_tags($field, $values['allowed_tags']);
}
