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
The following code handles a game where the user must guess a word. The program must take a dictionary
of words, tell the user which letters in the guess are correct, which ones are wrong, and which ones are in the wrong position.
At every turn, the program should keep count of the strings that are compatible with the suggestions given to the user and
be able to print those strings in alphabetical order.
Strings can be added to the dictionary at the state of every turn.
The dictionary is implemented as a **red black tree** and the constraints on the strings are marked on **hash tables**.
Hash tables of boolean are implemented as long ints, while hash tables of integers are implemented as vectors.

### What have I learned:
This project was the first of the 3 final projects for my bachelor's in computer science and engineering.
This was the first 'big' code assignment I worked on and I have to say that I have learned a lot from it, especially I learned how things should not be done.

This project was valued on performance only, so I did not pay attention at all to code quality and readability. Looking back I have to say it was a pretty terrible decision. If I had to rewrite it now I would:
- Avoid having all those side effects and global variables
- Create a clear state pattern instead of changing global int variables to determine the state
- Keep the code DRY by writing smaller functions
- Do not use all that recursion when performance is the main goal

Anyway, the code performed well and was marked 30/30, so the goal of achieving performance was met.
In particular, I think I have achieved a good result thanks to the implementation of the RBT with the additional flag that marks 'dead branches' and I managed to save a lot of space using bitwise operations (even if I took it too far despite code clarity).

In the making of this project, I learned:
- How to implement a Red-Black Tree in C from scratch
- Bit-wise operations in C
- string.h library
- stdint.h libray
