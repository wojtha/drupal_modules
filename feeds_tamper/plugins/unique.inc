<?php
// $Id: unique.inc,v 1.1.2.1 2011/01/14 05:40:05 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_unique_form',
  'callback' => 'feeds_tamper_unique_callback',
  'name'     => 'Unique',
  'multi'    => 'direct',
  'single'   => 'skip',
  'category' => 'List',
);

function feeds_tamper_unique_form($importer, $source, $settings) {
  $form = array();

  $form['#value'] = t('Makes the elements in a multivalued field unique.');

  return $form;
}

function feeds_tamper_unique_callback($source, $item_key, $element_key, &$field, $values) {
  $unique = array();
  foreach ($field as $f) {
    if (!in_array($f, $unique)) {
      $unique[] = $f;
    }
  }
  $field = $unique;
}
