Programmation système

Dans ce mini-projet de TP, nous nous proposons de réaliser un micro-shell qui permettra de réaliser différentes tâches.

Question 1 :

Dans un premier temps, nous allons simplement devoir afficher un message d’accueil ainsi qu’un prompt simple.

Pour ce faire, nous allons définir une fonction permettant d’afficher un message sur la console, il s’agit de la fonction display(). Lorsque nous exécutons le code, nous utilisons une première fois cette fonction pour afficher le message de bienvenue, puis nous entrons dans une boucle infinie qui utilise la fonction buildPrompt() pour afficher le prompt correspondant (celui-ci dépend de plusieurs choses qui seront vues dans des questions plus tard). Grâce à cela, voici ce que nous obtenons en sortie :

<img width="840" height="79" alt="image" src="https://github.com/user-attachments/assets/8bb53781-247f-4ead-8118-db5ca5aa4636" />

Ici, nous obtenons bien ce que nous voulions : un message d’accueil et un prompt simple.

Question 2 :

Nous souhaitons maintenant que notre shell puisse faire quelque chose, c’est-à-dire exécuter une commande.

Afin de pouvoir exécuter une commande donnée par l’utilisateur, nous devons lire ce que ce dernier écrit sur la console. C’est ce à quoi sert la fonction readCommand(). Grâce à cette fonction, nous pouvons lire ce que l’utilisateur écrit pour ensuite traiter ce qui est demandé.

Dans un premier temps, nous voulons simplement pouvoir faire des actions telles que des ls ou autres. Nous allons donc créer la fonction executeCommand(). Comme son nom l’indique, cette fonction exécute ce qui lui a été donné en entrée. Afin de faire le tout fonctionner, nous lisons dans la boucle infinie du main avant de traiter la demande. Voici ce que nous pouvons par exemple faire dans la console :

<img width="841" height="146" alt="image" src="https://github.com/user-attachments/assets/56c124d7-2aef-4125-bd0e-d934a289bf23" />

Ici, nous avons bien les commandes ls et date qui se sont exécutées dans notre shell telles qu’il le fallait.

Question 3 :

Nous voulons à présent pouvoir gérer l’arrêt du shell si l’utilisateur écrit "exit" ou qu’il appuie sur Ctrl + D. Afin de faire cela, nous écrivons la nouvelle fonction endCommand(), qui, en fonction de ce qu’il lui est donné en entrée, renvoie True ou False. Nous lui définissons donc les deux conditions d’arrêt en tant que conditions, et nous ajoutons un if() juste après avoir lu ce que l’utilisateur écrit dans le main. Ce if() affiche le message de fermeture et quitte le programme si la condition est True.

Voici une capture d’écran de test pour la gestion de la fermeture :

<img width="843" height="115" alt="image" src="https://github.com/user-attachments/assets/c67344b3-c9b8-40a3-b313-8e680a62d348" />

Questions 4 et 5 :

Dans cette partie, nous cherchons à programmer le code de retour de la commande précédente dans le prompt suivant, ainsi que le temps d’exécution de cette commande. C’est ici que nous définissons la fonction buildPrompt() que nous avons déjà utilisée. Cette fonction permet de définir le prompt à afficher. En effet, s’il s’agit du premier prompt, cela n’a pas de sens de donner le retour de la commande précédente ou son temps d’exécution, puisque cela serait faux. Cependant, si nous avions un prompt précédemment, nous affichons le retour de ce dernier ainsi que le temps mis.

Le temps mis par une commande est calculé par la différence de temps. En effet, dans le main, avant de lancer la commande, nous allons lancer un timer, que nous arrêterons juste après avoir fini la tâche. Nous avons défini les fonctions de lancement et d’arrêt du timer avec startTimer() et stopTimer().

Voici ce que nous pouvons obtenir en console pour ces deux fonctions :

<img width="366" height="87" alt="image" src="https://github.com/user-attachments/assets/fe67e0bb-a179-403a-b168-9c1729489d17" />

Nous avons bien le retour de la fonction précédente ainsi que le temps mis par celle-ci.

Question 6 :

Les commandes que nous pouvions faire jusqu’à présent ne pouvaient pas prendre d’arguments, ce qui limite beaucoup l’utilisation de notre shell. Nous cherchons maintenant à pouvoir aussi traiter les commandes avec des arguments.

Afin de faire ceci, nous avons écrit la fonction handleArguments(). Cette fonction nous permet, dès que la commande comporte des espaces, de comprendre qu’il s’agit d’arguments et de pouvoir les obtenir afin de les utiliser. Cette fonction est appelée dans notre executeCommand() avant d’exécuter la commande pour savoir et donner les arguments entrés par l’utilisateur. Voici un exemple de ce que nous pouvons maintenant faire avec ce code :

<img width="599" height="208" alt="image" src="https://github.com/user-attachments/assets/525c4355-d3d7-4d05-8412-3905f72bf9d7" />

Questions 7 et 8 :

Nous souhaitons maintenant être capables de gérer les redirections vers stdin et stdout, puis dans un second temps de gérer les pipes.

Afin de traiter les redirections, il faut être capable de détecter celles-ci. C’est ce à quoi sert la fonction parseCommand(). En effet, cette fonction regarde la commande entrée par l’utilisateur et réagit en fonction de ce qu’il y a dedans. S’il y a <, la fonction signale un fichier d’entrée, s’il y a >, alors il s’agit d’un fichier de sortie. Ceci est alors utilisé dans la fonction executeSimpleCommand(), où, en fonction que ce soit un fichier d’entrée ou de sortie, le programme va gérer le stdin ou le stdout.

Pour les pipes, nous avons écrit la fonction executePipeCommand(), qui utilise deux fork permettant de faire un stdout et un stdin à la suite.

Il suffit maintenant, dans le main, d’exécuter la fonction pipe s’il y a un |, sinon de faire le traitement normal. Voici ce que nous pouvons maintenant faire avec ce code :

<img width="690" height="164" alt="image" src="https://github.com/user-attachments/assets/40e4187f-6488-4b7d-9615-7772091bc3ec" />

Conclusion :

Dans ce mini-projet de TP, nous avons réalisé un shell permettant de réaliser plusieurs tâches, telles que l’exécution de commandes simples ou complexes, l’affichage des codes de retour de chaque commande ainsi que leur temps d’exécution, et la gestion des redirections stdin, stdout et des pipes.
