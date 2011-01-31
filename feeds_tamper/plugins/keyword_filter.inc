<?php
// $Id: keyword_filter.inc,v 1.3.2.5 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_keyword_filter_form',
  'callback' => 'feeds_tamper_keyword_filter_callback',
  'validate' => 'feeds_tamper_keyword_filter_validate',
  'name'     => 'Keyword Filter',
  'multi'    => 'skip',
  'category' => 'Filter',
);

function feeds_tamper_keyword_filter_form($importer, $source, $settings) {

  $form = array();

  $form['words'] = array(
    '#type' => 'textarea',
    '#title' => t('Words or phrases to filter on'),
    '#default_value' => isset($settings['words']) ? $settings['words'] : '',
    '#description' => t('A list of words/phrases that need to appear in the text.'),
  );

  $form['word_boundaries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Respect word boundaries'),
    '#default_value' => isset($settings['word_boundaries']) ? $settings['word_boundaries'] : TRUE,
    '#description' => t('If checked, then "book" will match "book" but not "bookcase".'),
  );

  $form['case_sensitive'] = array(
    '#type' => 'checkbox',
    '#title' => t('Case sensitive'),
    '#default_value' => isset($settings['case_sensitive']) ? $settings['case_sensitive'] : FALSE,
    '#description' => t('If checked, then "book" will match "book" but not "Book" or "BOOK".')
  );

  return $form;
}

function feeds_tamper_keyword_filter_validate(&$values) {
  $values['word_list'] = explode("\n", $values['words']);
  $values['word_list'] = array_map('trim', $values['word_list']);

  $values['regex'] = FALSE;

  if (!$values['word_boundaries'] && $values['case_sensitive']) {
    $values['func'] = 'strpos';
  }

  else if (!$values['word_boundaries'] && !$values['case_sensitive']) {
    $values['func'] = 'stripos';
  }
  else {
    foreach ($values['word_list'] as &$word) {
      $word = '/\b' . $word . '\b/';

      if (!$values['case_sensitive']) {
        $word .= 'i';
      }
    }
    $values['regex'] = TRUE;
  }
}

function feeds_tamper_keyword_filter_callback($source, $item_key, $element_key, &$field, $values) {
  if ($values['regex']) {
    foreach ($values['word_list'] as $word) {
      if (preg_match($word, $field) > 0) {
        return;
      }
    }
  }
  else {
    foreach ($values['word_list'] as $word) {
      if ($values['func']($field, $word) !== FALSE) {
        return;
      }
    }
  }
  unset($source->batch->items[$item_key]);
}
