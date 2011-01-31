<?php
// $Id$

$plugin = array(
  'form' => 'feeds_tamper_php_code_form',
  'callback' => 'feeds_tamper_php_code_callback',
  'name' => 'Custom PHP code',
  'multi' => 'direct'
);

function feeds_tamper_php_code_form($importer, $source, $settings) {
  $form = array();
  

  $form['php_code'] = array(
    '#type' => 'textarea',
    '#title' => t('PHP code to execute'),
    '#default_value' => isset($settings['php_code']) ? $settings['php_code'] : '',
    '#description' => t('Enter your PHP code surrounded by &lt;?php and ?&gt;. You can directly modify the variable $field in your PHP code.')
  );

  return $form;
}

function feeds_tamper_php_code_callback($source, $item_key, $element_key, &$field, $values) {
  // Not using drupal_eval in order to get access to the $field variable in the PHP code being evaluated
  ob_start();
  eval('?>' . $values['php_code']);
  ob_end_clean();
}

