<?php
error_reporting(E_ALL);
ini_set("display_errors", 'On');
require_once 'config.php';

$deviceId = filter_input(INPUT_GET, 'deviceId', FILTER_SANITIZE_STRING);
$registrationId = filter_input(INPUT_GET, 'registrationId', FILTER_SANITIZE_STRING);

if($deviceId !== false){
    if($database->has("pushClients", ["deviceId" => $deviceId])){
        $database->update("pushClients", ["registrationId" => $registrationId], ["deviceId" => $deviceId]);
    }else{
        $database->insert("pushClients", ["deviceId" => $deviceId, "registrationId" => $registrationId]);
    }
}