#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>

#include "lbbst.h"


#define BFACTOR 2
int heightNode(struct node *root){
  return root ? root->height : 0;
}
int maxi(int a, int b){
  return a > b ? a : b;
}
void fixHeightNode(struct node *root){
  if(root) root->height = maxi(heightNode(root->left), heightNode(root->right)) + 1;
}
int bfactor(struct node* root){
  return root == NULL ? 0: heightNode(root->right)-heightNode(root->left);
}
struct node* rightRotation(struct node *x){
  //printf("inizio Nodo girato sinistra %s\n", x->key);

  struct node *tmp = x->left;
  //if(tmp == NULL) printf("NULLLLLL\n");
  //printf("%d met Nodo girato sinistra\n", *(int *)tmp->value);
  struct node *t2 = tmp->right;

  tmp->right = x;
  x->left = t2;

  fixHeightNode(x);
  fixHeightNode(tmp);
  //printf("%d Nodo girato sinistra\n", *(int *)tmp->value);

  return tmp;
}
struct node* leftRotation(struct node *x){
  //printf("inizio Nodo girato destra %s\n", x->key);

  struct node *tmp = x->right;
  //if(tmp == NULL) printf("NULLLLLL\n");

  struct node *t2 = tmp->left;

  tmp->left = x;
  x->right = t2;

  fixHeightNode(x);
  fixHeightNode(tmp);
  //printf("%d Nodo girato destra\n", *(int *)tmp->value);

  return tmp;
}
struct node* balance(struct node *root){
  fixHeightNode(root);
  int bfact = bfactor(root);
  if(bfact <= -BFACTOR){
    if(bfactor(root->left) >= 1)
      root->left = leftRotation(root->left);
    root = rightRotation(root);
  }
  if(bfact >= BFACTOR){
    if(bfactor(root->right) <= -1)
      root->right = rightRotation(root->right);
    root = leftRotation(root);
  }
  //printf("Nodo bilanciato %d\n", bfact);
  return root;
}
struct node* insertElement(struct node *root, char *key, char *value){
  if(root == NULL){
    root = malloc(sizeof(*root));
    if(root == NULL) printf("out of memory\n");
    root->key = malloc(strlen(key)+1);
    strcpy(root->key, key);
    root->value = malloc(strlen(value)+1);
    strcpy(root->value, value);
    root->height = 1;
    root->left = root->right = NULL;
    return root;
  }

  int result = strcmp(key, root->key);

  if(result < 0){
    root->left = insertElement(root->left, key, value);
    root = balance(root);
  }else if(result > 0){
    root->right = insertElement(root->right, key, value);
    root = balance(root);
  }else{
    free(root->value);
    root->value = malloc(strlen(value)+1);
    strcpy(root->value, value);
  }
  return root;
}
struct node* minNode;
struct node* findMin(struct node *root){
  if(root->left)
    return balance(root->left = findMin(root->left));
  else{
    minNode = root;
    return root->right;
  }
}
struct node* deleteElement(struct node *root, char *key){
  if(root == NULL)
    return root;

  int result = strcmp(key, root->key);
  if(result < 0)
    root->left = deleteElement(root->left, key);
  else if(result > 0)
    root->right = deleteElement(root->right, key);
  else{
    if(root->left == NULL){
      struct node *tmp = root->right;
      free(root);
      root = tmp;
    }else if(root->right == NULL){
      struct node *tmp = root->left;
      free(root);
      root = tmp;
    }else{
      root->right = findMin(root->right);
      root->key = minNode->key;
      root->value = minNode->value;
      free(minNode);
    }
  }
  return balance(root);
}
int existsElement(struct node *root, char *key){
  if(root == NULL)
    return 0;
  int result = strcmp(key, root->key);
  if(result < 0)
    return existsElement(root->left, key);
  else if(result > 0)
    return existsElement(root->right, key);
  else
    return 1;
}
char* findElement(struct node *root, char *key){
  if(root == NULL)
    return NULL;
  int result = strcmp(key, root->key);
  if(result < 0)
    return findElement(root->left, key);
  else if(result > 0)
    return findElement(root->right, key);
  else
    return root->value;
}

void printTree(struct node *root, FILE *fw){
  if(root == NULL)
    return;
  printTree(root->left, fw);
  fprintf(fw, "a %s %s\n", root->key, root->value);
  printTree(root->right, fw);
}
