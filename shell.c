#include "builtin.h"
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
#include <signal.h>

#include "resources.h"

#define MAX_CMD_SIZE 256
#define MAX_ARG_SIZE 25
#define MAX_ARG_LEN  100

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
void executeWait();
void handle_sigint(int sig, siginfo_t *si, void *unused);
void setup_sigint_handler();
int check_status();

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
  read_history("resources/history");
  setup_sigint_handler();


  while(1) {
    cwd = getcwd(NULL, 0);
    read_cmd();
    add_history(buffer);
    cmd* cur_cmd = tokenize();
    // for(int i = 0; i < cur_cmd->argc; i++){
    //   printf("%d %s\n",i,cur_cmd->args[i]);
    // }
    // printf("%s",cur_cmd->cmd);
    execute(cur_cmd);
    write_history("resources/history");
    if(check_status()){
      break;
    }
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
  sprintf(prompt,"<"RED"HP✨ %d/%d"RESET" | "BLUE"Mana🌙 %d/%d"RESET"> "GREEN"%s@%s"RESET" [%s] $ ",curHP,maxHP,curMana,maxMana,user,hostname,dir_name);
}

cmd* tokenize(){
  cmd* new_cmd = malloc(sizeof(cmd));
  new_cmd->cmd = malloc(sizeof(char)*MAX_ARG_LEN);
  int tokenCount = 0;
  int tokenPos = 0;
  int inQuotes = 0;
  new_cmd->args[tokenCount]= malloc(sizeof(char)*MAX_ARG_LEN);
  for (size_t i = 0; i < strlen(buffer); i++) {
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
  if(strcmp(command->cmd, "potion") == 0){
      potion(&curMana);
  }
  else if(strcmp(command->cmd, "long_rest") == 0){
      curMana = 100;
      curHP = 100;
      //potion(&curMana);
  }
  else if (curMana <= 0) {
      printf("You don't have enough mana.\n");
      return 0;
  }
  else if(strcmp(command->cmd, "exit") == 0){
      exit(0);
  }
  else if(strcmp(command->cmd, "help") == 0){
      help();
  }
  else if(strcmp(command->cmd, "cd") == 0){
      curMana -= 10;
      char* path = command->args[1];
      cd(path);
  }
  else if(strcmp(command->cmd, "history") == 0){
      curMana -= 20;
      history();
  }
  else if(strcmp(command->cmd, "echo") == 0){
      curMana -= 10;
      echo(NULL);
  }
  else if(strcmp(command->cmd, "pwd") == 0){
      curMana -= 10;
      pwd();
  }
  else if(strcmp(command->cmd, "alias") == 0){
      curMana -= 10;
      alias(command->args[1]);
  }
  else if(strcmp(command->cmd, "wait") == 0){
      curMana -= 5;
      executeWait();
  }
  else if(strcmp(command->cmd, "quest") == 0){
      int dragon_health = get_dragon_health();
      if (dragon_health == 0) {
          print_dead_dragon();
      }
      else {
          print_alive_dragon();
          printf("  (Dragon health: %d)\n", dragon_health);
      }
  }
  else{
    external_exec(command);
      curMana -= 10;
  }
  return 0;
}

int external_exec(cmd* command){
  int fd[2];
  char buf[256];
  memset(buf, 0, sizeof(buf));

  if (pipe(fd) == -1) {
      perror("pipe");
      return 1;
  }

  pid_t pid = fork();

  if (pid < 0) {
      perror("Failed forking child");
      return 1;
  } else if (pid == 0) {
      // We are in the child process
      close(fd[0]); // Close the read end of the pipe

      if (execvp(command->cmd, command->args) < 0) {
          snprintf(buf, sizeof(buf), "%s", command->cmd);
          write(fd[1], buf, strlen(buf)); // Notify the parent of the error
          exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);  // Exit the child process
  } else {
      // We are in the parent process
      close(fd[1]); // Close the write end of the pipe
      wait(NULL);   // Wait for the child to exit
      
      if (read(fd[0], buf, sizeof(buf)) > 0) {
          print_error(buf);
          curHP -= 10;
          return 1;
      }
  }
  return 0;
}

// Utils

void print_error(char* cmd){
  char* error_prefix = colorize("🌌✨ Eldritch Error ✨🌌:", PURPLE);
  char* error_content = colorize("Alas, young adventurer!", RED);
  char* error_command = colorize(cmd, GREEN);
  char* error_suffix = colorize("the Enchanted Tome of Guidance", YELLOW);
  
  printf(PURPLE);
  printf("\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀\n");
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
  printf("%s\n%s The spell %s is unknown in this realm. Seek %s or try another incantation.📜🔮\n\n", error_prefix, error_content, error_command, error_suffix);

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

void executeWait() {
  printf("Loading ");
  fflush(stdout);  // Flush the buffer to display text immediately

  // Display loading bar for 2 seconds
  for (int i = 0; i < 20; i++) {
      putchar('#');
      fflush(stdout);  // Flush the buffer to display '#' immediately
      usleep(100000);  // Sleep for 100ms
  }
  putchar('\n');
}


void handle_sigint(int sig __attribute__((unused)), siginfo_t *si __attribute__((unused)), void *unused __attribute__((unused))) {
  //printf("\n🔮 "YELLOW"Ctrl+C?"RED" Oops! You just turned your coffee into a squirrel."RESET" Next time, just say "YELLOW"exit!"RESET" 🐿️☕\n");
  printf(DARK_PURPLE "\n🌌 Ah, "PURPLE"Ctrl+C"DARK_PURPLE"! A mere mortal's plea to the abyss. But the void is vast and indifferent. Whisper "YELLOW"exit"DARK_PURPLE" to find your way back to reality, lest you remain trapped in this liminal space forever... 🌑\n"RESET);
  fflush(stdout); // Make sure the message is immediately printed
  rl_forced_update_display();
}

void setup_sigint_handler() {
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));

  sa.sa_sigaction = handle_sigint;
  sa.sa_flags = SA_SIGINFO;

  // Setup the signal handler for SIGINT
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE); // Exit if we can't set the handler
  }
}

int check_status(){
  if(curHP <= 0) {
    printf(PURPLE "🔮💫 *BLIP!* 💫🔮\n" RESET);
    printf("Uh-oh, you've run out of mystical shell HP! Seems like that last command was a dragon-level challenge, eh? 😅\n\n");
    printf(RED "🐉🔥 \"Mwahahaha! Thought you could out-code the mighty DrakoTerminalus? Guess again, mere mortal coder!\"\n\n" RESET);
    printf(GREEN "Quick, reach into your pocket! Do you have a potion? No? A mana cookie? An enchanted USB stick? Anything?! 🍪✨\n\n" RESET);
    printf(YELLOW "🌌 Remember:"RESET" In the world of Terminalia, when life gives you a " YELLOW "`Segmentation Fault`" RESET ", make a " YELLOW "`Magic Potion`" RESET ". But for now... perhaps just restarting the shell will do. 😉\n" RESET);
    printf("See ya on the other side of the respawn, brave coder! And next time, bring a " CYAN "+5 enchanted keyboard." RESET " 🪄⌨️\n\n");
    return 1;
  }
  return 0;
}