#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "utility.h"

#define Welcome "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define Exit "Fin du programme\n"
#define BUFFER_SIZE 128

char buffer[BUFFER_SIZE], prompt[BUFFER_SIZE];
ssize_t buffer_l;
int duration = 0, status;
bool has_status = false;
struct timespec t_start, t_end;

/*
Description : Point d'entrée du Shell ENSEA                                             (Question 1)
Fonctionnement : 
    - Gère la boucle infinie (REPL)
    - Mesure le temps d'exécution
    - Identifie la présence de pipes pour orienter l'exécution
*/
int main() {
    

    // Configuration : Ignorer Ctrl+C dans le shell parent pour éviter de le fermer       (Question 9)
    signal(SIGINT, SIG_IGN);

    display(Welcome);

    while (1) {
        // Affichage du prompt dynamique (code de sortie + temps)                       (Questions 4 & 5)
        buildPrompt(prompt, has_status, status, duration);
        display(prompt);

        // Lecture de l'entrée utilisateur                                              (Question 2)
        readCommand(buffer, BUFFER_SIZE, &buffer_l);

        // Vérification de la condition de sortie (exit ou Ctrl+D)                       (Question 3)
        if (endCommand(buffer, &buffer_l)) {
            display(Exit);
            break;
        }

        startTimer(&t_start);

        // Analyse de la commande pour détecter un pipe '|'                              (Question 8)
        char* pipe_pos = strchr(buffer, '|');

        if (pipe_pos) {
            /* Cas 1 : Exécution avec un pipe 
               Fonctionnement : On coupe la chaîne en deux au niveau du '|'
            */
            *pipe_pos = '\0'; 
            executePipe(buffer, pipe_pos + 1, &status);
        } else {
            /* Cas 2 : Exécution d'une commande simple
               Fonctionnement : fork classique et exécution dans le fils
            */
            pid_t pid = fork();
            if (pid < 0) {
                display(ForkError);
            } else if (pid == 0) {
                // Le fils exécute la commande (gère aussi < et >)                      (Questions 6 & 7)
                executeSimpleCommand(buffer);
            } else {
                // Le parent attend la fin du processus fils
                waitpid(pid, &status, 0);
            }
        }

        // Calcul du temps écoulé après l'exécution                                     (Question 5)
        stopTimer(&t_start, &t_end, &duration);
        has_status = true; 
    }

    return 0;
}