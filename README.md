# Netflix Prize

## Sommaire
- [Netflix Prize](#netflix-prize)
  - [Sommaire](#sommaire)
  - [Important](#important)
  - [What is this ?](#what-is-this-)
  - [How to use it ?](#how-to-use-it-)
    - [films\_liked.txt](#films_likedtxt)
  - [Binary structures used](#binary-structures-used)
  - [Details and implementation choices](#details-and-implementation-choices)
    - [SystemAbstraction](#systemabstraction)
    - [File structures](#file-structures)
    - [Command line](#command-line)
    - [Tests](#tests)
  - [Some data and statistics](#some-data-and-statistics)
  - [To improve](#to-improve)

## Important
As with any project, we had a deadline so we had to stop somewhere. The code is questionable, all features aren't implemented, but everything works well.

## What is this ?
A project that provides movie recommendations from a database of ratings by Netflix users. It is made up of three programs:

- <u>film_parser</u>: Takes all user reviews as input and organizes them into a much smaller, faster-to-read binary data file.
- <u>film_stats</u>: takes as input the file of titles and years of films, filter options on reviews and the binary data file and calculates statistics and recommendations for each film based on a specified list of liked movies. It then writes the statistics to a binary stats file.
- <u>film_gui</u>: takes the binary stats file as input and displays the recommendations in descending order and statistics in a graphical interface. You can then search by name.

## How to use it ?

The first step is to install [xmake](https://xmake.io). Instructions are available [here](https://xmake.io/#/guide/installation).

Then you need to install SystemAbstraction, via the beta branch of the git repo, then compile and install it.
```
git clone https://github.com/mactul/system_abstraction -b beta
cd system_abstraction
xmake
xmake install
```

Then you can take care of the project

```
git clone https://github.com/mactul/tps_1year_project.git
cd tps_1year_project
xmake
```

To force xmake to recompile everything, you can call `xmake -r`

A doxygen documentation is provided, to generate it, you can call `xmake docs` or `doxygen` in the current folder. The documentation files will then be located in the `doc/html/` folder.

Then you must download the [Netflix Prize Dataset](https://academictorrents.com/details/9b13183dc4d60676b773c9e2cd6de5e5542cee9a) using a torrent client.  
We will assume that you extract the tar.gz file in `tps_1year_project/download/`.

To get a list of recommendations with the provided dataset, you can call:

First of all:
```
./bin/film_parser ./download/training_set/mv_*.txt
```

Then:
```
./bin/film_stats
```
or better:
```
./bin/film_stats -r ./data/films_liked.txt
```
to get recommendations based on the movies you like

Finally:
```
./bin/film_gui download/movie_titles.txt
```

### films_liked.txt

When using film_stats with the -r option, you must provide a text file.
This file is formatted with one movie ID per line.
The example provided in data/film_liked.txt contains the IDs of Nemo and Toys Story.
The films returned first in the graphical interface will therefore be films close to these Pixars.

## Binary structures used

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

## Details and implementation choices

### SystemAbstraction

This project led to the creation of a lot of functions, which could have simply been put in the project but are intended to be used elsewhere as well.
We have chosen to include them in the [SystemAbstraction](https://github.com/mactul/system_abstraction/tree/beta) library (abbreviated as SA), whose commits over the last few months are exclusively dedicated to providing functions for this project.
We have created a `beta` branch on this repository, this branch will contain the version of the library at the end of the project on 01/19/2024 (with the possible exception of the documentation which could evolve), the other branches may no longer be compatible because they are older or have undergone breaking changes.

The project GUI is based on the graphics overlay provided by SA.
SA was created to plug into any graphics library, but for now it uses X11, which is the native way to communicate with the window manager in Linux, however this protocol is old and outdated, this which brought us many problems.
One that we haven't been able to resolve at the moment is that one of X11's functions (XLookupString) is poorly designed and does not free the memory it allocates when closing the program.
Valgrind therefore indicates that blocks are unfreed but this does not come directly from our code...


### File structures

The structure of the data.bin file was chosen because it is almost identical to the data stored in RAM but instead of pointers, we note the position of the cursor in the file.

The Rating structure is designed to fit over 8 bytes. To keep the year in memory on only 1 byte, we kept the number of years since 1850 (YEARS_OFFSET). Each of these structures in our binary file is then aligned over 8 bytes.

Likewise the structure of the stats.bin file is a direct representation of RAM, so it can be read and written VERY quickly.

### Command line

Each program has help, which can be displayed using `./bin/[program] -h`.

Some command line options have been added or removed depending on what we found more convenient.
For example the `-f FOLDER` option was replaced each time by `-o OUTPUT_FILE_PATH`, the `-s FILM_ID` option was removed because you can search by film name in the graphical interface, Liked films are passed into a file using the `-r LIKED_FILMS_FILE_PATH` option, the `-n LIMIT` option does not exist because all suggestions are displayed in the graphical interface, and the `-t TIMEOUT option ` is not implemented because we can simply launch our program with the `timeout` tool which will generate a SIGINT signal.

The recommendation algorithm works with two values:
- by calculating the distance of each film from all liked films taking into account the rating bias of each user (for example a 4 given by a user who always gives 5s is not worth as much as a 4 given by a user who always puts 3). The “closer” a film is to the liked films, the more it is recommended.
- by calculating the average rating of a film, we can determine whether it will appeal to a general audience or not

With these two values, we calculate a recommendation value between 0 and 1, 0 meaning that the film is not recommended at all.


### Tests

We did not do a unit testing section directly in the project, because no function was really suitable for this exercise.
All the functions that do generic operations that are easily testable are found in SA, and they are tested if you configure xmake with `xmake f --build_tests=y`.
In the project, however, we chose to test the resistance of our programs to inconsistent arguments, too many arguments, not enough, non-existent files, etc...
This is done using the `tests/tests.sh` file which can be executed from anywhere.

## Some data and statistics

The data analysis program takes approximately 30 seconds on a Ryzen 5 5500u processor and NVMe M.2 storage. It uses around 3GB of RAM (not necessarily at a single time).

The statistics creation program takes 5 seconds on this same PC. It uses around 785MB of RAM.

The statistics display program launches instantly thanks to an optimized data structure.

## To improve

We still had a lot of ideas, but there was not enough time to implement everything.
One of my biggest regrets is not having had time to synchronize the GUI with the films_liked.txt file. Currently you have to enter the movie IDs by hand in the file.
I would have liked to display a clickable heart on the GUI, to add or remove films from favorites and to be able to restart film_stats from the GUI.
​