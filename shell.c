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
#define MAX_ARG_LEN  100

#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define YELLOW "\033[0;33m"
#define PURPLE "\033[0;35m"
#define RESET "\033[0m"



typedef struct cmd {
  char* cmd;
  char* args[MAX_ARG_SIZE];
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

char* colorize(char* str, const char* color_code);
void print_error(char* cmd);

void print_cmd(const cmd *command) {
    printf("Command: %s\n", command->cmd);
    printf("Number of arguments: %d\n", command->argc);
    for (int i = 0; i < command->argc; i++) {
        printf("Argument %d: %s\n", i + 1, command->args[i]);
    }
}

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
  cmd* new_cmd = malloc(sizeof(cmd));
  new_cmd->cmd = malloc(sizeof(char)*MAX_ARG_LEN);
  int tokenCount = 0;
  int tokenPos = 0;
  int inQuotes = 0;
  new_cmd->args[tokenCount]= malloc(sizeof(char)*MAX_ARG_LEN);
  for (int i = 0; i < strlen(buffer); i++) {
    if (buffer[i] == '"' && !inQuotes) {
        inQuotes = 1;
        continue;
    }
    if (buffer[i] == '"' && inQuotes) {
        inQuotes = 0;
        continue;
    }
    if ((buffer[i] == ' ' || buffer[i] == '\n') && !inQuotes) {
        new_cmd->args[tokenCount][tokenPos] = '\0';
        tokenCount++;
        tokenPos = 0;
        new_cmd->args[tokenCount]= malloc(sizeof(char)*MAX_ARG_LEN);
        continue;
    }
    new_cmd->args[tokenCount][tokenPos] = buffer[i];
    tokenPos++;
  }
  new_cmd->args[tokenCount][tokenPos] = '\0';
  new_cmd->argc = tokenCount+1;
  new_cmd->cmd = new_cmd->args[0];
  //print_cmd(new_cmd);
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
      if (execvp(command->cmd, command->args) < 0) {
          print_error(command->cmd);
          //perror(command->cmd);
      }
      exit(0);  // Exit the child process
  } else {
      // We are in the parent process
      wait(NULL);  // Wait for the child to exit
  }
}


// Utils

void print_error(char* cmd){
  char* error_prefix = colorize("🌌✨ Eldritch Error ✨🌌:", PURPLE);
  char* error_content = colorize("Alas, young adventurer!", RED);
  char* error_command = colorize(cmd, GREEN);
  char* error_suffix = colorize("the ancient scrolls", YELLOW);
  
  printf("%s\n%s The spell %s is unknown in this realm. Seek %s or try another incantation.📜🔮\n\n", error_prefix, error_content, error_command, error_suffix);
  printf(PURPLE);
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣾⣿⣿⣿⣿⠿⠿⠿⠿⢿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣾⣿⣿⣿⣿⠿⠿⠿⠿⢿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣾⣿⣿⣿⣿⠿⠿⠿⠿⢿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣾⣿⣿⣿⣿⠿⠿⠿⠿⢿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⠟⠉⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⠟⠉⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⠟⠉⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⠟⠉⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣧⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣧⠀⠀⠀⠠⡀⠀⠀⠀⠀⢀⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣧⠀⠀⠀⠠⡀⠀⠀⠀⠀⢀⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣧⠀⠀⠀⠠⡀⠀⠀⠀⠀⢀⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣧⠀⠀⠀⠠⡀⠀⠀⠀⠀⢀⣿⣿⡏⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⡷⣄⠀⠀⠘⠷⣦⣤⣶⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⡷⣄⠀⠀⠘⠷⣦⣤⣶⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⡷⣄⠀⠀⠘⠷⣦⣤⣶⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⡷⣄⠀⠀⠘⠷⣦⣤⣶⣿⡿⠋⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣶⠛⢓⣦⣀⠀⠈⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣶⠛⢓⣦⣀⠀⠈⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣶⠛⢓⣦⣀⠀⠈⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣶⠛⢓⣦⣀⠀⠈⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⡟⠉⠉⣹⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⡟⠉⠉⣹⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⡟⠉⠉⣹⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⡟⠉⠉⣹⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⢿⣿⣿⣿⡿⠛⠉⠉⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⢿⣿⣿⣿⡿⠛⠉⠉⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⢿⣿⣿⣿⡿⠛⠉⠉⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⢿⣿⣿⣿⡿⠛⠉⠉⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣧⣤⠤⠶⠿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣧⣤⠤⠶⠿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣧⣤⠤⠶⠿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣧⣤⠤⠶⠿⣷⡀⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⠁⠀⠀⠀⣨⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⠁⠀⠀⠀⣨⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⠁⠀⠀⠀⣨⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⠁⠀⠀⠀⣨⡇⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⠿⠶⠶⢿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⠿⠶⠶⢿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⠿⠶⠶⢿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⠿⠶⠶⢿⣿⡇⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⣿⣿⣿⣿⡏⠀⠀⠀⠀⣽⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⣿⣿⣿⣿⡏⠀⠀⠀⠀⣽⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⣿⣿⣿⣿⡏⠀⠀⠀⠀⣽⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣴⣿⣿⣿⣿⡏⠀⠀⠀⠀⣽⠁⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠀⢀⣠⣶⣿⣿⣿⣿⡿⠿⠿⠶⠶⢶⣶⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣶⣿⣿⣿⣿⡿⠿⠿⠶⠶⢶⣶⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣶⣿⣿⣿⣿⡿⠿⠿⠶⠶⢶⣶⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣶⣿⣿⣿⣿⡿⠿⠿⠶⠶⢶⣶⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀\n");
  printf("⠀⠀⠀⠀⠀⠈⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
  printf(RESET);
  // Free allocated memory
  free(error_prefix);
  free(error_content);
  free(error_command);
  free(error_suffix);
}

char* colorize(char* str, const char* color_code) {
  // Allocate memory for the colored string
  char* colored_str = malloc(strlen(color_code) + strlen(str) + strlen(RESET) + 1);
  if (!colored_str) {
      return NULL; // Memory allocation failed
  }

  // Construct the colored string
  sprintf(colored_str, "%s%s%s", color_code, str, RESET);
  
  return colored_str;
}
