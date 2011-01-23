OVERVIEW

This module parses a custom payload inside of an Atom wrapper.

It requires the feeds module.

It is spaces / groups aware.

Here is an example of what it parses - the payload can be seen in the <properties> and <field> area

<?xml version="1.0" encoding="utf-8" ?>
<feed xmlns="http://www.w3.org/2005/Atom">
  <title>drupal.makerlab.org | Atom</title>
  <link href="http://drupal.makerlab.org"/>
      <link rel="hub" href="http://drupal.makerlab.org/pubsubhubbub/endpoint" />
    <link rel="self" href="http://drupal.makerlab.org/atom-test" />
    <updated>2010-05-28T15:19:40-07:00</updated>
  <generator uri="http://drupal.makerlab.org">drupal.makerlab.org</generator>
  
<entry>
  <title>Hello Group</title>
  <id>306</id>
  <updated>2010-05-28T15:11:37-07:00</updated>
  <published>2010-05-28T15:11:37-07:00</published>
  <author>
    <name>hello</name>
  </author>
  <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns">
  <entity xmlns="http://drupal.org/" type="node" bundle="group">
    <title>Hello Group</title>
    <properties>
      <nid>306</nid>
      <type>group</type>
      <uid>1</uid>
      <status>1</status>
      <created>2010-05-28T15:11:37-07:00</created>
      <changed>2010-05-28T15:11:37-07:00</changed>
      <comment>2</comment>
      <promote>1</promote>
      <vid>306</vid>
      <revision_uid>1</revision_uid>
      <title>Hello Group</title>
      <body>Groups are the bomb.</body>
      <teaser>Groups are the bomb.</teaser>
      <revision_timestamp>1275084697</revision_timestamp>
      <format>1</format>
      <name>hello</name>
      <data>a:1:{s:13:"form_build_id";s:37:"form-a78cd812f8f75cb0c1ac8fdc9d1c9ebe";}</data>
      <og_description>group</og_description>
      <og_directory>1</og_directory>
      <og_public>1</og_public>
      <last_comment_timestamp>1275084697</last_comment_timestamp>
    </properties>
  </entity>
</rdf:RDF>
</entry>


<entry>
  <title>hello</title>
  <id>308</id>
  <updated>2010-05-28T15:19:40-07:00</updated>
  <published>1969-12-31T16:33:30-08:00</published>
  <author>
    <name>hello</name>
  </author>
  <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns">
  <entity xmlns="http://drupal.org/" type="node" bundle="story">
    <title>hello</title>
    <properties>
      <nid>308</nid>
      <type>story</type>
      <uid>1</uid>
      <status>1</status>
      <created>1969-12-31T16:33:30-08:00</created>
      <changed>2010-05-28T15:19:40-07:00</changed>
      <comment>2</comment>
      <promote>1</promote>
      <vid>308</vid>
      <revision_uid>1</revision_uid>
      <title>hello</title>
      <body>welcome to the world of content </body>
      <teaser>welcome to the world of content </teaser>
      <log>Created/updated by FeedsNodeProcessor</log>
      <revision_timestamp>1275085180</revision_timestamp>
      <format>1</format>
      <name>hello</name>
      <data>a:1:{s:13:"form_build_id";s:37:"form-a78cd812f8f75cb0c1ac8fdc9d1c9ebe";}</data>
      <og_public>1</og_public>
      <last_comment_timestamp>1275085180</last_comment_timestamp>
    </properties>
    <field type="text" name="field_superlative">
      <field-instance>
        <column name="value">super</column>
      </field-instance>
    </field>
  </entity>
</rdf:RDF>
</entry>

</feed>

HOW TO USE:

1) Add and activate the module
2) Go to feeds management ( http://../admin/build/feeds ) and add a custom feed manager or override the default feed manager
3) Edit your feed manager
4) Set your feed parser to FeedATOMRDFParser
5) Set your feed processor to FeedATOMRDFProcessor
6) Find yourself a feed like the above ( the above is produced by the views_atom module )
7) Make a feed - a feed is an ordinary content object - so just go to content making and then make a feed.
8) The title of the feed you are making does not matter.
9) The link of the feed you are making should point to the RDF blob like above. 
10) Import the feed!


