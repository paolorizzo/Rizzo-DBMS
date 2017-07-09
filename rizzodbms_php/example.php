<?php
  include "library_dbms.php";

  $conn = new MyRizzoDBMS;

  $address = "localhost";
  $port = 8888;
  $password = "password";
  $flag_encryption = 1; //verificare attivazione anche su server
  $encryption_key = "defaultkey123456"; //da 16 caratteri
  $encryption_iv = "initializationve"; //da 16 caratteri
  //con canale criptato con AES 128 CBC
  $conn->connect($address, $port, $password, $flag_encryption, $encryption_key, $encryption_iv) or die($conn->error);

  /*
  $address = "localhost";
  $port = 8888;
  $password = "paolo";
  $flag_encryption = 0; //verificare disattivazione anche su server
  //senza canale criptato
  $conn->query($address, $port, $password, $flag_encryption) or die($conn->error);
  */

  //aggiunta di valori di tipo stringa
  echo "Inserimento elemento con chiave 'chiave1' con valore \"valore\" <br>";
  $conn->add("chiave1", "valore") or die($conn->error);
  //aggiunta di valori numerici
  echo "Inserimento elemento con chiave 'chiave2' con valore 1<br>";
  $conn->add("chiave2", 1) or die($conn->error);
  //aggiunta di valori strutturati come array o oggetti
  echo "Inserimento elemento con chiave 'chiave3' con valore array(1, 43, 34)<br>";
  $conn->add("chiave3", array(1, 43, 34)) or die($conn->error);

  //eliminizazione di un elemento
  echo "Elimininazione elemento con chiave 'chiave2' <br>";
  $conn->delete("chiave2") or die($conn->error);

  //verifica presenza elemento
  echo "Presente elemento con chiave 'chiave2'? :".$conn->exists("chiave2")."<br>";
  echo "Presente elemento con chiave 'chiave1'? :".$conn->exists("chiave1")."<br>";

  //recupero elemento
  $result = $conn->find("chiave3");

  echo "Elemento con chiave3: ";
  var_dump($result);
?>
