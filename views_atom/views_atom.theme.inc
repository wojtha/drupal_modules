<?php
// $Id: views_atom.theme.inc,v 1.1.2.4 2010/12/31 22:07:37 hefox Exp $


function template_preprocess_views_view_views_atom(&$vars) {
  $view = $vars['view'];
  $options = $view->style_options;

  // Set basic info about the feed.
  $vars['view_title'] = variable_get('site_name', 'Drupal') . ' | ' . $view->get_title();
  $vars['description'] = ($options['description']['feed_description']) ? $options['description']['feed_description'] :  variable_get('site_mission', '');

  // Base URL for link tag
  global $base_url;
  $vars['link'] = $base_url;

  $vars['site_name'] = variable_get('site_name', 'Drupal');


  // If the push_hub module is enabled, we want to enable PuSH functionality
  // for this feed, too.
  $vars['use_push'] = FALSE;
  if (module_exists('push_hub')) {
    $vars['use_push'] = TRUE;

    // This is the URL that subscribers should contact to register as subscribers.
    $vars['hub_url'] = url('pubsubhubbub/endpoint', array('absolute' => TRUE));

    // This is the URL of the feed itself.
    $vars['feed_url'] = filter_xss_admin('http://'. $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI']);
  }
}

function template_preprocess_views_view_views_atom_fields(&$vars) {
  // Inherit the base processing for all atom feeds.
  template_preprocess_views_view_views_atom($vars);

  $view = $vars['view'];
  $options = $view->style_options;

  $vars['content'] = '';
  $updated = 0;
  foreach ($vars['rows'] as $i => $content) {
    $item = $view->result[$i];

    // We need to know the updated time of the most recently updated entry
    // so that we can set the whole feed's updated time to that.
    if (!empty($options['fields']['atom_updated']) && !empty($view->field[$options['fields']['atom_updated']])) {
      $alias = $view->field[$options['fields']['atom_updated']]->field_alias;
      $updated = max($item->$alias, $updated);
    }
    $vars['content'] .= theme('views_atom_fields_item', $content, $view, $item);
  }

  // If this is an empty feed, set the last updated time to right now.
  if (!$updated) {
    $updated = time();
  }

  $vars['updated'] = date('c', $updated);

}

function template_preprocess_views_view_views_atom_delete(&$vars) {
  global $user;

  // Inherit the base processing for all atom feeds.
  template_preprocess_views_view_views_atom($vars);

  $items = array();

  $now = date('c');

  $vars['updated'] = $now;

  $vars['items'] = array();
  
  // @todo Find a way to fix this really dirty hack !!!!
  if (empty($vars['rows'])) {
    $i = 0;
    while (arg($i) != '') {
      if (is_numeric(arg($i))) {
        $nid = arg($i);
        $node = new stdClass();
        $node->nid = $nid;
        $vars['rows'][0] = $node;
        break;
      }
      $i++;
    }
  }
  
  $guid_options = isset($vars['view']->style_options['guid']) ? $vars['view']->style_options['guid'] : array();
  // Now process the rows into deletion information.
  foreach ($vars['rows'] as $i => $row) {
    $item = array(
      'guid' => views_atom_guid('node', $row->nid, $guid_options),
      'when' => $now,
      'by' => array(
        'name' => '',
        'email' => '',
      ),
    );

    if ($user->uid) {
      $item['by']['name'] = $user->name;
      $item['by']['email'] = $user->mail;
    }

    $vars['items'][] = $item;
  }

  // Set Headers
  //drupal_set_header('Content-Type:  text/plain; charset=utf-8');

}


/**
 * Preprocess for theme('views_atom_fields_item').
 */
function template_preprocess_views_atom_fields_item(&$vars) {

  $item = $vars['item'];
  $view = $vars['view'];
  $options = $view->style_options;

  $vars['entry_type'] = ($options['entry_type'] == 'xml') ? $options['entry_type_custom'] : $options['entry_type'];

  $nid_alias = isset($view->field['nid']) ? $view->field['nid']->field_alias : 'nid';
  
  // Figure out which results go with the row we're working with.
  foreach ($view->result as $key => $result) {
    if ($result->{$nid_alias} == $item->{$nid_alias}) {
      $result_id = $key;
    }
  }

  foreach ($options['fields'] as $field => $column) {
    $alias = $view->field[$column]->field_alias;
    if (!empty($item->$alias)) {
      $vars[$field] = filter_xss_admin($item->$alias);
    }
    else if (!empty($view->style_plugin->rendered_fields[$result_id][$column])) {
      $vars[$field] = filter_xss_admin($view->style_plugin->rendered_fields[$result_id][$column]);
    } else {
      $vars[$field] = '';
    }
  }
  $guid_options = isset($view->style_options['guid']) ? $view->style_options['guid'] : array();
  $vars['atom_uuid'] = views_atom_guid('node', $item->{$nid_alias}, $guid_options);

  if (empty($vars['atom_url'])) {
    // If the user did not set up an atom_url in the view, set a default.
    // In contrast to what views_atom_guid() currently provides, this URL
    // includes the path alias and possible associated PURL.
    $vars['atom_url'] = url('node/'. $item->{$nid_alias}, array('absolute' => TRUE));
  }
  if ($vars['atom_published']) {
    // format_date() doesn't support the 'c' formatting code.
    $vars['atom_published'] = date('c', $vars['atom_published']);
  }
  if ($vars['atom_updated']) {
    // format_date() doesn't support the 'c' formatting code.
    $vars['atom_updated'] = date('c', $vars['atom_updated']);
  }

  $vars['language'] = $GLOBALS['language']->language;
}


/**
 * Theme function for feed icon.
 */
function theme_views_atom_feed_icon($url, $title, $icon) {
  if ($image = theme('image', $icon, t('Download Atom Feed'), $title)) {
    return '<a href="'. check_url($url) .'" class="feed-icon">'. $image .'</a>';
  }
}
