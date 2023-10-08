#ifndef BUILTIN_H
#define BUILTIN_H

int cd(char* path);
int echo(char* args);
void history();
void help();
void potion(int* curMana);
int get_dragon_health(void);
void print_alive_dragon(void);
void print_dead_dragon(void);
int pwd(void);

#endif // BUILTIN_H
