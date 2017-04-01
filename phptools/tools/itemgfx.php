<?php

require_once '../classes/d2pt.dc6.class.php';
require_once '../classes/d2pt.configfile.class.php';

$armor = new D2PT_configfile('../atma_config/Armor.txt');
$misc = new D2PT_configfile('../atma_config/Misc.txt');
$weapon = new D2PT_configfile('../atma_config/Weapons.txt');

$item = false;

$code = isset($_GET['code']) ? $_GET['code'] : false;
$quality = isset($_GET['qual']) ? intval($_GET['qual']) : 1;
$gfxvar = isset($_GET['gfxvar']) ? infcal($_GET['gfxvar']) : 0;

if (!$item) $item = $armor->select('code', $code, true);
if (!$item) $item = $misc->select('code', $code, true);
if (!$item) $item = $weapon->select('code', $code, true);

if (!$item) {
	header('404 - NOT FOUND');
	header('Content-type: text/plain');
	echo "404 - NOT FOUND";
	exit(0);	
}

$gfx_file = $item['invfile'];
if ($quality == '5' && $item['setinvfile'])
	$gfx_file = $item['setinvfile'];
if ($quality > 6 && $item['uniqueinvfile'])
	$gfx_file = $item['uniqueinvfile'];
	
$gfx = new D2PT_dc6('../atma_gfx/'.$gfx_file.'.dc6');

$img = $gfx->getImage(0, $gfxvar);

if (is_null($img)) {
	header('404 - NOT FOUND');
	header('Content-type: text/plain');
	echo "404 - NOT FOUND";
	exit(0);
}

header ('Content-type: image/png');
imagepng($img);
