<?php
// $Id: html_entity_encode.inc,v 1.1.2.3 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_html_entity_encode_form',
  'callback' => 'feeds_tamper_html_entity_encode_callback',
  'name'     => 'HTML entity encode',
  'multi'    => 'loop',
  'category' => 'HTML',
);

function feeds_tamper_html_entity_encode_form($importer, $source, $settings) {
  $form = array();

  $form['html'] = array(
    '#value' => t('This will convert all HTML special characters such as &gt; and &amp; to &amp;gt; and &amp;apm;.')
  );

  return $form;
}

function feeds_tamper_html_entity_encode_callback($source, $item_key, $element_key, &$field, $values) {
  $field = check_plain($field);
}
