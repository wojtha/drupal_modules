<?php
// $Id: implode.inc,v 1.2.2.4 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_implode_form',
  'callback' => 'feeds_tamper_implode_callback',
  'validate' => 'feeds_tamper_implode_validate',
  'name'     => 'Implode',
  'multi'    => 'direct',
  'single'   => 'skip',
  'category' => 'List',
);

function feeds_tamper_implode_form($importer, $source, $settings) {
  $form = array();

  $form['glue'] = array(
    '#type' => 'textfield',
    '#title' => t('String glue'),
    '#default_value' => isset($settings['glue']) ? $settings['glue'] : ',',
    '#description' => t('Join array elements into a string. For example, array(\'a\', \'b\', \'c\') would become "a, b, c".
                        A space can be represented by %s, tabs by %t, and newlines by %n.'),
  );

  return $form;
}

function feeds_tamper_implode_validate(&$values) {
  $values['real_glue'] = str_replace(array('%s', '%t', '%n'), array(' ', "\t", "\n"), $values['glue']);
}

function feeds_tamper_implode_callback($source, $item_key, $element_key, &$field, $values) {
  $field = implode($values['real_glue'], $field);
}
