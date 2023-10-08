#include <stdio.h>
#include <stdlib.h>
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
    printf(PURPLE"📜 ~~~ "YELLOW"Chronicle of Past Incantations"PURPLE" ~~~ 📜\n"RESET);

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

void help() {
    printf(PURPLE "📜 ~~~ " YELLOW "Enchanted Tome of Guidance" PURPLE " ~~~ 📜\n\n" RESET);

    printf("🧙 " GREEN "Commands and Arcane Rituals:\n\n" RESET);

    printf(YELLOW "Teleport (cd) 🚪\n" RESET);
    printf(CYAN "   - Use this incantation to traverse through magical portals to different realms (directories).\n\n");

    printf(YELLOW "Chronicle of Past Incantations (History) 📜\n" RESET);
    printf(CYAN "   - Recount tales (commands) of old. Relive your journey through this mystical realm.\n\n");
    
    printf(YELLOW "pwd 🧭\n" RESET);
    printf(CYAN "   - Summon this spell to reveal your current location within the vast labyrinth of realms.\n\n");

    printf(YELLOW "echo 🗣️\n" RESET);
    printf(CYAN "   - Channel your inner bard! Proclaim words and let them echo through the land.\n\n");

    printf(YELLOW "exit 🌀\n" RESET);
    printf(CYAN "   - Utter this spell when you wish to leave the mystical shell and return to the mundane world.\n\n");

    printf(BLUE "🔮 " GREEN "Sage Advice:\n\n" RESET);
    printf("1. Spells and incantations are potent; wield them with wisdom and care.\n");
    printf("2. The realms (directories) are vast; always use the 🧭 to know your standing.\n");
    printf("3. In times of uncertainty, the 'help' incantation is but a whisper away.\n");

    printf(PURPLE "\n   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
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
    }

    // go to directory
    if (!(path == NULL)) {
        const char *user_path = getenv(path);

        if(user_path == NULL) {
            printf("Can't find The Path.\n");
            return -1;
        }
        else if(chdir(user_path) != 0) {
            printf("Failed to cast Teleport spell.\n");
            return -1;
        }
    printf("%s! 🧙‍♂️\n", path);
    }
    // success.
    return 0;
}