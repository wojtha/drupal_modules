<?php
// $Id: atomrdf_parser.inc,v 1.1.2.3 2011/01/01 00:10:39 hefox Exp $

/**
 * @file
 * Contains the atomrd parsing functions.
 */

/**
 * Parse the feed into a data structure.
 *
 * @param $feed
 *  The feed object (contains the URL or the parsed XML structure.
 * @return
 *  stdClass The structured datas extracted from the feed.
 */
function atomrdf_parser_parse($string) {
  @ $xml = simplexml_load_string($string, NULL, LIBXML_NOERROR | LIBXML_NOWARNING | LIBXML_NOCDATA);

  // Got a malformed XML.
  if ($xml === FALSE || is_null($xml)) {
    return FALSE;
  }
  $feed_type = atomrdf_format_detect($xml);
  if ($feed_type ==  "atom1.0") {
    return atomrdf_parse($xml, $string);
  }

  return FALSE;
}

/**
 * Determine the feed format of a SimpleXML parsed object structure.
 *
 * @todo This whole function coudl stand to be cleaned up.
 * @param $xml
 *  SimpleXML-preprocessed feed.
 *
 * @return
 *  The feed format short description or FALSE if not compatible.
 */
function atomrdf_format_detect($xml) {
  if (!is_object($xml)) {
    return FALSE;
  }
  $attr = $xml->attributes();
  $type = drupal_strtolower($xml->getName());

  // If there are deleted-entry elements, then we know it's not just an Atom feed
  // but one with the Tombstone extension.
  if (in_array(FEEDS_ATOM_TOMBSTONE_NAMESPACE, $xml->getDocNamespaces())) {
    $deleted_entry = $xml->xpath('at:deleted-entry');
    if ($type == 'feed' &&  !empty($deleted_entry)) {
      return 'atom1.0';
    }
  }
  elseif ($type == 'feed' && !empty($xml->entry)) {
    return "atom1.0";
  }
  elseif ($type == "rss" && $attr["version"] == "2.0") {
    return "RSS2.0";
  }
  elseif ($type == "rdf" && isset($xml->channel)) {
    return "RDF";
  }
  elseif ($type == "rss" && $attr["version"] == "0.91") {
    return "RSS0.91";
  }
  elseif ($type == "rss" && $attr["version"] == "0.92") {
    return "RSS0.92";
  }
  return FALSE;
}

/**
 * Parse AtomRDF format - for example:
 *
 * <entry>
 *   <title>Week in DC Tech: April 12th Edition</title>
 *   <id>19</id>
 *   <updated>2010-04-29T12:02:55-07:00</updated>
 *   <published>2010-04-12T12:16:31-07:00</published>
 *   <author><name></name></author>
 *  <content type="application/rdf+xml" xml:lang="en">
 *    <RDF>
 *      <entity>
 *        <title>Week in DC Tech: April 12th Edition </title>
 *        <properties>
 *          <created>2010-04-12T12:16:31-07:00</created>
 *          <changed>2010-04-29T12:02:55-07:00</changed>
 *          <uid>0</uid>
 *          <status>1</status>
 *          <promote>1</promote>
 *        </properties>
 *      </entity>
 *      <field type="number_float" name="field_superlative">
 *        <field-instance>
 *          <column name="value">1234.57</column>
 *         </field-instance>
 *      </field>
 *    </RDF>
 *  </content>
 * </entry>
 *
 */

function atomrdf_parse($feed_XML, $feed_raw) {

  // declare normalized view of feed that will be returned when this method is done
  $parsed_source = array();

  // valid base?
  $base = (string) array_shift($feed_XML->xpath("@base"));
  if (!valid_url($base, TRUE)) {
    $base = FALSE;
  }

  /*
  // Unused right now:
  // Declare some canonical standard prefixes for well-known namespaces:
  static $canonical_namespaces = array(
    "content"  => "http://purl.org/rss/1.0/modules/content/",
    "dc"       => "http://purl.org/dc/elements/1.1/",
    'rdf'      => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#',
    'rdfs'     => 'http://www.w3.org/2000/01/rdf-schema#',
    'xsi'      => 'http://www.w3.org/2001/XMLSchema-instance#',
    'xsd'      => 'http://www.w3.org/2001/XMLSchema#',
    'owl'      => 'http://www.w3.org/2002/07/owl#',
    'dcterms'  => 'http://purl.org/dc/terms/',
    'dcmitype' => 'http://purl.org/dc/dcmitype/',
    'foaf'     => 'http://xmlns.com/foaf/0.1/',
    'rss'      => 'http://purl.org/rss/1.0/',
  );

  // Also get any other namespaces declared in the feed
  $namespaces = version_compare(phpversion(), '5.1.2', '<') ? array() : $feed_XML->getNamespaces(TRUE);
  */

  // feed metadata
  if (TRUE) {

    // feed title
    $parsed_source['title'] = isset($feed_XML->title) ? _parser_common_syndication_title("{$feed_XML->title}") : "";

    // feed description
    $parsed_source['description'] = isset($feed_XML->subtitle) ? "{$feed_XML->subtitle}" : "";

    // feed link
    $parsed_source['link'] = _parser_common_syndication_link($feed_XML->link);
    if (valid_url($parsed_source['link']) && !valid_url($parsed_source['link'], TRUE) && !empty($base)) {
      $parsed_source['link'] = $base . $parsed_source['link'];
    }
  }

  $parsed_source['items'] = array();

  // Parse out each entry in the feed.
  foreach ($feed_XML->entry as $news) {

    // Sometimes the id is the URL
    $original_url = NULL;
    $guid = !empty($news->id) ? "{$news->id}" : NULL;
    if (valid_url($guid, TRUE)) {
      $original_url = $guid;
    }

    /*
    // Unused right now:
    // Pull out RDF statements and normalize the prefixes
    // Actually there's no point in doing this since AtomRDF keeps RDF in a separate bucket.
    $rdf_data = array();
    foreach ($namespaces as $ns => $ns_uri) {
      foreach ($rss_item->attributes($ns_uri) as $attr_name => $attr_value) {
        $ns_prefix = ($ns_prefix = array_search($ns_uri, $canonical_namespaces)) ? $ns_prefix : $ns;
        $rdf_data[$ns_prefix .':'. $attr_name][] = (string)$attr_value;
      }
      foreach ($rss_item->children($ns_uri) as $rss_property) {
        $ns_prefix = ($ns_prefix = array_search($ns_uri, $canonical_namespaces)) ? $ns_prefix : $ns;
        $rdf_data[$ns_prefix .':'. $rss_property->getName()][] = (string)$rss_property;
      }
    }
    */

    // taxonomies

    $additional_taxonomies = array();
    if (isset($news->category)) {
      $additional_taxonomies['ATOM Categories'] = array();
      $additional_taxonomies['ATOM Domains'] = array();
      foreach ($news->category as $category) {
        if (isset($category['scheme'])) {
          $domain = "{$category['scheme']}";
          if (!empty($domain)) {
              if (!isset($additional_taxonomies['ATOM Domains'][$domain])) {
                $additional_taxonomies['ATOM Domains'][$domain] = array();
              }
              $additional_taxonomies['ATOM Domains'][$domain][] = count($additional_taxonomies['ATOM Categories']) - 1;
          }
        }
        $additional_taxonomies['ATOM Categories'][] = "{$category['term']}";
      }
    }

    // title
    $title = "{$news->title}";

    // body
    $body = '';
    if (!empty($news->content)) {
      foreach ($news->content->children() as $child)  {
        $body .= $child->asXML();
      }
      $body .= "{$news->content}";
    }
    elseif (!empty($news->summary)) {
      foreach ($news->summary->children() as $child)  {
        $body .= $child->asXML();
      }
      $body .= "{$news->summary}";
    }

    // author
    $author_found = FALSE;
    if (!empty($news->source->author->name)) {
      $original_author = "{$news->source->author->name}";
      if (!empty($news->source->author->email)) {
        $original_author_email = "{$news->source->author->email}";
      }
      if (!empty($news->source->author->url)) {
        $original_author_url = "{$news->source->author->url}";
      }
      $author_found = TRUE;
    }
    elseif (!empty($news->author->name)) {
      $original_author = "{$news->author->name}";
      if (!empty($news->author->email)) {
        $original_author_email = "{$news->author->email}";
      }
      if (!empty($news->author->url)) {
        $original_author_url = "{$news->author->url}";
      }
      $author_found = TRUE;
    }
    if (!empty($feed_XML->author->name) && !$author_found) {
      $original_author = "{$feed_XML->author->name}";
      if (!empty($feed_XML->author->email)) {
        $original_author_email = "{$feed_XML->author->email}";
      }
      if (!empty($feed_XML->author->url)) {
        $original_author_url = "{$feed_XML->author->url}";
      }
    }

    // some src elements in some valid atom feeds contained no urls at all
    $original_url = _parser_common_syndication_link($news->link);
    if (!$original_url && !empty($news->content['src'])) {
      if (valid_url("{$news->content['src']}", TRUE)) {
        $original_url = "{$news->content['src']}";
      }
    }

    // Parse out atom values and store them
    $item = array();
    $item['guid'] = !empty($guid) ? $guid : $item['url'];
    $item['url'] = trim($original_url);
    if (valid_url($item['url']) && !valid_url($item['url'], TRUE) && !empty($base)) {
      $item['url'] = $base . $item['url'];
    }
    $item['title'] = _parser_common_syndication_title($title, $body);
    $item['description'] = $body;
    $item['author_name'] = $original_author;
    $item['author_email'] = $original_author_email;
    $item['author_url'] = $original_author_url;
    $item['timestamp'] = _parser_common_syndication_parse_date(isset($news->published) ? "{$news->published}" : "{$news->issued}");
    $item['tags'] = isset($additional_taxonomies['ATOM Categories']) ? $additional_taxonomies['ATOM Categories'] : array();
    $item['domains'] = isset($additional_taxonomies['ATOM Domains']) ? $additional_taxonomies['ATOM Domains'] : array();

    // Parse out RDF values
    $rdf = $news->content ? $news->content : $news;
    $rdfns1 = $rdf->children("http://www.w3.org/1999/02/22-rdf-syntax-ns");
    if ($rdfns1) {
      $rdfns2 = $rdfns1->children("http://drupal.org/");
      if ($rdfns2) {
        $item['rdf'] = atomrdf_custom_copy($rdfns2);
      }
    }

    // Store this item
    $parsed_source['items'][] = $item;

  }

  // Add any deleted items to the parsed data.  These items follow the proposed
  // Atom Tombstone specification: http://tools.ietf.org/html/draft-snell-atompub-tombstones-06
  // @todo Ignore deleted-entry items that have a newer entry item.
  //$parsed_source['deleted'] = array();
    if (in_array(FEEDS_ATOM_TOMBSTONE_NAMESPACE, $feed_XML->getDocNamespaces())) {
    foreach ($feed_XML->xpath('at:deleted-entry') as $deleted) {
      $item = array(
        'guid' => "{$deleted['ref']}",
        'deleted' => TRUE,
        'deleted_when' => "{$deleted['when']}",
        'deleted_by' => array(
          'name' => '',
          'email' => '',
        ),
      );
      // We really should also be parsing out the by, name, and email information, too.
      // However, because the XML namespace changes for those sub elements it is
      // ridiculously hard to parse out in PHP, due to PHP sucking.  Since they're
      // not required parts of the tombstone definition, and not needed for just
      // delete-and-forget processing, we're going to ignore them.
      // @todo When we figure out how to parse that XML in the insanity that is
      // PHP, add that in here as well.

      $parsed_source['items'][] = $item;

      //$parsed_source['deleted']["{$deleted['ref']}"] = $record;
    }
  }

  // Return list of items
  return $parsed_source;
}

/*
 * Parse values from our custom format.
 * The format consists of an 1) array of properties and 2) an array of cck properties
 * TODO detect if a field_* property is a date or a sub-array and parse it out properly
 */
function atomrdf_custom_copy($xml) {
  $blob = array();
  foreach ($xml->children() as $xml_child) {
    $key = $xml_child->getName();
    if ($key == "field") {
      // The name 'field-instance' has a dash in it! So we are trying to get it without referring to it.
      // @todo This can easily be done with the xpath() method, or with $xml_child->{'field-instance'}

      $i = 0;
      $values = array();
      // Each field instance is a value of possible multi values
      foreach ($xml_child->children() as $field_instance) {
        $values[$i] = array();
        // each column makes up information for the value
        foreach ($field_instance->children() as $column) {
          $attributes = $column->attributes();
          if (!empty($attributes['name'])) {
            $name = (string) $attributes['name'];
            $value = (string) $column;
            if (!empty($attributes['serialize'])) {
              $value = unserialize($value);
            }
            $values[$i][$name] = $value;
          }
        }
        $i++;
      }
      $key = "{$xml_child['name']}";
      $blob[$key] = $values;

      $blob[$key]['#attributes'] = array('type' => (string) $xml_child['type']);
    }
    elseif ($key == "properties") {
      // Save properties from the properties block
      foreach ($xml_child->children() as $property) {
        $key = "{$property->getName()}";
        $value = "{$property}";
        $blob[$key] = $value;
      }
    }
    elseif ($key == 'taxonomy') {
      foreach ($xml_child->children() as $term) {
        $values = array();
        foreach ($term as $data_type => $value) {
          $values[$data_type] = "{$value}";
        }
        $blob[$key][] = $values;
      }
    }
    else {
      // There are some 'naked' properties that are not inside of the properties or fields block - save those.
      $value = "{$xml_child}";
      $blob[$key] = $value;
    }
  }

  // Invokes hook_feeds_atom_atomrdf_parser_alter().
  drupal_alter('feeds_atom_atomrdf_parser', $blob, $xml);

  return $blob;
}

/*
 * currently unused:
 * This is a dumb bulk copy of properties from our rdf blob.
 */

function atomrdf_bulk_copy($xml) {
  if ($xml) {
    $count = 0;
    $valid_count = 0;
    $blob = array();
    foreach ($xml->children() as $xml_child) {
      $count = $count + 1;
      $key = $xml_child->getName();
      $value = atomrdf_bulk_copy($xml_child);
      if ($key) {
        $blob[$key] = $value;
        $valid_count = $valid_count + 1;
      }
    }
    if ($count > 0) {
      if ($valid_count > 0) {
        return $blob;
      }
    }
    else {
      return "{$xml}";
    }
  }
  return NULL;
}

/**
 * Parse a date comes from a feed.
 *
 * @param $date_string
 *  The date string in various formats.
 * @return
 *  The timestamp of the string or the current time if can't be parsed
 */
function _parser_common_syndication_parse_date($date_str) {
  // PHP < 5.3 doesn't like the GMT- notation for parsing timezones.
  $date_str = str_replace("GMT-", "-", $date_str);
  $date_str = str_replace("GMT+", "+", $date_str);
  $parsed_date = strtotime($date_str);
  if ($parsed_date === FALSE || $parsed_date == -1) {
    $parsed_date = _parser_common_syndication_parse_w3cdtf($date_str);
  }
  return $parsed_date === FALSE ? time() : $parsed_date;
}

/**
 * Parse the W3C date/time format, a subset of ISO 8601.
 *
 * PHP date parsing functions do not handle this format.
 * See http://www.w3.org/TR/NOTE-datetime for more information.
 * Originally from MagpieRSS (http://magpierss.sourceforge.net/).
 *
 * @param $date_str
 *   A string with a potentially W3C DTF date.
 * @return
 *   A timestamp if parsed successfully or FALSE if not.
 */
function _parser_common_syndication_parse_w3cdtf($date_str) {
  if (preg_match('/(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2})(:(\d{2}))?(?:([-+])(\d{2}):?(\d{2})|(Z))?/', $date_str, $match)) {
    list($year, $month, $day, $hours, $minutes, $seconds) = array($match[1], $match[2], $match[3], $match[4], $match[5], $match[6]);
    // Calculate the epoch for current date assuming GMT.
    $epoch = gmmktime($hours, $minutes, $seconds, $month, $day, $year);
    if ($match[10] != 'Z') { // Z is zulu time, aka GMT
      list($tz_mod, $tz_hour, $tz_min) = array($match[8], $match[9], $match[10]);
      // Zero out the variables.
      if (!$tz_hour) {
        $tz_hour = 0;
      }
      if (!$tz_min) {
        $tz_min = 0;
      }
      $offset_secs = (($tz_hour * 60) + $tz_min) * 60;
      // Is timezone ahead of GMT?  If yes, subtract offset.
      if ($tz_mod == '+') {
        $offset_secs *= -1;
      }
      $epoch += $offset_secs;
    }
    return $epoch;
  }
  else {
    return FALSE;
  }
}

/**
 * Extract the link that points to the original content (back to site or
 * original article)
 *
 * @param $links
 *  Array of SimpleXML objects
 */
function _parser_common_syndication_link($links) {
  $to_link = '';
  if (count($links) > 0) {
    foreach ($links as $link) {
      $link = $link->attributes();
      $to_link = isset($link["href"]) ? "{$link["href"]}" : "";
      if (isset($link["rel"])) {
        if ("{$link["rel"]}" == 'alternate') {
          break;
        }
      }
    }
  }
  return $to_link;
}

/**
 * Prepare raw data to be a title
 */
function _parser_common_syndication_title($title, $body = FALSE) {
  if (empty($title) && !empty($body)) {
    // Explode to words and use the first 3 words.
    $words = preg_split("/[\s,]+/", strip_tags($body));
    $title = $words[0] .' '. $words[1] .' '. $words[2];
  }
  return $title;
}

