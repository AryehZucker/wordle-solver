# Best First Guess Finder for Wordle

This project is a C program designed to find the best first guess for the game Wordle by analyzing word combinations and their eliminations. It calculates which guesses can eliminate the most possible answers when paired together, determining the optimal starting word(s).

## How It Works

The program performs the following steps:
1. It takes two word lists as input:
  - A list of valid guesses.
  - A list of possible answers.
2. For every combination of two valid guesses, the program counts how many possible answers, on average, that combination would eliminate together.
3. Each individual word accumulates a total score, representing how many answers it helps to eliminate.
4. Finally, the program compares the totals of all valid words and outputs the word(s) with the highest elimination potential as the best first guess.

## Requirements

There are no external dependencies or libraries required to run this program.

## Compilation

The project includes a `Makefile` for easy compilation. To compile the project, simply run:

```bash
make
```

This will generate the executable needed to run the program.

## Usage

Once compiled, the program can be executed from the command line. You must provide the paths to two word lists: one for valid guesses and one for possible answers.

Example usage:
```bash
./firstword <path_to_possible_answers> <path_to_valid_guesses>
```

As the program runs, it will display progress in the terminal. Once completed, it will output the best guesses based on the analysis.

## Word Lists

- Valid Guesses: This file should contain a list of all the words that are allowed as guesses in Wordle.
- Possible Answers: This file should contain the list of words that could be the solution in Wordle.

## Output

When the program finishes, it lists the word or words that are determined to be the best first guesses based on the number of answers they help eliminate.
