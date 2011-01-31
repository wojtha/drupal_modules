<?php

/**
 * Parses Atom RDF feeds and allows you to define your own mapping for feeds
 */
class FeedsAtomRDFCustomParser extends FeedsParser {

  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    require_once './' . drupal_get_path('module','feeds_atom') . '/libraries/atomrdf_parser.inc';
    $result = atomrdf_parser_parse($batch->getRaw());
    $batch->title = $result['title'];
    $batch->description = $result['description'];
    $batch->link = $result['link'];
    if (is_array($result['items'])) {
      $batch->setItems($result['items']);
    }
  }

  /**
   * Get an element identified by $element_key of the given item.
   * The element key corresponds to the values in the array returned by
   * FeedsParser::getMappingSources().
   *
   * This method is invoked from FeedsProcessor::map() when a concrete item is
   * processed.
   *
   * @ingroup mappingapi
   *
   * @param $batch
   *   FeedsImportBatch object containing the sources to be mapped from.
   * @param $element_key
   *   The key identifying the element that should be retrieved from $source
   *
   * @return
   *   The source element from $item identified by $element_key.
   *
   * @see FeedsProcessor::map()
   * @see FeedsCSVParser::getSourceElement().
   */
  public function getSourceElement(FeedsImportBatch $batch, $element_key) {
    if (($node = $batch->feedNode()) && $element_key == 'parent:uid') {
      return $node->uid;
    }
    $item = $batch->currentItem();
    $fname = substr($element_key,0,6);
    if ($fname == 'field_') {
      $type = $item['rdf'][$element_key]['#attributes']['type'];
      switch ($type) {
        case 'text':
          return $item['rdf'][$element_key][0]['value'];
          break;
        case 'filefield':
          return $item['rdf'][$element_key][0]['full_url'];
          break;
        case 'email':
          return $item['rdf'][$element_key][0]['email'];
          break;
        case 'nodereference':
          if ($item['rdf'][$element_key][1]) {
            $return = array();
            foreach ($item['rdf'][$element_key] as $elt) {
              if ($elt['guid']) {
                $return[] = $elt['guid'];
              }
            }
            return $return;
          }
          else {
            return $item['rdf'][$element_key][0]['guid'];
          }
          break;
        default:
          if ($item['rdf'][$element_key][0]['value']) {
            return $item['rdf'][$element_key][0]['value'];
          }
          else {
            return '';
          }
      }
    }
    elseif (in_array($element_key, array('body', 'created', 'changed', 'uid'))) {
      if (in_array($element_key, array('created', 'changed'))) {
        return strtotime($item['rdf'][$element_key]);
      }
      elseif ($element_key == 'uid') {
        $user = user_load(array('mail' => $item['author_email']));
        return $user->uid;
      }
      else {
        return $item['rdf'][$element_key];
      }
    }
    elseif ($fname == 'taxono') { // Taxonomy
      // Get vocabulary name
      $tax_array = explode(':', $element_key);
      $vocabulary = $tax_array[1];
      $return_array = array();
      foreach ($item['rdf']['taxonomy'] as $tag) {
        if ($tag['vocabulary'] == $vocabulary) {
          $return_array[] = $tag['label'];
        }
      }
      return $return_array;
    }
    else {
      return isset($item[$element_key]) ? $item[$element_key] : '';
    }
  }
}
