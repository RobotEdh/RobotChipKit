<?php
$con = mysql_connect("localhost","edh","edh");

if (!$con) {
  die('Could not connect: ' . mysql_error());
}

mysql_select_db("CHIPKIT", $con);


$arr1 = array();
$sth = mysql_query("SELECT unix_timestamp(reg_date)*1000 as reg_date2, value FROM raw_data WHERE source = 1 ORDER BY reg_date");
while($req1 = mysql_fetch_assoc($sth)) {
    $r10 = $req1['reg_date2'];
    $r11 = $req1['value'];
    array_push($arr1,array("$r10", "$r11" ));
}

$arr2 = array();
$sth = mysql_query("SELECT unix_timestamp(reg_date)*1000 as reg_date2, value FROM raw_data WHERE source = 2 ORDER BY reg_date");
while($req2 = mysql_fetch_assoc($sth)) {
    $r20 = $req2['reg_date2'];
    $r21 = $req2['value2'];
    array_push($arr2,array("$r20", "$r21" ));
}

$result = array();
array_push($result,array("title"=>'Brightness',"name1"=>'Source 1',"name2"=>'Source 2'));
array_push($result,array("data"=>$arr1));
array_push($result,array("data"=>$arr2));

print json_encode($result, JSON_NUMERIC_CHECK);


mysql_close($con);
?>

 
