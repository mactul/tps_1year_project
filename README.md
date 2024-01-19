# Netflix Prize - Macéo Tuloup, Valentin Foulon

## Sommaire
- [Sommaire](#sommaire)
- [Qu'est-ce que c'est ?](#quest-ce-que-cest)
- [Comment l'utiliser ?](#comment-lutiliser)
- [Structures binaires utilisées](#structures-binaires-utilisées)
- [Détails et choix d'implémentation](#détails-et-choix-dimplémentation)
- [Quelques données](#quelques-données-et-statistiques)

## Qu'est-ce que c'est ?
Un projet qui fournit des recommandations de films à partir d'une base de données de notes par des utilisateurs de Netflix. Il est composé de trois programmes :

- <u>film_parser</u> : prend en entrée tous les avis des utilisateurs et les organise dans un fichier binaire de données beaucoup plus petit et rapide à lire.
- <u>film_stats</u> : prend en entrée le fichier des titres et années des films, des options de filtres sur les avis et le fichier binaire de données et calcule des statistiques et les recommandations pour chaque film en fonction d'une liste spécifiée de films aimés. Il écrit ensuite les statistiques dans un fichier binaire de stats.
- <u>film_gui</u> : prend en entrée le fichier binaire de stats et affiche les recommandations par ordre décroissant et statistiques dans une interface graphique. On peut ensuite effectuer une recherche par nom.

## Comment l'utiliser ?

La première étape est d'installer [xmake](https://xmake.io). Les instructions sont disponibles [ici](https://xmake.io/#/guide/installation).

Ensuite il faut installer SystemAbstraction, la bibliothèque d'abstraction de Macéo, via la branche dev du repo git, puis le compiler et l'installer
```
git clone https://github.com/mactul/system_abstraction -b dev
cd system_abstraction
xmake f -m debug --build_tests=n --build_graphics=y --graphics_renderer=x11
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

Une documentation doxygen est fournie, pour la générer, vous pouvez appeler `xmake doc` ou alors `doxygen` dans le dossier courant. Les fichiers de documentation se situeront alors dans le dossier `doc/html/`

Pour obtenir une liste de recommandations avec le jeu de données fourni, vous pouvez appeler dans l'ordre

```
./bin/film_parser download/training_set/mv_*.txt
```

```
./bin/film_stats download/movie_titles.txt
```

```
./bin/film_gui download/movie_titles.txt
```

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
<pre>
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
</pre>

## Détails et choix d'implémentation

Le programme d'interface graphique utilise X11, c'est horrible mais ça fonctionne presque parfaitement, excepté pour la fonction XLookupString qui provoque une légère fuite de mémoire. Cela permet d'avoir une exécution rapide car on ne passe pas par une surcouche d'affichage.

La structure du fichier data.bin a été choisie car elle est quasiment identique aux données stockées en RAM mais à la place de pointeurs, on note la position du curseur dans le fichier.

La structure Rating est faite pour tenir sur 8 octets mais pour garder en mémoire l'année sur 1 octet, nous avons uniquement pu garder le nombre d'années depuis 1850 (YEARS_OFFSET). Chacune de ces structures dans notre fichier binaire est alors alignée sur 8 octets.

De même la structure du fichier stats.bin est une représentation directe de la RAM, elle peut donc être lue et écrite TRÈS rapidement.

Certaines options de ligne de commande ont été rajoutées ou supprimées en fonction de ce qui nous paraissait plus pratique.
Par exemple l'option `-f FOLDER` a été remplacé à chaque fois par `-o OUTPUT_FILE_PATH`, l'option `-s FILM_ID` a été supprimée car on peut effectuer une recherche par nom de film dans l'interface graphique, les films likés sont passés dans un fichier par l'option `-r LIKED_FILMS_FILE_PATH`, l'option `-n LIMIT` n'existe pas car toutes les suggestions sont affichées dans l'interface graphique, et l'option `-t TIMEOUT` n'est pas implémentée car on peut simplement lancer notre programme avec l'outil `timeout` qui génèrera un signal SIGINT.

L'algorithme de recommandations fonctionne en 

## Quelques données et statistiques

Le programme de parsing des données met environ 30 secondes sur un processeur Ryzen 5 5500u et stockage NVMe M.2. Il utilise environ 3Go de RAM (pas nécessairement à un seul moment).

Le programme de création de statistiques met 5 secondes sur ce même PC. Il utilise environ 785 Mo de RAM.

Le programme d'affichage des statistiques se lance instantanément grâce à une structure de données optimisée.