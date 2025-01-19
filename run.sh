#! /bin/bash

ANSWERS=words/answers-shuffle.txt
GUESSES=words/guesses-shuffle.txt

if [ $1 ] && [ $1 == "-b" ]; then
	./wordle-solver $ANSWERS $GUESSES &>out.txt &
elif [ $1 ] && [ $1 == "-t" ]; then
	./wordle-solver -t $ANSWERS $GUESSES
else
	./wordle-solver $ANSWERS $GUESSES
fi
