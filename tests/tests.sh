#!/bin/bash

# exec_silent_test takes as its first argument a supposed return code for a command and then the command
# then it executes the command and compares its return code with the one passed as argument
exec_silent_test() {
    supposed_error_code="$1"
    shift
    echo -e "Expected return code is \033[35m$supposed_error_code\033[0m for \033[33m$@\033[0m"
    "$@" >/dev/null 2>&1
    real_error_code="$?"
    if [ "$real_error_code" != "$supposed_error_code" ]; then
        echo -e "\033[31mError, program returned $real_error_code\033[0m"
        exit
    fi
    echo -e "\033[32mSuccess\033[0m"
    echo "-----------------------------------------------------"
}

BASEDIR=$(dirname "$0")
pushd $BASEDIR/..
echo "rm -vf ./data/test_*.bin"
rm -vf ./data/test_*.bin

exec_silent_test 3 ./bin/film_gui -i ./data/test_film_stats.bin
exec_silent_test 3 ./bin/film_gui -i ./data/test_film_stats.bin ./download/movie_titles.txt ./download/movie_titles.txt
exec_silent_test 3 ./bin/film_gui -i ./data/test_film_stats.bin -z toto ./download/movie_titles.txt
exec_silent_test 2 ./bin/film_gui -i ./data/test_film_stats.bin ./download/ce_fichier_n_existe_pas.txt

exec_silent_test 3 ./bin/film_stats -i ./data/test_film_data.bin -o ./data/test_film_stats.bin ./download/movie_titles.txt
exec_silent_test 3 ./bin/film_stats -i ./data/test_film_data.bin -o ./data/test_film_stats.bin -z toto
exec_silent_test 2 ./bin/film_stats -i ./data/test_film_data.bin -o ./data/test_film_stats.bin

exec_silent_test 3 ./bin/film_parser -o ./data/test_film_data.bin
exec_silent_test 3 ./bin/film_parser -o ./data/test_film_data.bin -z toto ./download/training_set/mv_0000001.txt
exec_silent_test 2 ./bin/film_parser -o ./data/test_film_data.bin ./download/training_set/mv_0000001.txt ./download/training_set/ce_fichier_nexiste_pas.txt
exec_silent_test 4 timeout -s SIGINT --preserve-status 5 ./bin/film_parser -o ./data/test_film_data.bin ./download/training_set/mv_*.txt
exec_silent_test 0 ./bin/film_parser -o ./data/test_film_data.bin ./download/training_set/mv_00000*.txt

exec_silent_test 0 ./bin/film_stats

echo -ne "\033[34mFor this next test, you will have to close the window once it is opened, press any key to continue\033[0m"
read -n 1 arg

exec_silent_test 0 ./bin/film_gui ./download/movie_titles.txt

echo "-----------------------------------------------------"
echo "Tests with valgrind (on a small dataset)"

exec_silent_test 0 valgrind -q --leak-check=full --error-exitcode=42 ./bin/film_parser -o ./data/test_film_data.bin ./download/training_set/mv_00000*.txt
exec_silent_test 0 valgrind -q --leak-check=full --error-exitcode=42 ./bin/film_stats -i ./data/test_film_data.bin -o ./data/test_film_stats.bin

echo -ne "\033[34mFor this next test, you will have to close the window once it is opened, press any key to continue\033[0m"
read -n 1 arg
exec_silent_test 0 valgrind -q --leak-check=full --error-exitcode=42 ./bin/film_gui -i ./data/test_film_stats.bin ./download/movie_titles.txt

popd
