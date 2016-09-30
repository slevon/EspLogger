<?php

sleep(2);
$data=array();
foreach($_REQUEST as $key=>$val){
	$data[$key]=$val;
	}

echo json_encode($data);
?>
