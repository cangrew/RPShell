#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <libgen.h>

#define MAX_CMD_SIZE 256


#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

char buffer[MAX_CMD_SIZE];

int maxHP = 100;
int curHP = 100;
int maxMana = 100;
int curMana = 100;

char* user;
char hostname[1024];
char* cwd;

void print_prompt();

int main(){
  user = getlogin();
  gethostname(hostname, 1024);
  cwd = getcwd(NULL, 0);

  while(1) {
    print_prompt();
    scanf("%s",buffer);
    if(strcmp("exit",buffer) == 0){
      return 0;
    }
  }
  return 0;
}

void print_prompt(){
  printf("< %sHP %d/%d%s | %sMana %d/%d%s >",RED,curHP,maxHP,RESET,BLUE,curMana,maxMana,RESET);
  printf(" %s%s@%s%s",GREEN,user,hostname, RESET);
  char* dir_name = basename(cwd);
  printf(" [%s] $", dir_name);
}