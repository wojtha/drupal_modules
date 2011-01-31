<?php
// $Id: find_replace.inc,v 1.4.2.7 2011/01/14 05:06:09 twistor Exp $

$plugin = array(
  'form'     => 'feeds_tamper_find_replace_form',
  'callback' => 'feeds_tamper_find_replace_callback',
  'validate' => 'feeds_tamper_find_replace_validate',
  'name'     => 'Find Replace',
  'multi'    => 'loop',
  'category' => 'Text',
);

function feeds_tamper_find_replace_form($importer, $source, $settings) {
  $form = array();

  $form['find'] = array(
    '#type' => 'textfield',
    '#title' => t('Text to find'),
    '#default_value' => isset($settings['find']) ? $settings['find'] : '',
  );

  $form['replace'] = array(
    '#type' => 'textfield',
    '#title' => t('Text to replace'),
    '#default_value' => isset($settings['replace']) ? $settings['replace'] : '',
  );

  $form['case_sensitive'] = array(
    '#type' => 'checkbox',
    '#title' => t('Case sensitive'),
    '#default_value' => isset($settings['case_sensitive']) ? $settings['case_sensitive'] : FALSE,
    '#description' => t('If checked, "book" will match "book" but not "Book" or "BOOK".')
  );

  $form['word_boundaries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Respect word boundaries'),
    '#default_value' => isset($settings['word_boundaries']) ? $settings['word_boundaries'] : FALSE,
    '#description' => t('If checked, "book" will match "book" but not "bookcase".'),
  );

  $form['whole'] = array(
    '#type' => 'checkbox',
    '#title' => t('Match whole word/phrase'),
    '#default_value' => isset($settings['whole']) ? $settings['whole'] : FALSE,
    '#description' => t('If checked, then the whole word or phrase will be matched, e.g. "book" will match not match "book" in "the book".<br>If this option is selected then "Respect word boundaries" above will be ignored.')
  );

  return $form;
}

function feeds_tamper_find_replace_validate(&$values) {
  $values['regex'] = FALSE;

  if (!$values['word_boundaries'] && !$values['whole'] &&
      $values['case_sensitive']) {
    $values['func'] = 'str_replace';
  }

  else if (!$values['word_boundaries'] && !$values['whole'] &&
           !$values['case_sensitive']) {
    $values['func'] = 'str_ireplace';
  }

  else {
    $values['regex'] = TRUE;

    if ($values['whole']) {
      $regex = '/^' . $values['find'] . '$/';
    }
    else {
      $regex = '/\b' . $values['find'] . '\b/';
    }
    if (!$values['case_sensitive']) {
      $regex .= 'i';
    }

    $values['regex_find'] = $regex;
  }
}

function feeds_tamper_find_replace_callback($source, $item_key, $element_key, &$field, $values) {
  if ($values['regex']) {
    $field = preg_replace($values['regex_find'], $values['replace'], $field);
  }

  else {
    $field = $values['func']($values['find'], $values['replace'], $field);
  }
}
