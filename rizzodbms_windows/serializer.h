#ifndef HEADER_SERIALIZER_H
  #define HEADER_SERIALIZER_H
  struct comando{
    char *cmd, *key, *value;
  };
  extern struct comando parse(char *s);
#endif
