#! /bin/bash

ANSWERS=words/answers-shuffle.txt
GUESSES=words/guesses-shuffle.txt
GUESSES_SHORT=words/guesses-short-shuffle.txt

head -n $1 $GUESSES >$GUESSES_SHORT

if [ $2 ] && [ $2 == "-b" ]; then
	./wordle-solver $ANSWERS $GUESSES_SHORT &>out.txt &
elif [ $2 ] && [ $2 == "-t" ]; then
	./wordle-solver -t $ANSWERS $GUESSES_SHORT
else
	./wordle-solver $ANSWERS $GUESSES_SHORT
fi
