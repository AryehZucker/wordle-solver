#! /bin/bash

ANSWERS=words/answers-shuffle.txt
GUESSES=words/guesses-shuffle.txt

if [ $1 ]; then
	GUESSES_SHORT=words/guesses-short-shuffle.txt
	head -n $1 $GUESSES >$GUESSES_SHORT
	GUESSES=$GUESSES_SHORT
fi

./wordle-solver $ANSWERS $GUESSES
