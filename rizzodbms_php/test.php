<?php
  include "library_dbms.php";
  function generateRandomString($length) {
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $charactersLength = strlen($characters);
    $randomString = '';
    for ($i = 0; $i < $length; $i++) {
        $randomString .= $characters[rand(0, $charactersLength - 1)];
    }
    return $randomString;
  }

  $start = microtime(true);
  $conn = new MyRizzoDBMS;
  $conn->connect("localhost", 8888, "password", 1, "defaultkey123456", "initializationve") or die("Error: ".$conn->error);

  for($i=0;$i<100000;$i++){
    //$key= generateRandomString(10);
	$key = ''.$i.'';
    $conn->add($key, 2);
  }
  $time_elapsed_secs = microtime(true) - $start;
  echo '<b>Total Execution Time:</b> '.$time_elapsed_secs .' Sec';
?>
