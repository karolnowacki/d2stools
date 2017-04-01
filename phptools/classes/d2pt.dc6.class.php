<?php

$_D2PT_dc6_Palete = array(
	0,0,0,0,0,36,8,24,28,16,36,44,24,52,60,0,0,92,32,64,72,40,72,84,0,0,144,16,
	72,140,0,0,188,32,132,208,108,196,244,80,124,140,100,156,172,8,12,12,16,16,
	20,28,28,28,28,36,40,44,44,44,48,56,60,56,56,56,72,72,72,72,80,88,52,88,100,
	88,88,88,60,100,116,100,100,100,112,116,124,132,132,132,148,152,156,196,196,
	196,244,244,244,4,4,8,0,4,16,4,8,24,8,16,24,16,20,28,4,12,36,12,24,32,20,24,
	32,8,16,44,16,28,36,12,32,40,4,8,56,16,28,48,20,40,48,12,20,64,16,40,56,4,
	12,72,28,40,56,12,32,76,20,44,68,32,44,64,8,16,88,32,52,72,40,52,68,28,40,
	84,20,28,92,8,36,92,24,56,84,36,56,84,16,24,108,16,44,104,36,68,92,24,36,
	112,36,60,104,12,44,124,44,76,100,8,72,112,24,40,128,40,80,116,36,48,136,64,
	80,108,12,52,140,104,32,112,68,88,120,64,64,140,48,92,132,36,52,156,24,40,
	164,20,72,164,64,100,140,56,112,140,80,104,140,52,68,176,32,76,180,88,112,
	152,64,120,160,28,96,188,72,132,156,84,84,196,32,108,200,88,124,172,72,136,
	176,84,124,200,32,112,224,84,156,184,0,44,252,112,140,192,80,152,204,48,132,
	228,112,112,224,120,152,208,60,136,248,56,160,236,100,184,216,132,164,224,68,
	180,240,76,192,244,140,176,240,92,212,252,176,176,252,16,36,4,24,36,20,24,60,
	32,12,72,24,48,68,0,8,100,24,36,92,36,36,92,56,68,108,8,20,124,40,52,116,64,
	48,120,88,28,156,52,56,132,112,52,160,72,76,144,88,40,188,68,72,152,132,76,
	184,96,0,252,24,92,220,116,124,208,140,136,252,160,40,12,12,72,24,24,88,0,0,
	68,20,56,104,36,16,100,60,40,120,40,40,128,16,76,132,76,56,148,48,48,140,96,
	72,160,92,56,172,80,80,172,108,76,188,120,84,216,96,36,208,120,100,224,144,
	100,220,160,128,252,32,164,240,132,132,252,160,160,252,184,144,144,140,88,
	164,160,104,196,192,132,212,208,152,252,204,168,244,244,204,128,160,192,168,
	192,196,148,196,224,116,232,252,176,252,196,164,228,252,196,252,252,4,4,4,8,
	8,8,12,12,12,16,16,16,20,20,20,24,24,24,24,28,36,32,32,32,36,36,36,40,40,40,
	32,40,48,48,48,48,40,48,56,52,52,52,60,56,52,52,56,68,60,60,60,48,60,76,64,
	64,64,60,64,68,68,68,68,60,72,80,56,68,88,76,76,76,60,76,96,92,88,52,80,80,
	80,84,84,84,92,92,92,80,92,104,96,96,96,116,112,68,80,100,124,104,104,104,
	108,108,108,96,108,120,112,112,112,116,116,116,124,124,124,100,128,148,116,
	132,144,112,136,172,144,144,144,132,148,156,184,148,128,160,160,160,152,172,
	176,172,172,172,184,184,184,204,204,204,216,216,216,252,204,204,228,228,228,
	128,0,252,8,20,16,8,28,20,8,32,20,12,36,24,12,44,32,24,52,32,28,56,48,68,32,
	0,104,0,24,4,8,8,16,20,20,20,24,24,20,28,28,16,32,28,24,32,36,20,40,36,32,40,
	40,36,40,44,48,44,32,36,44,48,40,48,52,44,52,56,64,60,48,52,60,64,56,68,72,
	68,76,80,76,88,92,84,92,96,88,100,104,255,255,255);


class D2PT_dc6 {
	
	protected $init = false;
	
	protected $header = array(
		'version' => 0,
		'unk1' => 0,
		'unk2' => 0,
		'termination' => '',
		'directions' => 0,
		'frames_per_dir' => 0
	);
	
	protected $offsets = array();
	
	protected $frames = array();
	
	protected static function create_image($width, $height) {
		global $_D2PT_dc6_Palete;
		$img = imagecreate($width, $height);
		imagesavealpha($img, true);
		imagealphablending($img, false);
		$bg = imagecolorallocatealpha($img, 0, 0, 0, 127);
		imagefilledrectangle($img, 0, 0, $width, $height, $bg);
		for ($i=1; $i<256; ++$i) {
			imagecolorallocate($img, $_D2PT_dc6_Palete[3*$i+2], $_D2PT_dc6_Palete[3*$i+1], $_D2PT_dc6_Palete[3*$i]).'<br />';			
		}
		return $img;
	}
	
	public function __construct($file) {
		
		if (!is_readable($file))
			return;
			
		$fh = fopen($file, "rb");
		
		$this->header = unpack('Vversion/Vunk1/Vunk2', fread($fh, 12));
		$this->header['termination'] = fread($fh, 4);
		$this->header += unpack('Vdirections/Vframes_per_dir', fread($fh, 8));
		
		for ($i = 0; $i < $this->header['directions']; ++$i) {
			$this->offsets[$i] = array_values(unpack('V*0', fread($fh, 4*$this->header['frames_per_dir'])));		
		}
		
		for ($i = 0; $i < $this->header['directions']; ++$i) 
			for ($j = 0; $j < $this->header['frames_per_dir']; ++$j) {
				$frame = unpack('Vflip/Vwidth/Vheight/Voffset_x/Voffset_y/Vunk/Vnext_block/Vlength', fread($fh, 32));
				
				$frame['block'] = fread($fh, $frame['length']);
				
				$frame['img'] = self::create_image($frame['width'], $frame['height']);
				$x = 0; $y = $frame['height']-1; $pos = 0;
				
				while ($pos < $frame['length']) {
					$byte = ord($frame['block']{$pos});
					
					if ($byte == 0x80) {
						$y--; $x = 0;
					} elseif ($byte & 0x80) {
						$x += $byte & 0x7F;
					} else {
						while ($byte--)
							//imagesetpixel($frame['img'], $x, $y, 220);
							imagesetpixel($frame['img'], $x++, $y, ord($frame['block']{++$pos}));
					}
					++$pos;	
				}
										
				$this->frames[$i][$j] = $frame;
			}			
		
		
		fclose($fh);
		$this->init = true;
		
	}
	
	public function frameExists($dir, $frame) {
		if (!$this->init)
			return false;
		if ($dir < 0 || $frame < 0 || $dir >= $this->header['directions'] || $frame >= $this->header['frames_per_dir'])
			return false;
		return true;		
	}
	
	public function getWidth($dir = 0, $frame = 0) {
		if (!$this->frameExists($dir, $frame))
			return 0;
		return $this->frames[$dir][$frame]['width'];
	}
	
	public function getHeight($dir = 0, $frame = 0) {
		if (!$this->frameExists($dir, $frame))
			return 0;
		return $this->frames[$dir][$frame]['height'];
	}
	
	public function getImage($dir = 0, $frame = 0) {
		if (!$this->frameExists($dir, $frame))
			return NULL;
		return $this->frames[$dir][$frame]['img'];
	}
	
	public function getDirections() {
		if (!$this->init)
			return 0;
		return $this->header['directions'];
	}
	
	public function getFrames() {
		if (!$this->init)
			return 0;
		return $this->header['frames_per_dir'];
	}
	
	
	
	
	
	
}
