#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <libgen.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

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
char prompt[512];
size_t len;
const char delim[] = " \n";

int maxHP = 100;
int curHP = 100;
int maxMana = 100;
int curMana = 100;

char* user;
char hostname[1024];
char* cwd;

void gen_prompt();
void read_cmd();
cmd* tokenize();
int execute(cmd* command);
int external_exec(cmd* command);

int main(){
  user = getlogin();
  gethostname(hostname, 1024);
  cwd = getcwd(NULL, 0);

  while(1) {
    read_cmd();
    cmd* cur_cmd = tokenize();
    // for(int i = 0; i < cur_cmd->argc; i++){
    //   printf("%d %s\n",i,cur_cmd->args[i]);
    // }
    // printf("%s",cur_cmd->cmd);
    execute(cur_cmd);
  }
  return 0;
}

void read_cmd(){
  gen_prompt();
  buffer = readline(prompt);
  // getline(&buffer, &len, stdin);
}

void gen_prompt(){
  char* dir_name = basename(cwd);
  sprintf(prompt,"<%sHP✨ %d/%d%s | %sMana🌙 %d/%d%s> %s%s@%s%s [%s] $ ",RED,curHP,maxHP,RESET,BLUE,curMana,maxMana,RESET,GREEN,user,hostname,RESET,dir_name);
}

cmd* tokenize(){
  int i = 1;
  cmd* new_cmd = malloc(sizeof(cmd));
  new_cmd->args = malloc(sizeof(char*) * MAX_ARG_SIZE);

  if (!new_cmd) {
      return NULL;
  }
  if (!new_cmd->args) {
      free(new_cmd);
      return NULL;
  }

  new_cmd->cmd = strtok(buffer, delim);
  new_cmd->args[0] = new_cmd->cmd;
  if (!new_cmd->cmd) {
      free(new_cmd->cmd);
      return NULL;
  }

  while (i < MAX_ARG_SIZE && (new_cmd->args[i] = strtok(NULL, delim)) != NULL) {
      i++;
  }

  new_cmd->argc = i;
  return new_cmd;
}

int execute(cmd* command) {
  if(!command){
    // TODO: Handle Failed.
  }
  if(strcmp(command->cmd, "exit") == 0){
      exit(0);
  }
  else if(strcmp(command->cmd, "potion") == 0){

  }
  else if(strcmp(command->cmd, "cd") == 0){

  }
  else if(strcmp(command->cmd, "history") == 0){
    
  }
  else if(strcmp(command->cmd, "echo") == 0){
    
  }
  else{
    external_exec(command);
  }
  return 0;
}

int external_exec(cmd* command){
  pid_t pid = fork();

  if (pid < 0) {
      perror("Failed forking child");
      return 1;
  } else if (pid == 0) {
      // We are in the child process
      char *argv[] = {command, NULL};  // Create argument list
      if (execvp(command->cmd, command->args) < 0) {
          perror(command);
      }
      exit(0);  // Exit the child process
  } else {
      // We are in the parent process
      wait(NULL);  // Wait for the child to exit
  }
}
