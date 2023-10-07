#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <libgen.h>

#define MAX_CMD_SIZE 256
#define MAX_ARG_SIZE 25

#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define RESET "\033[0m"



typedef struct cmd {
  char* cmd;
  char** args;
  int argc;
} cmd;

char* buffer;
size_t len;
const char delim[] = " \n";

int maxHP = 100;
int curHP = 100;
int maxMana = 100;
int curMana = 100;

char* user;
char hostname[1024];
char* cwd;

void print_prompt();
void read_cmd();
cmd* tokenize();
int execute(cmd* command);
enum cmd_names find_token(char* token);

int main(){
  user = getlogin();
  gethostname(hostname, 1024);
  cwd = getcwd(NULL, 0);
  print_prompt();

  while(1) {
    read_cmd();
    cmd* cur_cmd = tokenize();
    // for(int i = 0; i < cur_cmd->argc; i++){
    //   printf("%d %s\n",i,cur_cmd->args[i]);
    // }
    printf("%s",cur_cmd->cmd);
    execute(cur_cmd);
    print_prompt();
  }
  return 0;
}

void read_cmd(){
  getline(&buffer, &len, stdin);
}

void print_prompt(){
  printf("< %sHP %d/%d%s | %sMana %d/%d%s >",RED,curHP,maxHP,RESET,BLUE,curMana,maxMana,RESET);
  printf(" %s%s@%s%s",GREEN,user,hostname, RESET);
  char* dir_name = basename(cwd);
  printf(" [%s] $", dir_name);
}

cmd* tokenize(){
  cmd* new_cmd = malloc(sizeof(cmd));
    if (!new_cmd) {
        return NULL;
    }

    new_cmd->cmd = strtok(buffer, delim);
    if (!new_cmd->cmd) {
        free(new_cmd->cmd);
        return NULL;
    }

    new_cmd->args = malloc(sizeof(char*) * MAX_ARG_SIZE);
    if (!new_cmd->args) {
        free(new_cmd);
        return NULL;
    }

    int i = 0;
    while (i < MAX_ARG_SIZE && (new_cmd->args[i] = strtok(NULL, delim)) != NULL) {
        i++;
    }

    new_cmd->argc = i;
    return new_cmd;
}

int execute(cmd* command) {
  if(!command){
    // Handle Failed.
  }
  if(strcmp(command->cmd, "exit") == 0){
      exit(0);
  }
  else if(strcmp(command->cmd, "cd") == 0){

  }
  else if(strcmp(command->cmd, "history") == 0){
    
  }
  else if(strcmp(command->cmd, "echo") == 0){
    
  }
  return 0;
}
