import sys
from games import *


# TODO: 
# Write code to compute the answers to the questions below and print the answers.
# Feel free to modify this file and the other files as much as you like and need.
# Document your code if you'd like to receive any partial credit.
# Pay special attention to the formatting requirements.

# You probably want to start with importing classes and functions from the games file: from games import ...

def isRowValid(list):
	for strVal in list:
		if(strVal != 'X' and strVal != 'O' and strVal != '-'):
			return False
	return True

rowVal = 3
colVal = 3
kVal = 3
# TODO - Implement. Write code to get the first line from the user. 
# An example input line: X - O
# In this example, the first entry is X, the second entry is -, indicating blank, and the third entry is O
print("Enter the first row.")
firstRow = str(input())

print("Enter the second row.")
secondRow = str(input())

# TODO - Implement
print("Enter the third row.")
thirdRow = str(input())

firstRowList = firstRow.split()
secondRowList = secondRow.split()
thirdRowList = thirdRow.split()

if(len(firstRowList) != 3 or isRowValid(firstRowList) != True):
	print("Invalid input is given for row 1")
	sys.exit()
if(len(secondRowList) != 3 or isRowValid(secondRowList) != True):
	print("Invalid input is given for row 2")
	sys.exit()
if(len(thirdRowList) != 3 or isRowValid(thirdRowList) != True):
	print("Invalid input is given for row 3")
	sys.exit()



listObj = firstRowList + secondRowList + thirdRowList
rowCount = 1
colCount = 1
xMoves = list()
oMoves = list()
for strVal in listObj:
	if(rowCount > rowVal):
		break;
	if(strVal == 'X'):
		xMoves.append((rowCount, colCount))
	elif(strVal == 'O'):
		oMoves.append((rowCount, colCount))

	if(colCount == colVal):
		colCount = 1
		rowCount += 1
	else:
		colCount += 1

ttt = TicTacToe(rowVal, colVal, kVal)
myGameState = ttt.buildState(('X', 'O')[len(xMoves) > len(oMoves)], xMoves, oMoves)

alphaBetaCount = None
utilityCal = ttt.compute_initial_utility(firstRow.split(), secondRow.split(), thirdRow.split())

if(utilityCal == 1 or utilityCal == -1 or (utilityCal == 0 and len(myGameState.moves) == 0)):
	ttt.initial = GameState(to_move=myGameState.to_move, utility=utilityCal, board=myGameState.board, moves=myGameState.moves)
	alphaBetaCount = 1
else:
	utilityCal = ttt.who_wins(myGameState, alphabeta_player)
	minimax_decision(myGameState, ttt)
	(bestMove, alphaBetaCount) = alphabeta_player_count_states(ttt, myGameState)


# For the following questions, you can implement everything here or under the questions or you can do a mix. 
# It's totally up to you where to put your implemention in this file.
# However, please pay special attention to the instructions regarding the format of the answers.
print("Whose turn is it now?")
print(myGameState.to_move)
# TODO - Implement. Compute and print the answer. 
# The answer should a single letter: either X or O. No punctuation. No other text.
print("What is the value of the current state from the perspective of X?")
print(utilityCal)

print("How many states did the minimax algorithm evaluate?")
print(ttt.minMaxCount)
# TODO - Implement. Compute and print the answer. 
# The answer should a single number. No punctuation. No other text.
# You probably need to modify games.py to compute this.

print("How many states did the alpha-beta pruning algorithm evaluate?")
print(alphaBetaCount)
# TODO - Implement. Compute and print the answer.
# The answer should a single number. No punctuation. No other text.
# You probably need to modify games.py to compute this.


print("Assuming both X and O play optimally, does X have a guaranteed win? Is it a tie? Is it a guaranteed loss for X?")
if(utilityCal == 1):
	print("X will win.")
elif(utilityCal == -1):
	print("X will lose.")
else:
	print("It is a tie.")

# TODO - Implement. Compute and print the answer. 
# The answer should be either of "X will win.", or "It is a tie." or "X will lose."
# Note: you already computed this somewhere above.

print("Assuming both X and O would play optimally, how would they play till the game ends?")
ttt.play_game(alphabeta_player, alphabeta_player)

# TODO - Implement. Compute.
# Display the states one at a time, using the display method of the Game class (or its subclasses, whichever is appropriate). 
# Print a single blank line between the states. That is, use print(). Do not print any additional info.