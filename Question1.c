#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void commande(char *entree) {
	entree[strcspn(entree, "\n")] = '\0';
	if (strcmp(entree, "test") == 0) {
		write(1, "Ceci est un test.\r\n", 20);
	}
	else if(strcmp(entree, "exit") == 0) {
		write(1, "Fermeture de l'EnseaSH.\r\n", 25);
		exit(0);
	}
	else {
		write(1, "Commande inconnue.\r\n", 21);
	}
}

int main (){
	char buf[100];
	write(1,"Bienvenue dans le Shell ENSEA.\r\n", 33);
	write(1,"Pour quitter, tapez 'exit'.\r\n", 29);
	
	while(1){
		write(1,"enseash % ", 10);
		int n = read(0, buf, sizeof(buf)-1);
		buf[n] = '\0';
		write(1,buf, n);
		commande(buf);
	}

return 0;
}
