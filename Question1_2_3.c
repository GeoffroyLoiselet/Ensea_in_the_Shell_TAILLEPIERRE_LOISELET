#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MSG_Welcome "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define MSG_Prompt "enseash % "
#define MSG_Exit "Fin du programme\n"
#define MSG_ForkError "Erreur de fork\n"
#define MSG_InvalidCommand "La commande saisie est introuvable \n"
#define PROMPT_LEN strlen(PROMPT)
#define BUFFER_SIZE 128

char buffer[BUFFER_SIZE]; // Command typed by the user
ssize_t command; // Length of the Command

// Function that prints in the terminal
void print(const char* msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

// Function that reads the command typed in the terminal and formats it
void readCommand(char* buffer, ssize_t* cmd) {
    *cmd = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    buffer[command] = '\0';
    if (buffer[command - 1] == '\n') {
        buffer[command - 1] = '\0';
    }
    printf("Command typed : %s\n", buffer);
}

int main() {
    print(MSG_Welcome);

    while (1) {
        print(MSG_Prompt);
        readCommand(buffer, &command);

        if (strcmp(buffer, "exit") == 0) {
            print(MSG_Exit);
            break;
        }

        pid_t pid = fork();
        pid_t status;

        if (pid < 0) {
            print(MSG_ForkError);
        } else if (pid > 0) {
            wait(&status);
        } else {
            char *args[] = {buffer, NULL};
            execvp(buffer, args);
            print(MSG_InvalidCommand);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

