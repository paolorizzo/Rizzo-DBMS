
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>

#include "serializer.h"

//{"cmd":"add","key":"chiave","value":"O:6:\"Utente\":2:{s:2:\"id\";i:5;s:4:\"name\";s:5:\"Paolo\";}"}
//effettua il parse della stringa ricevuta
//3 campi
//cmd il comando tipo string
//key il nome delle variabile tipo string
//value il valore tipo stringa(serializzato da php prima di arrivare qui)
struct comando parse(char *s){
  struct comando com;
  if(strncmp(s, "{\"cmd\":\"", 8) != 0){
    com.cmd=malloc(6);
    strcpy(com.cmd, "error\0");
    return com;
  }
  int pos=8;
  s+=8;
  char *cmd_inizio=s;
  int cmd_pos_inizio=pos;
  while(s[0]!='\"'){
    //printf("%c", s[0]);
    s++;
    pos++;
    if(s[0]=='\0'){
      com.cmd=malloc(6);
      strcpy(com.cmd, "error\0");
      return com;
    }
  }
  com.cmd=malloc(pos-cmd_pos_inizio+1);
  strncpy(com.cmd, cmd_inizio, pos-cmd_pos_inizio);
  com.cmd[pos-cmd_pos_inizio]='\0';
  //printf("\n");
  if(strncmp(s, "\",\"key\":\"", 9) != 0){
    com.cmd=malloc(6);
    strcpy(com.cmd, "error\0");
    return com;
    if(s[0]=='\0'){
      com.cmd=malloc(6);
      strcpy(com.cmd, "error\0");
      return com;
    }
  }
  s+=9;
  pos+=9;
  char *key_inizio=s;
  int key_pos_inizio=pos;
  while(s[0]!='\"'){
    //printf("%c", s[0]);
    s++;
    pos++;
    if(s[0]=='\0'){
      com.cmd=malloc(6);
      strcpy(com.cmd, "error\0");
      return com;
    }
  }
  com.key=malloc(pos-key_pos_inizio+1);
  strncpy(com.key, key_inizio, pos-key_pos_inizio);
  com.key[pos-key_pos_inizio]='\0';
  //printf("\n");
  if(strncmp(s, "\",\"value\":\"", 11) != 0){
    com.cmd=malloc(6);
    strcpy(com.cmd, "error\0");
    return com;
  }
  s+=11;
  pos+=11;
  char *value_inizio=s;
  int value_pos_inizio=pos;
  int escape=0;
  while(s[0]!='\"' || escape){
    if(s[0]=='\\')
      escape=1;
    else
      escape=0;
    //printf("%c", s[0]);
    s++;
    pos++;
    if(s[0]=='\0'){
      com.cmd=malloc(6);
      strcpy(com.cmd, "error\0");
      return com;
    }
  }
  com.value=malloc(pos-value_pos_inizio+1);
  strncpy(com.value, value_inizio, pos-value_pos_inizio);
  com.value[pos-value_pos_inizio]='\0';
  //printf("\n");
  if(strncmp(s, "\"}", 3) != 0){
    com.cmd=malloc(6);
    strcpy(com.cmd, "error\0");
    return com;
  }

  //printf("ok lettura\n");
  return com;
}
