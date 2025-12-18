#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>
#include "utility.h"

#define Welcome "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define Prompt "enseash % "
#define Exit "Fin du programme\n"
#define BUFFER_SIZE 128

char buffer[BUFFER_SIZE];
char prompt[BUFFER_SIZE] = "enseash % ";
int duration=0;
int status;
bool has_status = false;
struct timespec t_start, t_end;

int main() {
    display(Welcome);
    while (1) {
        buildPrompt(prompt, has_status, status,duration);
        display(prompt);
        readCommand(buffer,BUFFER_SIZE);
        if (endCommand(buffer)) {               // Sortie du shell
            display(Exit);
            break;                              // Le break permet d'interrompre la boucle
        }
        startTimer(&t_start);
        executeCommand(buffer, &status);
        stopTimer(&t_start, &t_end, &duration);
        has_status = true; 

    }
    return 0;
}