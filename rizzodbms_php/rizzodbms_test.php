<html>
  <head>
    <title>Rizzo DBMS test</title>
    <style>
      #conteiner {
        margin-left: 10%;
        margin-right: 10%;
        align-content: center;
      }
      #progress {
        width:100%;
        background-color:grey;
      }
      #bar {
        width: 0%;
        height: 30px;
        background-color:green;
      }
    </style>
  </head>
  <body>
    <h2 align="center">Rizzo DBMS TEST</h2>

    <div id="conteiner">
      <span align="center">Test: inserimento di 100000 elementi in Rizzo DBMS</span><br><br>
      <div id="progress">
        <div align="left" id="bar"></div>
      </div>
      <br>
      <div id="stats">
        <strong><span>Avanzamento: </span></strong><span id="percentuale">0</span><span>%</span><br>
        <strong><span>Elementi inseriti: </span></strong><span id="elementi">0</span><span> di 100000</span><br>


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
      function echo_flush($txt){
        if(ob_get_level() == 0)
          ob_start();
        echo $txt;
        print str_pad('', 4096)."\n";
        ob_flush();
        flush();
      }

      $start = microtime(true);
      $conn = new MyRizzoDBMS;
      $conn->connect("localhost", 8888, "password", 1, "defaultkey123456", "initializationve") or die("Error: ".$conn->error);
      $width=0;
      $num=100000;
      $unpercento=(int)$num/100;

      for($i=1;$i<=$num;$i++){
        //$key= generateRandomString(10);
    	  $key = ''.$i.'';
        $conn->add($key, 2);

        if($i%$unpercento==0){
          $width++;
          $stringa='<script>
                      document.getElementById("bar").style.width="'.$width.'%";
                      document.getElementById("percentuale").innerHTML="'.$width.'";
                      document.getElementById("elementi").innerHTML="'.$i.'";
                    </script>';

          echo_flush($stringa);

        }
      }

      $time_elapsed_secs = microtime(true) - $start;
      echo '<span><strong>Tempo totale di esecuzione: </strong> '.$time_elapsed_secs .' Sec</span>';
  ?>
      </div>
    </div>
  </body>
</html>
