<?php

$xmlstr = <<<EOB
<item quest="0" identified="1" disabled="0" dup="0" socketed="0" illegal="0" ear="0" starter="0" 
simple="0" ethereal="0" inscribed="0" runeword="0" type="knif" width="1" height="3" nodurability="0"
 stackable="0" location="0" bodypos="0" row="5" col="0" storedin="5">
 <d2i>Sk0QAIAAZQAKmrMmB4LUsZJS66EKMCZUeCy0PvAgB3306esiW5Oka6ZwDRdakv0H</d2i>
 <type>9kr</type><guid>a52563a9</guid><level>86</level><qual>7</qual><UID>170</UID>
 <maxDur>24</maxDur><curDur>19</curDur>
 <properties>
 	<property id="21"><stat1>15</stat1></property>
 	<property id="22"><stat1>45</stat1></property>
 	<property id="31"><stat1>60</stat1></property>
 	<property id="57"><stat1>500</stat1><stat2>500</stat2><stat3>250</stat3></property>
 	<property id="93"><stat1>50</stat1></property>
 	<property id="107"><stat1>73</stat1><stat2>5</stat2></property>
 	<property id="107"><stat1>83</stat1><stat2>4</stat2></property>
 	<property id="107"><stat1>92</stat1><stat2>4</stat2></property>
 	<property id="150"><stat1>50</stat1></property>
 	</properties></item>
EOB
;

$xml = simplexml_load_string($xmlstr);

echo '<img src="itemgfx.php?code='.$xml->type.'&qual='.$xml->qual.'" alt="" /><br />';

require_once '../classes/d2pt.properties.class.php';

echo '<b>Magic properties:</b><ul>';
$properties = D2PT_properties::getInstance();

foreach ($xml->properties->property as $prop) {
	echo '<li>'.$properties->getPropertyStr($prop['id'], $prop->stat1, $prop->stat2, $prop->stat3, $prop->stat4).'</li>';	
}
echo '</ul>';

//var_dump($properties->getProperty(54));


