<?php
// $Id: explode.inc,v 1.2.2.5 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_explode_form',
  'callback' => 'feeds_tamper_explode_callback',
  'validate' => 'feeds_tamper_explode_validate',
  'name'     => 'Explode',
  'multi'    => 'skip',
  'category' => 'List',
);

function feeds_tamper_explode_form($importer, $source, $settings) {
  $form = array();

  $form['separator'] = array(
    '#type' => 'textfield',
    '#title' => t('String separator'),
    '#default_value' => isset($settings['separator']) ? $settings['separator'] : ',',
    '#description' => t('This will break up sequenced data into an array. For example, "a, b, c" would get broken up into the array(\'a\', \'b\', \'c\').
                        A space can be represented by %s, tabs by %t, and newlines by %n.'),
  );

  return $form;
}

function feeds_tamper_explode_validate(&$values) {
  $values['real_separator'] = str_replace(array('%s', '%t', '%n'), array(' ', "\t", "\n"), $values['separator']);
}

function feeds_tamper_explode_callback($source, $item_key, $element_key, &$field, $values) {
  $field = explode($values['real_separator'], $field);
}
