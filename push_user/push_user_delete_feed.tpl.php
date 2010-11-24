<?php print "<?xml"; ?> version="1.0" encoding="utf-8" <?php print "?>"; ?>
<feed xmlns="http://www.w3.org/2005/Atom" xmlns:at="http://purl.org/atompub/tombstones/1.0">

  <title><?php print $feed_title; ?></title>
  <link rel="hub" href="<?php print $hub ?>" />
  <link rel="self" href="<?php print $feed_url ?>" />
  <link href="<?php print $user_url ?>"/>
  <updated><?php print $updated ?></updated>
  <author>
    <name><?php print $site_name ?></name>
    <uri><?php print $site_url ?></uri>
  </author>
  <id><?php print $feed_url ?></id>

  <at:deleted-entry ref="<?php print $user_openid; ?>" when="<?php print $updated; ?>">
  </at:deleted-entry>

</feed>
