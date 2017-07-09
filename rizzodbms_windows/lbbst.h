#ifndef HEADER_LBBST_H
  #define HEADER_LBBST_H
struct node{
  char *key;
  char *value;
  int height;
  struct node *left, *right;
};

extern struct node* insertElement(struct node *root, char *key, char *value);

extern struct node* deleteElement(struct node *root, char *key);

extern int existsElement(struct node *root, char *key);

extern char* findElement(struct node *root, char *key);

extern void printTree(struct node *root, FILE *fw);
#endif
