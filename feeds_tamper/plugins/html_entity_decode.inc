<?php
// $Id: html_entity_decode.inc,v 1.1.2.3 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_html_entity_decode_form',
  'callback' => 'feeds_tamper_html_entity_decode_callback',
  'name'     => 'HTML entity decode',
  'multi'    => 'loop',
  'category' => 'HTML',
);

function feeds_tamper_html_entity_decode_form($importer, $source, $settings) {
  $form = array();

  $form['html'] = array(
    '#value' => t('This will convert all HTML entities such as &amp;amp; and &amp;quot; to &amp; and &quot;.')
  );

  return $form;
}

function feeds_tamper_html_entity_decode_callback($source, $item_key, $element_key, &$field, $values) {
  $field = html_entity_decode($field, ENT_QUOTES, 'UTF-8');
}
