# Rizzo-DBMS

Rizzo-DBMS è un key-value store scritto in C, che mantiene in RAM i dati, con la possibilità di permanenza su disco.
L'applicazione apre una determinata porta e attende il collegamento da parte dei client.
I dati possono essere criptati con l'algortimo a chiave simmetrica AES128-CBC. 
Le operazioni supportate sono di 4 tipi:
1. Find
2. Add
3. Delete
4. Exists


Gli elementi inseriti avranno una chiave e un valore.
E' presente una libreria PHP per facilitare l'utilizzo, e permette anche di salvare qualsiasi tipo di valore dopo che esegue una serializzazione su esso.


# COME USARLO

E' disponibile una classe PHP per poter utilizzare l'applicazione.


    $conn = new MyRizzoDBMS;


E' possibile utilizzare o no la cifratura dei messaggi al momento della connessione con l'applicazione.

<b>Connessione con cifratura</b>

    $address = "localhost";
    $port = 8888;
    $password = "password";
    $flag_encryption = 1; //verificare attivazione anche su server
    $encryption_key = "defaultkey123456"; //da 16 caratteri
    $encryption_iv = "initializationve"; //da 16 caratteri
    $conn->connect($address, $port, $password, $flag_encryption, $encryption_key, $encryption_iv) or die($conn->error);

<b>Connessione senza cifratura</b>

    $address = "localhost";
    $port = 8888;
    $password = "paolo";
    $flag_encryption = 0; //verificare disattivazione anche su server
    $conn->query($address, $port, $password, $flag_encryption) or die($conn->error);


<b>Aggiunta elemento</b>

    //aggiunta di valori di tipo stringa
    $conn->add("chiave1", "valore") or die($conn->error);
    
    //aggiunta di valori numerici
    $conn->add("chiave2", 1) or die($conn->error);
    
    //aggiunta di valori strutturati come array o oggetti
    $conn->add("chiave3", array(1, 43, 34)) or die($conn->error);
    
<b>Eliminazione elemento</b>

    $conn->delete("chiave2") or die($conn->error);

<b>Esiste elemento</b>

    echo "Presente elemento con chiave 'chiave2'? :".$conn->exists("chiave2")."<br>";
    
<b>Recupera elemento</b>

    $result = $conn->find("chiave3");
    echo "Elemento con chiave3: ";
    var_dump($result);
    
