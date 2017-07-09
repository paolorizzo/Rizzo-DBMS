<?php
  class Comando{
    public $cmd;
    public $key;
    public $value;

    public function __construct($cmd, $key, $value){
      $this->cmd = $cmd;
      $this->key = $key;
      $this->value = $value;
    }
  }
  define('AES_128_CBC', 'aes-128-cbc');
  class MyRizzoDBMS{

    protected $sock;
    protected $authentication=false;
    protected $encryption=false;
    protected $encryption_key;
    protected $encryption_iv;
    public $error="";

    protected function write($mex){

      if($this->encryption){
        while(strlen($mex)%16!=0) $mex .= "\0";
        $mex = openssl_encrypt($mex, AES_128_CBC, $this->encryption_key, OPENSSL_NO_PADDING, $this->encryption_iv);
        //echo bin2hex($mex) . "<br>";
      }

      if(!socket_send($this->sock, $mex, strlen($mex), 0)){
          $errorcode = socket_last_error();
          $errormsg = socket_strerror($errorcode);

          $this->error .= "Could not send data: [$errorcode] $errormsg <br>";
      }
    }

    protected function read(){
      if(!socket_recv($this->sock, $buf, 2045, 0)){
          $errorcode = socket_last_error();
          $errormsg = socket_strerror($errorcode);

          $this->error .= "Could not receive data: [$errorcode] $errormsg <br>";
      }

      if($this->encryption){
        $buf = openssl_decrypt($buf, AES_128_CBC, $this->encryption_key, OPENSSL_NO_PADDING, $this->encryption_iv);
        $buf = trim($buf, "\0"); //pulisco la stringa dai caratteri nulli alla fine
      }

      $answer = (array)json_decode($buf);
      if($answer == null)
        $this->error .= "Decrypted/Message JSON ".json_last_error_msg(). "<br>";
      return $answer;
    }

    public function connect($address, $port, $password, $encryption, $encryption_key = null, $encryption_iv = null){
      $this->encryption = $encryption;

      if($this->encryption){
        $this->encryption_key = $encryption_key;
        $this->encryption_iv = $encryption_iv;
      }
      //echo "Socket creating... <br>";

      if(!($this->sock = socket_create(AF_INET, SOCK_STREAM, 0))){
          $errorcode = socket_last_error();
          $errormsg = socket_strerror($errorcode);

          $this->error .= "Couldn't create socket: [$errorcode] $errormsg <br>";
      }

      //echo "Socket created... <br>";

      if(!socket_connect($this->sock, $address, $port)){
          $errorcode = socket_last_error();
          $errormsg = socket_strerror($errorcode);

          $this->error .= "Could not connect: [$errorcode] $errormsg <br>";
      }

      $this->write($password);
      $answer = $this->read();

      if($answer["status"]=="OK"){
        $this->authentication = true;
        return true;
      }else{
        $this->error .= $answer["error"]."<br>";
        return null;
      }

    }

    public function add($key, $value){
      if(!$this->authentication){
        $this->error .= "No connection<br>";
        return null;
      }

      $comando = new Comando("add", base64_encode($key), base64_encode(serialize($value)));//non deserializza altrimenti
      $mex=json_encode($comando);

      $this->write($mex);
      $answer = $this->read();

      if($answer["status"]=="OK")
        return true;
      else{
        $this->error .= $answer["error"]."<br>";
        return null;
      }

    }

    public function delete($key){
      if(!$this->authentication){
        $this->error .= "No connection<br>";
        return null;
      }
      $comando = new Comando("delete", base64_encode($key), "N;");
      $mex=json_encode($comando);

      $this->write($mex);
      $answer = $this->read();

      if($answer["status"]=="OK")
        return true;
      else{
        $this->error .= $answer["error"]."<br>";
        return null;
      }

    }

    public function exists($key){
      if(!$this->authentication){
        $this->error .= "No connection<br>";
        return null;
      }
      $comando = new Comando("exists", base64_encode($key), "N;");
      $mex=json_encode($comando);

      $this->write($mex);
      $answer = $this->read();

      if($answer["status"]=="OK"){
        return $answer["data"];
      }else{
        $this->error .= $answer["error"]."<br>";
        return null;
      }

    }

    public function find($key){
      if(!$this->authentication){
        $this->error .= "No connection<br>";
        return null;
      }
      $comando = new Comando("find", base64_encode($key), "N;");
      $mex=json_encode($comando);

      $this->write($mex);
      $answer = $this->read();

      if($answer["status"]=="OK"){
        return unserialize(base64_decode($answer["data"]));
      }else{
        $this->error .= $answer["error"]."<br>";
        return null;
      }

    }

    public function close(){
      $this->authentication = false;
      socket_close($this->sock);
    }

  }
?>
