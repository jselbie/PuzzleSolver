# PuzzleSolver

December 13, 2017 · 
I've had this puzzle for 20 years. Left unsolved until now. The idea is that you have 16 tiles, with a hieroglyphic on each side. Arrange the tiles in a 4x4 grid such that each glyph matches the glyph on the adjacent tile. After solving it, the center glyph on each tile maps to a password on a website that gives you additional puzzles.

Manually solving this proved difficult. I'm sure my friends here on FB that love these types of puzzles will think this is lame, but I tried different ways of writing code to solve it with code. With 22x10²² possible solutions, a brute force algorithm is unlikely to find anything in my lifetime. Years ago I tried a genetic algorithm, but it didn't do any better than random sampling of permutations. Then over the holidays, I resurrected this game. I had an epiphany for an algorithm involving divide and conquer, a hash table, and perhaps a little dynamic programming.  Coded it up, flushed out the bugs, and the solutions were on the screen after a few seconds. That's right, multiple solutions. The inventor of the puzzle might not have realized that some of the tiles can swap positions and still form a valid solution. (24 unique solutions in all).

No problem, I'll just go to the website and try all of them.... It turns out, the website is now defunct and offline. So, we'll never know...


![alt text](/internetpuzzle1.png "Internet Puzzle Box")

![alt text](/internetpuzzle2.png "Internet Puzzle Solution")
