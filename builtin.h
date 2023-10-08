#ifndef BUILTIN_H
#define BUILTIN_H

#include <uthash.h>

int cd(char* path);
int echo(char* args);
void history();
void help();
void alias(char* arg);
void potion(int* curMana);
int get_dragon_health(void);
void print_dragon(void);

void add_alias(char *alias, char *command);
char *find_alias(char *alias);
void delete_alias(char *alias);

typedef struct {
    char *alias;      // key
    char *command;    // value
    UT_hash_handle hh;
} AliasMap;

#endif // BUILTIN_H
