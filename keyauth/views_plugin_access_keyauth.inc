<?php
// $Id$

/**
 * Access plugin that provides key based control to a view
 */
class views_plugin_access_keyauth extends views_plugin_access {
  function access($account) {
    return keyauth_views_access();
  }

  function get_access_callback() {
    return array('keyauth_views_access', array());
  }

  function summary_title() {
    return t('Key authentication');
  }

  function option_defaults(&$options) {
  }

  function options_form(&$form, &$form_state) {
    keyauth_include();
    $keys = keyauth_all_keys();
    $options = array();
    foreach ($keys as $pub => $key) {
      $options[$pub] = $key['title'];
    }
    $form['key'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('Allowed key'),
      '#default_value' => $this->options['key'],
      '#description' => t('Select the key that will grant access to this view'),
    );
  }
}
