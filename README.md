# Netflix Prize - Macéo Tuloup, Valentin Foulon

## Sommaire
- [Netflix Prize - Macéo Tuloup, Valentin Foulon](#netflix-prize---macéo-tuloup-valentin-foulon)
  - [Sommaire](#sommaire)
  - [Qu'est-ce que c'est ?](#quest-ce-que-cest-)
  - [Comment l'utiliser ?](#comment-lutiliser-)
    - [films\_liked.txt](#films_likedtxt)
  - [Structures binaires utilisées](#structures-binaires-utilisées)
  - [Détails et choix d'implémentation](#détails-et-choix-dimplémentation)
    - [SystemAbstraction](#systemabstraction)
    - [Structures des fichiers](#structures-des-fichiers)
    - [Ligne de commande](#ligne-de-commande)
    - [Tests](#tests)
  - [Quelques données et statistiques](#quelques-données-et-statistiques)
  - [A améliorer](#a-améliorer)

## Qu'est-ce que c'est ?
Un projet qui fournit des recommandations de films à partir d'une base de données de notes par des utilisateurs de Netflix. Il est composé de trois programmes :

- <u>film_parser</u> : prend en entrée tous les avis des utilisateurs et les organise dans un fichier binaire de données beaucoup plus petit et rapide à lire.
- <u>film_stats</u> : prend en entrée le fichier des titres et années des films, des options de filtres sur les avis et le fichier binaire de données et calcule des statistiques et les recommandations pour chaque film en fonction d'une liste spécifiée de films aimés. Il écrit ensuite les statistiques dans un fichier binaire de stats.
- <u>film_gui</u> : prend en entrée le fichier binaire de stats et affiche les recommandations par ordre décroissant et statistiques dans une interface graphique. On peut ensuite effectuer une recherche par nom.

## Comment l'utiliser ?

La première étape est d'installer [xmake](https://xmake.io). Les instructions sont disponibles [ici](https://xmake.io/#/guide/installation).

Ensuite il faut installer SystemAbstraction, la bibliothèque d'abstraction de Macéo, via la branche beta du repo git, puis la compiler et l'installer
```
git clone https://github.com/mactul/system_abstraction -b beta
cd system_abstraction
xmake
xmake install
```

Ensuite vous pourrez vous occuper du projet

```
git clone https://git.unistra.fr/vfoulon/projet-programmation
cd projet-programmation
xmake
```

Pour forcer xmake à tout recompiler (équivalent de `make -B`), vous pouvez appeler `xmake -r`

Une documentation doxygen est fournie, pour la générer, vous pouvez appeler `xmake docs` ou alors `doxygen` dans le dossier courant. Les fichiers de documentation se situeront alors dans le dossier `doc/html/`

Pour obtenir une liste de recommandations avec le jeu de données fourni, vous pouvez appeler:

D'abord:
```
./bin/film_parser ./download/training_set/mv_*.txt
```

Puis:
```
./bin/film_stats
```
ou mieux:
```
./bin/film_stats -r ./data/films_liked.txt
```
pour avoir des recommandations basées sur les films que vous aimez

Enfin:
```
./bin/film_gui download/movie_titles.txt
```

### films_liked.txt

Lorsque l'on utilise film_stats avec l'option -r, il faut fournir un fichier texte.  
Ce fichier est formaté avec un identifiant de film par ligne.  
L'exemple fourni dans data/film_liked.txt contient les IDs de Nemo et Toys Story.
Les films retournées en premier dans l'interface graphique seront donc des films proches de ces Pixars.

## Structures binaires utilisées

data.bin
```
 63    56|55    48|47    40|39    32|31    24|23    16|15     8|7      0
 
+-----------------------------------------------------------------------+
|                               Film count                              |
+-----------------------------------+-----------------------------------+
|         (1) Rating count          |            (1) Film ID            |
+-----------------------------------+-----------------------------------+
|                          (1) Ratings offset                           |--+
+-----------------------------------+-----------------------------------+  |
|         (2) Rating count          |            (2) Film ID            |  |
+-----------------------------------+-----------------------------------+  |
|                          (2) Ratings offset                           |--|--+
+-----------------------------------+-----------------------------------+  |  |
|         (3) Rating count          |            (3) Film ID            |  |  |
+-----------------------------------+-----------------------------------+  |  |
|                          (3) Ratings offset                           |--|--|--+
+-----------------------------------+-----------------------------------+  |  |  |
|                                                                       |  |  |  |
|                                  ...                                  |  |  |  |
|                                                                       |  |  |  |
+-----------------------------------+--------+--------+--------+--------+<-+  |  |
|          (1) user_id_a1           |(1) note| (1) dd | (1) mm | (1) yy |     |  |
+-----------------------------------+--------+--------+--------+--------+     |  |
|          (1) user_id_a2           |(1) note| (1) dd | (1) mm | (1) yy |     |  |
+-----------------------------------+--------+--------+--------+--------+     |  |
|          (1) user_id_a3           |(1) note| (1) dd | (1) mm | (1) yy |     |  |
+-----------------------------------+--------+--------+--------+--------+<----+  |
|          (2) user_id_b1           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |
|          (2) user_id_b2           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |   (user list sorted by user_id)
|          (2) user_id_b3           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |
|          (2) user_id_b4           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |
|          (2) user_id_b5           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+<-------+
|          (3) user_id_c1           |(3) note| (3) dd | (3) mm | (3) yy |
+-----------------------------------+--------+--------+--------+--------+
|          (3) user_id_c2           |(3) note| (3) dd | (3) mm | (3) yy |
+-----------------------------------+--------+--------+--------+--------+
|                                                                       |
|                                  ...                                  |
|                                                                       |
+-----------------------------------------------------------------------+
|                              user_count                               |
+-----------------------------------+-----------------+-----------------+
|             user_id1              |   rating_count  |    avg_note     |
+-----------------------------------+-----------------+-----------------+
|             user_id2              |   rating_count  |    avg_note     |   (user list sorted by user_id)
+-----------------------------------+-----------------+-----------------+
|             user_id3              |   rating_count  |    avg_note     |
+-----------------------------------+-----------------+-----------------+
|                                                                       |
|                                  ...                                  |
|                                                                       |
+-----------------------------------------------------------------------+
```

stats.bin
```
 63    56|55    48|47    40|39    32|31    24|23    16|15     8|7      0
 
+-----------------------------------------------------------------------+
|                               Stat count                              |
+-----------------------------------+-----------------------------------+
|            (1) film_id            |        (1) recommendation         |
+-----------------------------------+-----------------------------------+
|           (1) max_year            |   (1) mean_rating_over_years[0]   |
+-----------------------------------+-----------------------------------+
|   (1) mean_rating_over_years[1]   |   (1) mean_rating_over_years[2]   |
+-----------------------------------+-----------------------------------+
|                                                                       |
|                                  ...                                  |
|                                                                       |
+-----------------------------------+-----------------------------------+
|   (1) mean_rating_over_years[7]   |   (1) mean_rating_over_years[8]   |
+-----------------------------------+-----------------------------------+
|   (1) mean_rating_over_years[9]   |(1) kept_rating_count_over_years[0]|
+-----------------------------------+-----------------------------------+
|(1) kept_rating_count_over_years[1]|(1) kept_rating_count_over_years[2]|
+-----------------------------------+-----------------------------------+
|                                                                       |
|                                  ...                                  |
|                                                                       |
+-----------------------------------+-----------------------------------+
|(1) kept_rating_count_over_years[7]|(1) kept_rating_count_over_years[8]|
+-----------------------------------+-----------------------------------+
|(1) kept_rating_count_over_years[9]|            (2) film_id            |
+-----------------------------------+-----------------------------------+
|                                                                       |
|                                  ...                                  |
|                                                                       |
+-----------------------------------+-----------------------------------+
```

## Détails et choix d'implémentation

### SystemAbstraction

Ce projet a mené a la création d'énormément de fonctions, qui auraient put être simplement mises dans le projet mais sont destinées à être utilisées ailleurs également.  
Nous avons fait le choix de les inclure à la bibliothèque [SystemAbstraction](https://github.com/mactul/system_abstraction/tree/beta) (abrégée en SA), dont les commits des derniers mois sont exclusivement dédiés à fournir des fonctions pour ce projet.  
Nous avons créé une branche `beta` sur ce repository, cette branche contiendra la version de la librairie à la fin du projet le 19/01/2024 (à l'exception peut-être de la documentation qui pourrait évoluer), les autres branches pourraient ne plus êtres compatibles car antérieures ou bien ayant subi des breaking changes.

La GUI du projet est basée sur la surcouche graphique fournie par SA.  
SA a été créée pour pouvoir être branchée sur n'importe quelle bibliothèque graphique, mais pour l'instant, elle utilise X11, qui est la façon native de communiquer avec le gestionnaire de fenêtres sous Linux, cependant ce protocole est vieux et dépassé, ce qui nous a amené de nombreux problèmes.  
Un de ceux que nous n'avons pas pu résoudre pour le moment est que l'une des fonctions de X11 (XLookupString) est mal conçue et ne libère pas la mémoire qu'elle alloue lors de la fermeture du programme.  
Valgrind indique donc que des blocs sont non-libérés mais cela ne vient pas directement de notre code...


### Structures des fichiers

La structure du fichier data.bin a été choisie car elle est quasiment identique aux données stockées en RAM mais à la place de pointeurs, on note la position du curseur dans le fichier.

La structure Rating est faite pour tenir sur 8 octets. Pour garder en mémoire l'année sur seulement 1 octet, nous avons gardé le nombre d'années depuis 1850 (YEARS_OFFSET). Chacune de ces structures dans notre fichier binaire est alors alignée sur 8 octets.

De même la structure du fichier stats.bin est une représentation directe de la RAM, elle peut donc être lue et écrite TRÈS rapidement.

### Ligne de commande

Chaque programme possède une aide, affichable a l'aide de `./bin/[program] -h`.

Certaines options de ligne de commande ont été rajoutées ou supprimées en fonction de ce qui nous paraissait plus pratique.  
Par exemple l'option `-f FOLDER` a été remplacé à chaque fois par `-o OUTPUT_FILE_PATH`, l'option `-s FILM_ID` a été supprimée car on peut effectuer une recherche par nom de film dans l'interface graphique, les films likés sont passés dans un fichier par l'option `-r LIKED_FILMS_FILE_PATH`, l'option `-n LIMIT` n'existe pas car toutes les suggestions sont affichées dans l'interface graphique, et l'option `-t TIMEOUT` n'est pas implémentée car on peut simplement lancer notre programme avec l'outil `timeout` qui génèrera un signal SIGINT.

L'algorithme de recommandations fonctionne avec deux valeurs :
- en calculant la distance de chaque film à l'ensemble des films likés en prenant en compte le biais de notation de chaque utilisateur (par exemple un 4 donné par un utilisateur qui met tout le temps des 5 ne vaut pas autant qu'un 4 donné par un utilisateur qui met tout le temps des 3). Plus un film est "proche" des films likés, plus il est recommandé.
- en calculant la note moyenne d'un film, on peut déterminer si il plaira à un grand public ou non

Avec ces deux valeurs, on calcule une valeur de recommandation entre 0 et 1, 0 signifiant que le film n'est pas du tout recommandé.


### Tests

Nous n'avons pas fait de section de tests unitaire directement dans le projet, car aucune fonction ne se prettait vraiment à cette exercice.  
Toutes les fonctions qui font des opérations génériques facilement testables se sont retrouvées dans SA, et elles sont testées si l'on configure xmake avec `xmake f --build_tests=y`.  
Dans le projet, nous avons en revanche choisi de tester la résistance de nos programmes à des arguments incohérents, trop d'arguments, pas assez, des fichiers inexistants, etc...  
Cela se fait grace au fichier `tests/tests.sh` qui peut-être éxecuté depuis n'importe où.

## Quelques données et statistiques

Le programme de parsing des données met environ 30 secondes sur un processeur Ryzen 5 5500u et stockage NVMe M.2. Il utilise environ 3Go de RAM (pas nécessairement à un seul moment).

Le programme de création de statistiques met 5 secondes sur ce même PC. Il utilise environ 785 Mo de RAM.

Le programme d'affichage des statistiques se lance instantanément grâce à une structure de données optimisée.

## A améliorer

Nous avions encore beaucoup d'idées, mais le temps a manqué pour tout implémenter.  
L'un de mes plus gros regrets, c'est de ne pas avoir eu le temps de synchroniser la GUI avec le fichier films_liked.txt. Actuellement il faut entrer les ids des films à la main dans le fichier.  
J'aurais aimé afficher un coeur clickable sur la GUI, pour ajouter ou retirer des films des favoris et pouvoir relancer film_stats depuis la GUI.