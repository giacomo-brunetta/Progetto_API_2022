# Final Project for API class in Politecnico di Milano 2022

### Topics of Exam:
* Algorithms,
* data structures,
* automata theory
### Mark: 
30/30
### Author:
[Giacomo Brunetta](https://github.com/giacomo-brunetta)

### Description:
The following code handles a game in which the user has to guess a word. The program must handle a dictionary
of words, tell the user which letter in the guess are correct, which one are wrong and which one are in the wrong position.
Every turn the program should keep count of the strings that are compatible with the suggestions given to the user and
be able to print those strings in alphabetical order.
Strings can be added to the dictionary at the state of every turn.
The dictionary is implemented as a **red black tree** and the constraints on the strings are marked on **hash tables**.
Hash tables of boolean are implemented as long ints, while hash tables of integers are implemented as vectors.
