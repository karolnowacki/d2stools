<?php

class D2PT_configfile {
	
	protected $header = array();
	protected $rows = array();
	protected $rowcount = 0;
	protected $table = NULL;
	
	public function __construct($file) {
		
		$fh = fopen($file, "r");
		$this->header = fgetcsv($fh, 8196, "\t");
		
		$rc = 0;
		while (($row = fgetcsv($fh, 8196, "\t")) !== FALSE) {
        	$this->rows[] = $row;
        	$rc++;
    	}
    	$this->rowcount = $rc;
    	
		fclose($fh);
		
		$this->fixvalues();
	}
	
	private function fixvalues() {
		$hc = count($this->header);
		for($i=0; $i<$this->rowcount; ++$i)
			for ($j=0; $j<$hc; ++$j) {
				if (empty($this->rows[$i][$j]))
					$this->rows[$i][$j] = 0;
				elseif (is_numeric($this->rows[$i][$j]))
					$this->rows[$i][$j] = intval($this->rows[$i][$j]);			
			}	
	}
	
	public function select($key, $value, $justone = false) {
		$key = array_search($key, $this->header);
		if (!$key)
			return false;
		
		$ret = array();
		
		for ($i=0; $i<$this->rowcount; $i++) {
			if ($this->rows[$i][$key] === $value) {
				
				$r = array_combine($this->header, $this->rows[$i]);
				
				if ($justone)
					return $r;
					
				$ret[] = $r;
			}
		}
		
		return $ret;
	}
	
	public function getFieldNames() {
		return $this->header;	
	}
	
	public function getTable() {
		if (is_null($this->table)) {
			$this->table = array();
			for ($i=0; $i<$this->rowcount; $i++) {
				$this->table[] = array_combine($this->header, $this->rows[$i]);
			}
		}
		return $this->table;
	}
	
}