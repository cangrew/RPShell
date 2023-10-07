#include <stdio.h>
#include <string.h>
#include <readline/history.h>
#include <unistd.h>

#include "resources.h"

int get_dragon_health() {
    int total_health = 1;
    FILE *ifp = fopen("quest/scale1", "r");

    if (ifp == NULL) {
        printf("dragon not found.");
        return -1;
    }

    char buffer[1024];
    while (fgets(buffer, 1024, ifp) != NULL) {
        // printf(">>%s<<\n", buffer);
        if (strcmp(buffer, "fireball\n") == 0) {
            total_health--;
        }
    }

    return total_health;
}

void print_dragon() {
    // TODO make the dragon look different based on its current health
    printf("        ,     \\    /      ,        \n");
    printf("       / \\    )\\__/(     / \\       \n");
    printf("      /   \\  (_\\  /_)   /   \\      \n");
    printf(" ____/_____\\__\\@  @/___/_____\\____ \n");
    printf("|             |\\../|              |\n");
    printf("|              \\VV/               |\n");
    printf("|        ----------------         |\n");
    printf("|_________________________________|\n");
    printf(" |    /\\ /      \\\\       \\ /\\    | \n");
    printf(" |  /   V        ))       V   \\  | \n");
    printf(" |/     `       //        '     \\| \n");
    printf(" `              V                '\n");
    printf("\n(https://www.asciiart.eu/mythology/dragons)\n\n");
    printf("A dragon is attacking! Find its weak spots and cast fireballs at them\n");
}

void potion(int* curMana) {
    printf("Drinking potion...\n");
    if (*curMana + 20 > 100) {
        *curMana = 100;
    }
    else {
        *curMana += 20;
    }
}

void history() {
    printf(PURPLE"📜 ~~~ "RESET"Chronicle of Past Incantations"PURPLE" ~~~ 📜\n"RESET);

    for (int i = 1; i <= history_length; i++) {
        HIST_ENTRY *entry = history_get(i);
        if (entry && entry->line) {
            // Determine the prefix emoji for each command (for simplicity, using the wizard emoji for all commands)
            printf(BLUE"🧙 "YELLOW"Verse %d:\n"RESET, i);
            printf(GREEN"   > %s\n"RESET, entry->line);
        }
    }

    printf(PURPLE"   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"RESET);
}

int cd(char* path){
    printf("Teleporting to...\n");
    // go to user's home directory
    if (path == NULL || (strcmp(path,"~") == 0)) {
        const char *home_directory = getenv("HOME");
        //printf(getenv("HOME"));

        if(home_directory == NULL) {
            printf("Can't find Home.\n");
            return -1;
        }
        else if(chdir(home_directory) != 0) {
            printf("Failed to cast Home spell.\n");
            return -1;
        }
    printf("Back Home. 🏡\n");
    return 0;
    }

}