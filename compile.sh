#! /bin/bash

FLAGS="-Ofast -Wall"

if [ $1 ]; then
	echo "compiling file: $1"
	gcc $FLAGS -c $1
else
	echo -en "compiling files:" *.c "\n"
	gcc $FLAGS -c $(ls *.c)
fi

if (($? != 0)); then
	rm -v *.o
	exit 1
fi

if [ ! -d obj_files/ ]; then
	mkdir obj_files/
fi

mv *.o obj_files/

echo -en "linking files:" *.o "--> firstword\n"
gcc $FLAGS obj_files/*.o -o firstword
