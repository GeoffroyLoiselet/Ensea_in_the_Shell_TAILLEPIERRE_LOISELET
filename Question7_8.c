#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define MSG_Welcome "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define MSG_Exit "Fin du programme\n"
#define MSG_ForkError "Erreur de fork\n"
#define MSG_InvalidCommand "La commande saisie est introuvable \n"

#define BUFFER_SIZE 128
#define PROMPT_SIZE 64
#define MAX_ARGS 64

char buffer[BUFFER_SIZE];
ssize_t command_len;

// Fonction d'écriture simple
void print(const char* msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

// Lecture de la commande entrée par l'utilisateur
void readCommand(char* buffer, ssize_t* cmd_len) {
    *cmd_len = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

    if (*cmd_len == 0) {
        buffer[0] = '\0';
        return;
    }

    buffer[*cmd_len] = '\0';

    if (buffer[*cmd_len - 1] == '\n') {
        buffer[*cmd_len - 1] = '\0';
    }
}

// Analyse de la commande pour séparer les arguments et les fichiers d'entrée/sortie
void parseCommand(char* cmd, char** args, char** input_file, char** output_file) {
    int i = 0;
    char* token = strtok(cmd, " ");

    while (token != NULL && i < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            *input_file = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            *output_file = token;
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }

    args[i] = NULL;
}

int main() {
    int duration = 0;
    int status;
    int has_status = 0;
    char prompt_msg[PROMPT_SIZE];

    // Ignorer Ctrl+C dans le shell principal
    signal(SIGINT, SIG_IGN);

    print(MSG_Welcome);

    while (1) {
        // Affichage du prompt
        if (has_status) {
            if (WIFSIGNALED(status)) {
                sprintf(prompt_msg, "enseash [sign:%d|%dms] %% ", WTERMSIG(status), duration);
            } else if (WIFEXITED(status)) {
                sprintf(prompt_msg, "enseash [exit:%d|%dms] %% ", WEXITSTATUS(status), duration);
            }
        } else {
            sprintf(prompt_msg, "enseash %% ");
        }

        print(prompt_msg);

        // Lecture de la commande
        readCommand(buffer, &command_len);

        // Gestion de la commande 'exit' ou entrée vide
        if ((command_len == 0) || strcmp(buffer, "exit") == 0) {
            print(MSG_Exit);
            break;
        }

        // Mesure du temps d'exécution
        struct timespec t_start, t_end;
        clock_gettime(CLOCK_REALTIME, &t_start);

        // Vérifie si la commande contient un pipe '|'
        char* pipe_pos = strchr(buffer, '|');

        if (pipe_pos) {
            *pipe_pos = '\0';
            char* cmd1_str = buffer;
            char* cmd2_str = pipe_pos + 1;

            char* args1[MAX_ARGS];
            char* args2[MAX_ARGS];
            char* in1 = NULL;
            char* out1 = NULL;
            char* in2 = NULL;
            char* out2 = NULL;

            parseCommand(cmd1_str, args1, &in1, &out1);
            parseCommand(cmd2_str, args2, &in2, &out2);

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                print(MSG_ForkError);
                continue;
            }

            pid_t pid1 = fork();
            if (pid1 == 0) {
                // Processus du premier côté du pipe
                if (out1) {
                    int fd = open(out1, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                } else {
                    dup2(pipefd[1], STDOUT_FILENO);
                }

                close(pipefd[0]);
                close(pipefd[1]);

                if (in1) {
                    int fd = open(in1, O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                signal(SIGINT, SIG_DFL);
                execvp(args1[0], args1);
                print(MSG_InvalidCommand);
                exit(EXIT_FAILURE);
            }

            pid_t pid2 = fork();
            if (pid2 == 0) {
                // Processus du second côté du pipe
                if (in2) {
                    int fd = open(in2, O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                } else {
                    dup2(pipefd[0], STDIN_FILENO);
                }

                close(pipefd[1]);
                close(pipefd[0]);

                if (out2) {
                    int fd = open(out2, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                signal(SIGINT, SIG_DFL);
                execvp(args2[0], args2);
                print(MSG_InvalidCommand);
                exit(EXIT_FAILURE);
            }

            // Fermeture des descripteurs du pipe dans le parent
            close(pipefd[0]);
            close(pipefd[1]);

            // Attente des deux processus
            waitpid(pid1, NULL, 0);
            waitpid(pid2, &status, 0);

        } else {
            // Commande simple sans pipe
            pid_t pid = fork();
            if (pid < 0) {
                print(MSG_ForkError);
            } else if (pid > 0) {
                waitpid(pid, &status, 0);
            } else {
                char* args[MAX_ARGS];
                char* input_file = NULL;
                char* output_file = NULL;

                parseCommand(buffer, args, &input_file, &output_file);

                if (input_file) {
                    int fd = open(input_file, O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                if (output_file) {
                    int fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                signal(SIGINT, SIG_DFL);
                execvp(args[0], args);
                print(MSG_InvalidCommand);
                exit(EXIT_FAILURE);
            }
        }

        // Calcul de la durée d'exécution
        clock_gettime(CLOCK_REALTIME, &t_end);
        long sec_diff = t_end.tv_sec - t_start.tv_sec;
        long nsec_diff = t_end.tv_nsec - t_start.tv_nsec;
        duration = sec_diff * 1000 + nsec_diff / 1000000;
        has_status = 1;
    }

    return 0;
}

