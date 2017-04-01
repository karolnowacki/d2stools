<?php

require_once dirname(__FILE__).'/d2pt.configfile.class.php';

class D2PT_properties {
	
	private static $oInstance = NULL;
	protected $propconf = NULL;
	
	/**
	 * @return D2PT_properties
	 */
	public static function getInstance() {
        if(is_null(self::$oInstance)) {
            self::$oInstance = new self();
        }
        return self::$oInstance;
    }
	
	private function __construct() {
		$this->propconf = new D2PT_configfile('../atma_config/ATMA_properties.txt');					
	}
	
	public function getProperty($id) {
		$id = intval($id);
		return $this->propconf->select('Id', $id, true);		
	}
	
	
	
	protected function convertValue($value, $bias, $convfunc) {
		$stat = $value - $bias;
		
		switch ($convfunc) {
			case "INTEGER" : return intval($stat);
			case "DURATION" : return $stat/25;
			case "POISON_DAMAGE" : return $stat/256;
			
			case "SKILL" : 
				$skills = new D2PT_configfile('../atma_config/Skills.txt');
				$sk = $skills->select('Id', $stat, true);
				return $sk['skill'];
			
			case "ELEMENT" :
				$elements = array('Cold', 'Fire', 'Lightning', 'Poison', 'Magic');
				if (isset($elements[$stat]))
					return $elements[$stat];
				return "_UNKNOWN_ELEMENT_";
			
			case "MONSTER" : return -2;
			case "SKILL_SET" :
				$char = $stat >> 3;
				$tree = $stat & 0xF;
				
				$skillstrees = array (
					array(
						" to Bow and Crossbow Skills (Amazon Only)",
						" to Passive and Magic Skills (Amazon Only)",
						" to Javelin and Spear Skills (Amazon Only)",
					),
					array(
						" to Fire Spells (Sorceress Only)",
						" to Lightning Spells (Sorceress Only)",
						" to Cold Spells (Sorceress Only)",
					),
					array(
						" to Curses (Necromancer Only)",
						" to Poison and Bone Spells (Necromancer Only)",
						" to Summoning Spells (Necromancer Only)"
					),
					array(
						" to Combat Skills (Paladin Only)",
						" to Offensive Auras (Paladin Only)",
						" to Defensive Auras (Paladin Only)"
					),
					array(
						" to Combat Skills (Barbarian Only)",
						" to Combat Masteries (Barbarian Only)",
						" to Warcries (Barbarian Only)"
					),
					array(
						" to Summoning (Druid Only)",
						" to Shape Shifting (Druid Only)",
						" to Elemental (Druid Only)"
					),
					array(
						" to Traps (Assassin Only)",
						" to Shadow Disciplines (Assassin Only)",
						" to Martial Arts (Assassin Only)"
					)
				);
				
				return $skillstrees[$char][$tree];
			
			case "FREQUENCY" : return 100 / $stat;
			case "TIME_OF_DAY" : 
				$timenames = array('During Daytime', 'Near Dusk', 'During Nighttime', 'Near Dawn');
				
				if (isset($timenames[$stat]))
					return $timenames[$stat];
					
				return "_UNKNOWN_DAYTIME_";
			
			case "EIGHTHS" : return 1;
				
		}
		return -5;
	}
	
	public function getPropertyStr($id, $stat1 = 0, $stat2 = 0, $stat3 = 0, $stat4 = 0) {
		$prop = $this->getProperty($id);
		
		if (!$prop)
			return false;
			
		if ($prop['displayFunc'] == 'CHARGED_SPELL' || 
			$prop['displayFunc'] == 'ELEMENT_UP' || 
			$prop['displayFunc'] == 'CAST_SPELL'
			) {
				$tmp = $stat1; $stat1 = $stat2; $stat2 = $tmp;	
		}
		
		if ($prop['displayFunc'] == 'SKILL_UP') {
			$skills = new D2PT_configfile('../atma_config/Skills.txt');
			$sk = $skills->select('Id', intval($stat1), true);
			$stat3 = $sk['charclass'];	
		}
		
		$stat1 = $this->convertValue($stat1, $prop['bias'], $prop['convertFieldFunc1']);
		$stat2 = $this->convertValue($stat2, $prop['bias'], $prop['convertFieldFunc2']);
		if (!empty($prop['convertFieldFunc3'])) $stat3 = $this->convertValue($stat3, $prop['bias'], $prop['convertFieldFunc3']);
		$stat4 = $this->convertValue($stat4, $prop['bias'], $prop['convertFieldFunc4']);
		
		if ($prop['displayFunc'] == 'CHARGED_SPELL' || $prop['displayFunc'] == 'ELEMENT_UP' || $prop['displayFunc'] == 'SKILL_UP')
			return sprintf($prop['fmtString1'], $stat2, $stat1, $stat3, $stat4);
		if ($prop['displayFunc'] == 'SKILL_SET_UP')
			return sprintf("+%d%s", $stat2, $stat1);
		if ($prop['displayFunc'] == 'CAST_SPELL')
			return sprintf($prop['fmtString1'], $stat3, $stat2, $stat1, $stat4);
		if ($prop['displayFunc'] == 'POISON_DAMAGE') {
			if ($stat1 == $stat2) {
				return sprintf($prop['fmtString1'], $stat1*25*$stat3, $stat3);
			} else {
				return sprintf($prop['fmtString2'], $stat1*25*$stat3, $stat2*25*$stat3, $stat3);
			}
		}
			
		return sprintf($prop['fmtString1'], $stat1, $stat2, $stat3, $stat4);
	}
	
}