<?php
//if($_SERVER['REMOTE_ADDR'] == '77.248.220.122')
//	die;
//error_reporting(E_ALL);
//ini_set("display_errors", 'On');
header('Content-Type: text/xml');
require_once 'config.php';

$classId = filter_input(INPUT_GET, 'classId', FILTER_VALIDATE_INT);
$deviceId = filter_input(INPUT_GET, 'deviceId', FILTER_SANITIZE_STRING);

if($deviceId == false)
die;

$deviceSelect = $database->select("pushClients", ["deviceId", "registrationId", "className"], ["deviceId" => $deviceId]); 
if(sizeof($deviceSelect) < 1){
    $database->insert("pushClients", ["deviceId" => $deviceId, "registrationId" => ""]);
    die;
}else{
  if($classId != false){
    $classSelect = $database->select("Class", "className", ["Id" => $classId]);
    // update class
    $database->update("pushClients", ["className" => $classSelect[0]], ["deviceId" => $deviceId]);
    $useClassId = $classId;
  }else{
    if(isset($deviceSelect[0]["className"])){
      $tempClassName = $deviceSelect[0]["className"];
      $selectId = $database->select("Class", "Id", ["className" => $tempClassName]);
      $useClassId = $selectId[0];
    }else{
      //echo "<xml>fail</xml>";
      die;
    }
  }
}
if($deviceId != false && $useClassId != -1)
{
//SELECT * FROM lesson WHERE Id IN (SELECT `value` FROM Class_lessons WHERE object_id = '?')
      $lessons = $database->select("ChangeItem", [
	"title", "teacher", "location", "startdate", "enddate", "year", "description", "change"
	], ["AND" => ["Id" => $database->select("Class_changeList", "value", ["object_id" => $useClassId])],  "ORDER" => ["startdate ASC"]]);
	//echo $database->last_query();
    
    echo '<xml>';
    $microtime = microtime();
    $comps = explode(' ', $microtime);

    echo '<date>' . sprintf('%d%03d', $comps[1], $comps[0] * 1000) . '</date>';
    foreach($lessons as $lesson){
    $startdate = htmlspecialchars($lesson["startdate"]);
    $enddate = htmlspecialchars($lesson["enddate"]);
    $startdate = new DateTime($startdate);
      echo '<lesson>';
      echo '<title>'.htmlspecialchars($lesson["title"]) . '</title>';
      echo '<teacher>'.htmlspecialchars($lesson["teacher"]).'</teacher>';
      echo '<location>'.htmlspecialchars($lesson["location"]).'</location>';
      echo '<starttime>'.$startdate->format("Y-m-d H:i:s").'</starttime>';
	  if($enddate){
		$enddate = new DateTime($enddate);
		echo '<endtime>'.$enddate->format("Y-m-d H:i:s").'</endtime>';
	  }
	  else
		  echo '<endtime></endtime>';
	  echo '<year>'.htmlspecialchars($lesson["year"]).'</year>';
	  echo '<comment>'.htmlspecialchars($lesson["description"]).'</comment>';
      echo '<change>'.$lesson['change'].'</change>';
      echo '</lesson>';
    }
    echo '</xml>';
}