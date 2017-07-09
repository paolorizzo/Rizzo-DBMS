#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
//#include<sys/socket.h>
#pragma comment(lib, "ws2_32.lib")
#include<winsock2.h>
#include<fcntl.h>
#include<io.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<time.h>
#include<signal.h>

#include "lbbst.h"
#include "serializer.h"
#include "aes.h"
#include "queue.h"

int PORT = 2323;
int ENCRYPT = 1; //1 se attivare la criptazione del canale, altrimenti 0
int PERSISTENCE = 1; //1 attiva la persistenza dei dati, altrimenti 0
#define MAX_LENGTH_MESSAGE 2048 //lunghezza messaggio di comunicazione
char PATH_FILE_DB[1000] = "./data/database.db"; //percorso del file che memorizza il db
char PATH_FILE_CONFIG[] = "./config/config.txt";

uint8_t key[16] = "defaultkey123456";
uint8_t iv[16] =  "initializationve";
char password[1000]="password";

struct node *root = NULL; //radice della struttura dati (albero AVL)
pthread_mutex_t lock_encryption, lock_bbst, lock_queue, lock_print; //mutua esclusione

void *connection_handler(void *); //the thread function
void loadConfig();
void loadDB();
void atExit();

int main(int argc , char *argv[]){

	loadConfig();

	if (ENCRYPT)
		puts("Encryption enable");
	else
		puts("Encryption disable");

	if (PERSISTENCE)
		puts("Persistence enable");
	else
		puts("Persistence disable");

	if (PERSISTENCE) {
		puts("Loading database...");
		loadDB();
	}
	signal(SIGINT, atExit);

    srand(time(0));
    SOCKET socket_desc , client_sock , *new_sock;
    int c;
    struct sockaddr_in server , client;

    unsigned short wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )) {
       fprintf(stderr, "Could not find useable sock dll %d\n",WSAGetLastError());
       return 0;
    }
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("Bind done");


    //Listen
    listen(socket_desc , 3);

    pthread_mutex_init (&lock_encryption, NULL);
    pthread_mutex_init (&lock_bbst, NULL);
    pthread_mutex_init (&lock_queue, NULL);
    pthread_mutex_init (&lock_print, NULL);


    //Accept and incoming connection
    printf("Waiting for incoming connections on port %d...\n", PORT);
    c = sizeof(SOCKADDR);
    while( (client_sock = accept(socket_desc, (SOCKADDR*)&client, &c)) ){
        //puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0){
            perror("could not create thread");
            return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //puts("Handler assigned");
    }

    if (client_sock < 0){
        perror("accept failed");
        return 1;
    }

    return 0;
}


void decrypt_string(char *in, char *out){
  int string_length = 0;
  int sw=0;
  while(!sw){
    sw=1;
    for(int i=0;i<16;i++)
      if(in[string_length+i]!='\0') sw=0;
    if(!sw)
      string_length += 16;
  }
  AES128_CBC_decrypt_buffer((uint8_t*)out, (uint8_t*)in, string_length, key, iv);
}

void encrypt_string(char *in, char *out){
  int string_length = strlen(in);
  AES128_CBC_encrypt_buffer((uint8_t*)out, (uint8_t*)in, string_length, key, iv);
}

void reply(SOCKET sock, char *answer, char *encrypted){
  char *answer_pt;
  if(ENCRYPT){
    pthread_mutex_lock(&lock_encryption);
    encrypt_string(answer, encrypted);
    pthread_mutex_unlock(&lock_encryption);
    answer_pt = encrypted;
  }else{
    answer_pt = answer;
  }
  send(sock , answer_pt, strlen(answer_pt), 0);
}

void *connection_handler(void *socket_desc){
    //Get the socket descriptor
    SOCKET sock = *(SOCKET*)socket_desc;
    int read_size;
    char *command_pt;
    char command[MAX_LENGTH_MESSAGE], decrypted[MAX_LENGTH_MESSAGE], encrypted[MAX_LENGTH_MESSAGE];


    memset(command, 0, MAX_LENGTH_MESSAGE);
    memset(decrypted, 0, MAX_LENGTH_MESSAGE);
    memset(encrypted, 0, MAX_LENGTH_MESSAGE);

    recv(sock , command , MAX_LENGTH_MESSAGE , 0);

    if(ENCRYPT){
      pthread_mutex_lock(&lock_encryption);
      decrypt_string(command, decrypted);
      pthread_mutex_unlock(&lock_encryption);
      command_pt = decrypted;
    }else
      command_pt = command;

    //effettua il controllo della password
    if(strcmp(password, command_pt)!=0){
      printf("Errore! Wrong password!\n");
      char answer[]="{\"status\":\"ERROR\", \"error\":\"Wrong password!\"}";
      reply(sock, answer, encrypted);
      free(socket_desc);
      return 0;
    }else{
      char answer[]="{\"status\":\"OK\", \"error\":\"\"}";
      reply(sock, answer, encrypted);
    }

    memset(command, 0, MAX_LENGTH_MESSAGE);
    memset(decrypted, 0, MAX_LENGTH_MESSAGE);
    memset(encrypted, 0, MAX_LENGTH_MESSAGE);
    //printf("Client password ok\n");
    //Receive a message from client
    while( (read_size = recv(sock , command , MAX_LENGTH_MESSAGE , 0)) > 0 ){

        if(ENCRYPT){
          pthread_mutex_lock(&lock_encryption);
          decrypt_string(command, decrypted);
          pthread_mutex_unlock(&lock_encryption);
          command_pt = decrypted;
        }else
          command_pt = command;

        struct comando com=parse(command_pt);

        if(strcmp(com.cmd, "find")==0){

          pthread_mutex_lock(&lock_bbst);
          char *data = findElement(root, com.key);
          pthread_mutex_unlock(&lock_bbst);

          if(data){
			char answer[MAX_LENGTH_MESSAGE]="{\"status\":\"OK\", \"error\":\"\", \"data\": \"";
            strcat(answer, data);
            strcat(answer, "\"}\0");
            reply(sock, answer, encrypted);
          }else{
            char answer[]="{\"status\":\"ERROR\", \"error\":\"This key is not stored!\"}";
            reply(sock, answer, encrypted);
          }

        }else if(strcmp(com.cmd, "add")==0){

          pthread_mutex_lock(&lock_bbst);
          root = insertElement(root, com.key, com.value);
          if(PERSISTENCE){
            pthread_mutex_lock(&lock_queue);
            add(com);
            pthread_mutex_unlock(&lock_queue);
          }
          pthread_mutex_unlock(&lock_bbst);



          char answer[]="{\"status\":\"OK\", \"error\":\"\"}";
          reply(sock, answer, encrypted);

        }else if(strcmp(com.cmd, "delete")==0){

          pthread_mutex_lock(&lock_bbst);
          root = deleteElement(root, com.key);
          if(PERSISTENCE){
            pthread_mutex_lock(&lock_queue);
            add(com);
            pthread_mutex_unlock(&lock_queue);
          }
          pthread_mutex_unlock(&lock_bbst);



          char answer[]="{\"status\":\"OK\", \"error\":\"\"}";
          reply(sock, answer, encrypted);

        }else if(strcmp(com.cmd, "exists")==0){

          pthread_mutex_lock(&lock_bbst);
          int answer = existsElement(root, com.key);
          pthread_mutex_unlock(&lock_bbst);

          if(answer == 1){
            char answer[]="{\"status\":\"OK\", \"error\":\"\", \"data\": 1}";
            reply(sock, answer, encrypted);
          }else{
            char answer[]="{\"status\":\"OK\", \"error\":\"\", \"data\": 0}";
            reply(sock, answer, encrypted);
          }

        }else{

          printf("Errore! Wrong syntax!\n");

          char answer[]="{\"status\":\"ERROR\", \"error\":\"Wrong syntax!\"}";
          reply(sock, answer, encrypted);

        }

        //pulisco il canale
        memset(command, 0, MAX_LENGTH_MESSAGE);
        memset(decrypted, 0, MAX_LENGTH_MESSAGE);
        memset(encrypted, 0, MAX_LENGTH_MESSAGE);
    }

    if(read_size == 0){
        //puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1){
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    if(PERSISTENCE){
      pthread_mutex_lock(&lock_print);
      printf("Inizio salvataggio in background...\n");
      FILE *fd=fopen(PATH_FILE_DB, "a");
      pthread_mutex_lock(&lock_queue);
      struct comando *cmd_p=top();
      pthread_mutex_unlock(&lock_queue);

      while(cmd_p){

        if(strcmp(cmd_p->cmd, "add")==0)
          fprintf(fd, "a %s %s\n", cmd_p->key, cmd_p->value);
        else
          fprintf(fd, "d %s %s\n", cmd_p->key, cmd_p->value);

        //printf("%s %s\n", cmd_p->key, cmd_p->value);
        pthread_mutex_lock(&lock_queue);
        free(cmd_p);
        cmd_p=top();
        pthread_mutex_unlock(&lock_queue);
      }

      fclose(fd);
      printf("Fine salvataggio in background\n");
      pthread_mutex_unlock(&lock_print);
    }
    return 0;
}


void loadConfig(){
	FILE *fr = fopen(PATH_FILE_CONFIG, "r");
	char name_config[50];
	while(fr && fscanf(fr, "%s ", &name_config) == 1){
		if(strcmp(name_config, "PORT") == 0){
			int port;
			if(fscanf(fr, "%d\n", &port)){
				PORT = port;
			}
		}else if(strcmp(name_config, "ENCRYPT") == 0){
			int encrypt;
			if(fscanf(fr, "%d\n", &encrypt)){
				ENCRYPT = encrypt;
			}
		}else if (strcmp(name_config, "PERSISTENCE") == 0){
			int persistence;
			if(fscanf(fr, "%d\n", &persistence)){
				PERSISTENCE = persistence;
			}
		}else if(strcmp(name_config, "PATH_FILE_DB") == 0){
			char path[1000];
			if(fscanf(fr, "%s\n", &path)){
				strcpy(PATH_FILE_DB, path);
			}
		}else if(strcmp(name_config, "ENCRYPTION_KEY") == 0){
			char key_temp[1000];
			if(fscanf(fr, "%s\n", &key_temp)){
				strncpy(key, key_temp, 16);
			}
    }else if(strcmp(name_config, "INITIALIZATION_VECTOR") == 0){
			char iv_temp[1000];
			if(fscanf(fr, "%s\n", &iv_temp)){
				strncpy(iv, iv_temp, 16);
			}
    }else if(strcmp(name_config, "PASSWORD") == 0){
			char password_temp[1000];
			if(fscanf(fr, "%s\n", &password_temp)){
				strcpy(password, password_temp);
			}
    }
	}
	if (fr)
		fclose(fr);
}

void loadDB(){
  clock_t s =clock();
  char c;
  char key_node[MAX_LENGTH_MESSAGE], value_node[MAX_LENGTH_MESSAGE];
  FILE *fr = fopen(PATH_FILE_DB, "r");
  while(fr && fscanf(fr, "%c ", &c) == 1){
    if(c=='a'){
      fscanf(fr, "%s %s\n", key_node, value_node);
      root = insertElement(root, key_node, value_node);
    }else{
      fscanf(fr, "%s\n", key_node);
      root = deleteElement(root, key_node);
    }
  }
  if(fr)
    fclose(fr);
  //ora riscrivo il file db pulendo le istruzioni di troppo
  //e lo stampo inorder per rendere più veloce la ricostruzione successivamente

  FILE *fw = fopen(PATH_FILE_DB, "w");
  printTree(root, fw);
  fclose(fw);
  clock_t e =clock();
  printf("Load finish. Time: %f\n", (float)(e-s)/CLOCKS_PER_SEC);


}

void atExit() {
    //blocco tutto e chiudo
    pthread_mutex_lock(&lock_encryption);
    pthread_mutex_lock(&lock_bbst);
    pthread_mutex_lock(&lock_queue);
    pthread_mutex_lock(&lock_print);
    printf("Exiting...\n");
    if(PERSISTENCE){
      printf("Inizio salvataggio in background...\n");
      FILE *fd=fopen(PATH_FILE_DB, "a");

      struct comando *cmd_p=top();

      while(cmd_p!=NULL){
        if(strcmp(cmd_p->cmd, "add")==0)
          fprintf(fd, "a %s %s\n", cmd_p->key, cmd_p->value);
        else
          fprintf(fd, "d %s %s\n", cmd_p->key, cmd_p->value);
        //printf("%s %s\n", cmd_p->key, cmd_p->value);
        free(cmd_p);
        cmd_p=top();
      }
      fclose(fd);
      printf("Fine salvataggio in background\n");
    }
    exit(0);
}
