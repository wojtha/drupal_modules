<?php
// $Id: feeds_tamper_ui.admin.inc,v 1.1.2.3 2011/01/27 11:33:47 twistor Exp $

/**
 * Build overview of available configurations.
 */
function feeds_tamper_ui_overview_form(&$form_state) {
  $importers = feeds_importer_load_all();
  $options = array_keys($importers);
  $options = array_combine($options, $options);
  $form['importer'] = array(
    '#title' => t('Select Importer'),
    '#type' => 'select',
    '#options' => $options,
  );

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Continue'),
  );
  return $form;
}

function feeds_tamper_ui_overview_form_submit($form, &$form_state) {
  $importer_id = check_plain($form_state['values']['importer']);
  drupal_goto('admin/build/feeds_tamper/edit/' . $importer_id);
}

function feeds_tamper_ui_edit_form(&$form_state, $importer) {
  $mappings = $importer->processor->config['mappings'];

  $instances = feeds_tamper_load_by_importer($importer->id);

  $form = array();
  $form['#tree'] = TRUE;

  $header = array(t('Name'), t('Plugin Type'), t('Operations'));

  foreach ($mappings as $mapping) {
    $form['mappings'][$mapping['source']] = array(
      '#type' => 'fieldset',
      '#title' => $mapping['target'],
    );
    $form['mappings'][$mapping['source']]['table'] = array();
    //$form['mappings'][$mapping['source']]['table']['#theme'] = 'feeds_tamper_ui_table';
    if (isset($instances[$mapping['source']])) {
      $table = array();
      foreach ($instances[$mapping['source']] as $instance) {
        $plugin = feeds_tamper_get_plugin($instance->plugin_id);
        $table[] = array(
          $instance->id,
          $plugin['name'],
          l(t('Edit'), 'admin/build/feeds_tamper/edit_plugin/' . $instance->id) . ' | ' .
          l(t('Remove'), 'admin/build/feeds_tamper/remove_plugin/' . $instance->id),
        );
      }
      $form['mappings'][$mapping['source']]['table']['#value'] = theme_table($header, $table);
    }
    $form['mappings'][$mapping['source']]['add_link']['#value'] =
    l(t('Add plugin'), 'admin/build/feeds_tamper/edit/' . $importer->id . '/add/' . $mapping['source']);
  }

  return $form;
}

function feeds_tamper_ui_add_plugin_form(&$form_state, $importer, $source) {
  $form_state['importer'] = $importer;
  $form_state['source'] = $source;
  $form = array();
  $feeds_tamper_plugins = feeds_tamper_get_plugins();


  $plugins = array();
  foreach ($feeds_tamper_plugins as $plugin_id => $plugin) {
    $plugins[$plugin['category']][$plugin_id] = $plugin['name'];
  }

  ksort($plugins);
  foreach ($plugins as &$p) {
    asort($p);
  }

  $form['id'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['plugin_id'] = array(
    '#title' => 'The plugin to add',
    '#type' => 'select',
    '#options' => $plugins,
    '#default_value' => '',
    '#tree' => TRUE,
  );

  ctools_include('dependent');
  foreach ($feeds_tamper_plugins as $machine_name => $plugin) {
    $i = str_replace('_', '-', $machine_name);
    $form['plugin_forms'][$machine_name] = array(
      '#title' => t('Configure ' . $plugin['name']),
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-plugin-id' => array($machine_name)),
      '#input' => TRUE,
      '#prefix' => '<div id="edit-'. $i .'-wrapper"><div id="edit-'. $i .'">',
      '#suffix' => '</div></div>'

    );
    foreach ($plugin['form']($importer, $source, array()) as $key => $form_item) {
      $form['plugin_forms'][$machine_name][$key] = $form_item;
    }
  }

  $form['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  return $form;
}

function feeds_tamper_ui_add_plugin_form_submit(&$form, &$form_state) {
  $obj = new stdClass();
  $obj->plugin_id = $form_state['values']['plugin_id'];
  $obj->id = $form_state['values']['id'];
  $obj->settings = $form_state['values'][$obj->plugin_id];
  $obj->importer = $form_state['importer']->id;
  $obj->source = $form_state['source'];
  $plugin = feeds_tamper_get_plugin($obj->plugin_id);
  if ($plugin['validate']) {
    $plugin['validate']($obj->settings);
  }
  drupal_write_record('feeds_tamper', $obj);
  drupal_goto('admin/build/feeds_tamper/edit/' . $obj->importer);
}

function feeds_tamper_ui_add_plugin_form_validate(&$form, &$form_state) {
  $plugin_id = $form_state['values']['plugin_id'];
  $plugin = feeds_tamper_get_plugin($plugin_id);
  if ($plugin['validate']) {
    $plugin['validate']($form_state['values'][$plugin_id]);
  }
}

function feeds_tamper_ui_edit_plugin_form(&$form_state, $instance) {
  $form_state['instance'] = $instance;
  $plugin = feeds_tamper_get_plugin($instance->plugin_id);
  $form = array();
  $form['#tree'] = TRUE;

  $importer = feeds_importer($instance->importer);
  foreach ($plugin['form']($importer, $instance->source, $instance->settings) as $key => $form_item) {
    $form[$instance->plugin_id][$key] = $form_item;
  }

  $form['add'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function feeds_tamper_ui_edit_plugin_form_submit($form, &$form_state) {
  $obj = new stdClass();
  $obj->plugin_id = $form_state['instance']->plugin_id;
  $obj->id = $form_state['instance']->id;
  $obj->settings = $form_state['values'][$obj->plugin_id];
  $obj->importer = $form_state['instance']->importer;
  $obj->source = $form_state['instance']->source;
  $plugin = feeds_tamper_get_plugin($obj->plugin_id);
  if ($plugin['validate']) {
    $plugin['validate']($obj->settings);
  }
  drupal_write_record('feeds_tamper', $obj, 'id');
  drupal_goto('admin/build/feeds_tamper/edit/' . $obj->importer);
}

function feeds_tamper_ui_remove_plugin_form(&$form_state, $instance) {
  $form_state['instance'] = $instance;
  $form = array();
  $form['remove'] = array('#type' => 'submit', '#value' => t('Remove'));
  return $form;
}

function feeds_tamper_ui_remove_plugin_form_submit($form, $form_state) {
  db_query("DELETE from {feeds_tamper} WHERE id = '%s'", $form_state['instance']->id);
  drupal_goto('admin/build/feeds_tamper/edit/' . $form_state['instance']->importer);
}
