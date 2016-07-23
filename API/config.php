<?php
require_once 'medoo.min.php';
$database = new medoo([
  'database_type' => 'mysql',
  'database_name' => 'NHLRooster',
  'server' => 'localhost',
  'username' => 'NHLRooster',
  'password' => '',
  'charset' => 'utf8'
]);