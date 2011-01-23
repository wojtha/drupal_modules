<?php

/**
 * Parses Atom RDF feeds.
 */
class FeedsAtomRDFParser extends FeedsParser {

  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    require_once './' . drupal_get_path('module','feeds_atom') . '/libraries/atomrdf_parser.inc';
    $result = atomrdf_parser_parse($batch->getRaw());
    $batch->setTitle($result['title']);
    $batch->setDescription($result['description']);
    $batch->setLink($result['link']);
    if (is_array($result['items'])) {
      $batch->setItems($result['items']);
    }
  }

  /**
   * Return mapping sources.
   *
   * At a future point, we could expose data type information here,
   * storage systems like Data module could use this information to store
   * parsed data automatically in fields with a correct field type.
   */
  public function getMappingSources() {
    return array(
      'title' => array(
        'name' => t('Title'),
        'description' => t('Title of the feed item.'),
      ),
      'description' => array(
        'name' => t('Description'),
        'description' => t('Description of the feed item.'),
      ),
      'author_name' => array(
        'name' => t('Author name'),
        'description' => t('Name of the feed item\'s author.'),
      ),
      'author_email' => array(
        'name' => t('Author email'),
        'description' => t('Email of the feed item\'s author.'),
      ),
      'author_url' => array(
        'name' => t('Author url'),
        'description' => t('Url of the feed item\'s author.'),
      ),
      'timestamp' => array(
        'name' => t('Published date'),
        'description' => t('Published date as UNIX time GMT of the feed item.'),
      ),
      'url' => array(
        'name' => t('Item URL (link)'),
        'description' => t('URL of the feed item.'),
      ),
      'guid' => array(
        'name' => t('Item GUID'),
        'description' => t('Global Unique Identifier of the feed item.'),
      ),
      'tags' => array(
        'name' => t('Categories'),
        'description' => t('An array of categories that have been assigned to the feed item.'),
      ),
    );
  }
}
