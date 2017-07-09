#include "queue.h"
//#include<stdlib.h>
#include<stdio.h>
struct print_queue *head=NULL, *tail=NULL;
int size=-1;
void add(struct comando cmd){
  //printf("%s %s\n", cmd.key, cmd.value);
  if(!tail){
    tail = (struct print_queue*)malloc(sizeof(struct print_queue));
    tail->cmd = (struct comando*)malloc(sizeof(cmd));
    tail->cmd->cmd = cmd.cmd;
    tail->cmd->key = cmd.key;
    tail->cmd->value = cmd.value;
    tail->next = NULL;
    head = tail;
  }else{
    struct print_queue *tmp = (struct print_queue*)malloc(sizeof(struct print_queue));
    tmp->cmd = (struct comando*)malloc(sizeof(cmd));
    tmp->cmd->cmd = cmd.cmd;
    tmp->cmd->key = cmd.key;
    tmp->cmd->value = cmd.value;
    tail->next = tmp;
    tail = tmp;
    tail->next=NULL;
  }
  size++;
}
struct comando* top(){
  if(head){
    struct comando *tmp = head->cmd;
    head = head->next;
    size--;
    if(!head)
      tail = NULL;
    return tmp;
  }
  else{
    return NULL;
  }
}
