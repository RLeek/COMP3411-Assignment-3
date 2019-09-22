# COMP3411-Assignment-3
COMP3411, Semester 1, Assignment 3
Ricky Liu z5207969

Agent.c is the code for the agent we wrote for playing a modified version of 
ultimate tic-tac-toe. This modified version works the same as normal ultimate
tic-tac-toe, except the first player to win one of the nine boards, wins the
game overall.

To see a game of ultimate tic-tac-toe use ./playcs, which contains instructions
on how to set up a game. ./playcs2 is a custom shell script that allows for 
playing 100 games, and then counts the number of wins and losses.

Questions 1:
Briefly describe how your program works, including any algorithms and data 
structures employed, and explain any design decisions you made along the way.

The agent below uses a modified version of alpha-beta search. The main
modification is that this alpha-beta search also accounts for average path 
cost, instead of just the end-result. This is because if two final nodes have
the same heurisitic value, our search would naturally choose the move that 
was generated first, however this is not necessarily optimal given that the
second final node has a higher average path cost. The higher average path 
cost means that the second final node will likely have better 'alternative'
moves given that the opponent AI plays differently to how we presume our 
oppoent plays in our alpha-beta search. In 100 games, by accounting for 
average path cost, we win approximately 5-10 more games. Our evaluation 
heuristic ratios (i.e. How we weigh our heuristic values), have largely been 
determined through choosing a value, running 100 games to compute the win
percentage against ./lookt and then choosing a different value until we have
maximized our wins. 

Our alpha-beta search starts with determining if there is a winning move.
If there is none, then we generate our moves in the order of the board. 
We then evaluate each corresponding board we generate using our move. The 
evaluation heuristic we use is the one used for a normal tic-tac toe board 
minus the evaluation of the next board (based on our move) for the opponent. 
The ratio is 1 to 0.6 respectively. We then add this value to a 'path cost' 
variable. We then repeat this for the opponent, checking for a win and 
then evaluating the board for the opponent minus evaluating the next board 
for us. We then minus this from our path cost (since we want to maximise 
our path cost, not the opponent). This process is repeated until the 
terminal node. Here we use a different evaluation heurisitic. This 
heuristic essentially searches through each board and determines whether 
we have a winning move or our opponent has a winning move and then 
adds/subtracts a given value. Our path cost is then divided by the depth 
to compute an average which is added to this terminal node heuristic, which 
we then return. The final move we choose is the move that returns the best\
alpha value first.

The struct we use to present the ultimate board is an array of smaller 
boards in the form of a 9-length array. This was chosen because it made it 
simple to isolate and evaluate individual boards.
