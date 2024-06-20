#! /bin/bash

if [ $1 ]; then
	echo "compiling file: $1"
	gcc -Ofast -c $1
else
	echo -en "compiling files:" *.c "\n"
	gcc -Ofast -c $(ls *.c)
fi

if (($? != 0)); then
	exit 1
fi

if [ ! -d obj_files/ ]; then
	mkdir obj_files/
fi

mv *.o obj_files/

echo -en "linking files:" *.o "--> firstword\n"
gcc -Ofast obj_files/*.o -o firstword
