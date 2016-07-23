<?php
/* 
Only compatible with version 20 and older
*/
//error_reporting(E_ALL);
//ini_set("display_errors", 'On');
header('Content-Type: text/xml');
require_once 'config.php'

$weekNr = filter_input(INPUT_GET, 'weekNr', FILTER_VALIDATE_INT);
$classId = filter_input(INPUT_GET, 'classId', FILTER_VALIDATE_INT);
$deviceId = filter_input(INPUT_GET, 'deviceId', FILTER_SANITIZE_STRING);

if($deviceId == false)
die;

$deviceSelect = $database->select("pushClients", ["deviceId", "registrationId", "className"], ["deviceId" => $deviceId]); 
if(sizeof($deviceSelect) < 1){
$database->insert("pushClients", ["deviceId" => $deviceId, "registrationId" => ""]);
//print_r($database->error());
die;
//die("THIS CANNOT HAPPEN");
/*
  if($classId == false || $classId == -1)
    die("ERROR, CLASSID IS NOT KNOWN, SHOULD NEVER HAPPEN");
    $classSelect = $database->select("Classes", "className", ["classId" => $classId]);
  $database->update("pushClients", ["className" => $classSelect[0]], ["deviceId" => $deviceId);
  $useClassId = $classId;
  */
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
if($weekNr != false && $deviceId != false && $useClassId != -1)
{
//SELECT * FROM lesson WHERE Id IN (SELECT `value` FROM Class_lessons WHERE object_id = '?')
      $lessons = $database->select("Lesson", [
	"title", "teacher", "location", "startdate", "enddate", "year", "description"
	], ["AND" => ["Id" => $database->select("Class_lessons", "value", ["object_id" => $useClassId]), "weekNr" => $weekNr],  "ORDER" => ["startdate ASC"]]);
	//echo $database->last_query();
    
    echo '<xml>';
    $microtime = microtime();
    $comps = explode(' ', $microtime);

    echo '<date>' . sprintf('%d%03d', $comps[1], $comps[0] * 1000) . '</date>';
    foreach($lessons as $lesson){
    $startdate = htmlspecialchars($lesson["startdate"]);
    $enddate = htmlspecialchars($lesson["enddate"]);
    $startdate = new DateTime($startdate);
    $enddate = new DateTime($enddate);
      echo '<lesson>';
      echo '<title>'.htmlspecialchars($lesson["title"]) . '</title>';
      echo '<teacher>'.htmlspecialchars($lesson["teacher"]).'</teacher>';
      echo '<location>'.htmlspecialchars($lesson["location"]).'</location>';
      echo '<starttime>'.$startdate->format("Y-m-d H:i:s").'</starttime>';
      echo '<endtime>'.$enddate->format("Y-m-d H:i:s").'</endtime>';
	  echo '<year>'.htmlspecialchars($lesson["year"]).'</year>';
	  echo '<comment>'.htmlspecialchars($lesson["description"]).'</comment>';
      echo '</lesson>';
    }
    echo '</xml>';
}