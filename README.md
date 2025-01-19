# Best First Guess Finder for Wordle

This project is a C program designed to find the best first guess for the game **Wordle** by analyzing word combinations and their eliminations. It calculates which guesses can eliminate the most possible answers when paired together, determining the optimal starting word(s).

## How It Works

The program performs the following steps:
1. It takes two word lists as input:
    - A list of valid guesses.
    - A list of possible answers.
2. For every combination of two valid guesses, the program analyzes them alongside each potential answer. It creates a data structure representing the information that a player would deduce based on the feedback he would receive from these guesses. This data structure contains:
    - Letters known to be in the answer.
    - Letters known not to be in the answer.  
    For each letter known to be in the answer, the data structure stores four additional pieces of information:
    - The minimum number of occurrences of that letter.
    - Whether the number of occurrences is capped (indicating all occurrences of that letter have been discovered).
    - Positions the letter is known to be in.
    - Positions the letter is known not to be in.
3. To improve efficiency, the program first consults a binary search tree which stores the elimination counts for data structures that have already been generated. If a matching data structure is found in the tree, the corresponding elimination count is retrieved.
4. If the data structure is not found in the tree, the program then counts eliminations by comparing this data structure with the data of each individual answer. It determines whether a word can be eliminated based on whether the information deduced from the two guesses complies with it. The result is then stored in the tree for future use.
3. Each individual word accumulates a total score, representing how many answers it helps to eliminate. Once the number of potential answers eliminated is determined for a combination of guesses, it is added to the total for each of the guesses in the pair.
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
./wordle-solver words/answers.txt words/guesses.txt
```

As the program runs, it will display progress in the terminal. Once completed, it will output the best guesses based on the analysis.

## Word Lists

- Valid Guesses: This file should contain a list of all the words that are allowed as guesses in Wordle.
- Possible Answers: This file should contain the list of words that could be the solution in Wordle.

## Output

When the program finishes, it lists the word or words that are determined to be the best first guesses based on the number of answers they help eliminate.
