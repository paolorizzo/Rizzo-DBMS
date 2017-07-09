#ifndef HEADER_QUEUE_H
  #define HEADER_QUEUE_H

  #include "serializer.h"

  struct print_queue{
    struct comando *cmd;
    struct print_queue *next;
  };

  extern void add(struct comando cmd);

  extern struct comando* top();

  void pop();

  extern int q_size();

#endif
