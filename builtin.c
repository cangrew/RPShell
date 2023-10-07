#include <stdio.h>
#include <string.h>

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
