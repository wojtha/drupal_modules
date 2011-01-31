<?php
// $Id: find_replace_regex.inc,v 1.4.2.6 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_find_replace_regex_form',
  'callback' => 'feeds_tamper_find_replace_regex_callback',
  'validate' => 'feeds_tamper_find_replace_regex_validate',
  'name'     => 'Find Replace REGEX',
  'multi'    => 'direct',
  'category' => 'Text',
);

function feeds_tamper_find_replace_regex_form($importer, $source, $settings) {
  $form = array();

  $form['find'] = array(
    '#type' => 'textfield',
    '#title' => t('REGEX to find'),
    '#default_value' => isset($settings['find']) ? $settings['find'] : '',
    '#description' => t(check_plain('A regular expression in the form: /<your regex here>/')),
  );

  $form['replace'] = array(
    '#type' => 'textfield',
    '#title' => t('Replacement pattern'),
    '#default_value' => isset($settings['replace']) ? $settings['replace'] : '',
    '#description' => t('The replacement pattern.'),
  );

  $form['limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Limit number of replacements'),
    '#default_value' => isset($settings['limit']) ? $settings['limit'] : '',
    '#description' => t('This sets an optional limit. Leave it blank for no limit.'),
  );

  return $form;
}

function feeds_tamper_find_replace_regex_validate(&$values) {
  // test the regex
  $test = @preg_replace($values['find'], '', 'asdfsadf');
  if ($test === NULL) {
    return form_set_error('find', 'Invalid regular expression.');
  }
  $values['limit'] = trim($values['limit']);
  if (empty($values['limit'])) {
    $values['real_limit'] = -1;
  }
  else {
    $values['real_limit'] = intval($values['limit']);
  }
}

function feeds_tamper_find_replace_regex_callback($source, $item_key, $element_key, &$field, $values) {
  $field = preg_replace($values['find'], $values['replace'], $field, $values['real_limit']);
}
