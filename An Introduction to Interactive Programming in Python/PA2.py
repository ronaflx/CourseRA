
# template for "Guess the number" mini-project
# input will come from buttons and an input field
# all output for the game will be printed in the console

import random
import simplegui
import math

# initialize global variables used in your code

correct_answer = -1
chose_range = 100
remaining = 0
# helper function to start and restart the game
def new_game():
    global correct_answer
    global remaining
    if chose_range == 100:
        print 'Game Start with [0, 100)'
        correct_answer = range100()
        remaining = int(math.ceil(math.log(101) / math.log(2)))
    elif chose_range == 1000:
        print 'Game Start with [0, 1000)'
        correct_answer = range1000()
        remaining = int(math.ceil(math.log(1001) / math.log(2)))
    else:
        print 'Error chose_range', chose_range

# define event handlers for control panel
def range100():
    return random.randrange(0, 100)

def range1000():
    return random.randrange(0, 1000)

def chose100():
    global chose_range
    chose_range = 100
    new_game()
    
def chose1000():
    global chose_range
    chose_range = 1000
    new_game()

def input_guess(guess):
    global remaining
    try:
        guess_number = int(guess)
    except ValueError:
        print "Oops!  That was no valid number.  Try again..."
        return
    if guess_number == correct_answer:
        print 'Correct!'
        new_game()
    elif guess_number < correct_answer:
        remaining -= 1
        print 'Higher,', remaining, 'time(s) to guess'
    else:
        remaining -= 1
        print 'Lower,', remaining, 'time(s) to guess'
    if remaining == 0:
        print 'Restart game'
        new_game()
# create frame

frame = simplegui.create_frame("Guess Game", 400, 400)

# register event handlers for control elements
guess_input = frame.add_input("Guess Input", input_guess, 100)
restart_button = frame.add_button("Range: 0 - 100", chose100, 100)
restart_button = frame.add_button("Range: 0 - 1000", chose1000, 100)

# call new_game and start frame
new_game()
frame.start()


#
