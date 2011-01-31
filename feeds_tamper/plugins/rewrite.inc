<?php
// $Id: rewrite.inc,v 1.1.2.5 2011/01/27 10:52:56 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_rewrite_form',
  'callback' => 'feeds_tamper_rewrite_callback',
  'name'     => 'Rewrite',
  'multi'    => 'skip',
  'category' => 'Other',
);

function feeds_tamper_rewrite_form($importer, $source, $settings) {
  $form = array();

  $mappings = $importer->processor->config['mappings'];
  $replace = array();
  $options = array();
  foreach ($mappings as $mapping) {
    $replace[] = '[' . $mapping['source'] . ']';
  }
  $form['title']['#value'] = t('The availiable replacement patterns are:');
  $form['help']['#value'] = theme('item_list', $replace);

  $form['text'] = array(
    '#type' => 'textarea',
    '#title' => t('Replacement pattern'),
    '#default_value' => isset($settings['text']) ? $settings['text'] : '',
  );
  return $form;
}

function feeds_tamper_rewrite_callback($source, $item_key, $element_key, &$field, $values) {
  $trans = array();
  $item = $source->batch->items[$item_key];
  foreach ($item as $key => $value) {
    $trans['[' . $key . ']'] = $value;
  }
  $field = strtr($values['text'], $trans);
}
