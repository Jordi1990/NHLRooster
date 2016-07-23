<?php
function contains($str, array $arr)
{
    foreach($arr as $a) {
        if (stripos($str,$a) === false) return false;
    }
    return true;
}
error_reporting(E_ALL);
ini_set("display_errors", 'On');
header('Content-Type: text/xml');
require_once 'config.php';

$departments = array("PABODT" => "Pabo(deeltijd)", "ECC" => "Communicatie", "ECD" => "Docent Theater BA", "ECE" => "Exacte Vakken", "ECF" => "Sociale Vakken(deeltijd)", "ECS" => "Sociale Vakken(voltijd", "ECK" => "Beeldende Kunst en Vormgeving", "ECP" => "Pabo", "ECT" => "Talen", "EM" => "Economie & Management", "MAS" => "Masters", "MPE" => "Master Pedagogiek", "TEB" => "Built Environment", "TEE" => "Engineering", "TEM" => "Maritieme Techniek", "ZWV" => "Verpleegkunde", "ZWW" => "Welzijndsopleidingen", "MIWB" => "Maritiem Instituut Willem Barentsz");

$searchQuery = filter_input(INPUT_GET, 'q', FILTER_SANITIZE_STRING);

$searchQuery = trim(preg_replace('/[[:space:]]+/', ' ', $searchQuery)); // strip extra spaces
$search_array = explode(' ', $searchQuery);
if(sizeof($search_array) > 5)
  die;

$searchHits = $database->select("Class", ["className", "Id", "departmentString", "cpath"], ["className[~]" => $search_array[0], "LIMIT" => 20]);
$limit = 0;
echo '<xml>';
foreach($searchHits as $hit){
$student = 0;
if($hit['cpath'] == 'stud')
    $student = 1;
else if($hit['cpath'] == 'exam')
    $student = 2;
// filter all following results
  if(contains($hit["className"], $search_array) && $limit < 10){
    echo '<result>';
    echo '<id>'.$hit['Id'].'</id><name>'.htmlspecialchars($hit['className']).'</name><department>'.htmlspecialchars($departments[''.$hit['departmentString'] . '']).'</department><type>'.$student.'</type>';
    echo '</result>';
    $limit++;
  }
}
echo '</xml>';
//SELECT * FROM Classes WHERE ClassName LIKE '%Informatica%'