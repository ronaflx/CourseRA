# implementation of card game - Memory

import simplegui
import random

# helper function to initialize globals
KCARDS = 16
num_of_cards = []
# 0: unflipped, 1: flipped in this turn, 2: flipped
state_of_cards = []
turns = 0
def new_game():
    global turns, state_of_cards, num_of_cards
    turns = 0
    state_of_cards = [0] * KCARDS
    num_of_cards = range(0, 8) * 2
    state_of_cards = [0] * KCARDS
    random.shuffle(num_of_cards)

     
# define event handlers
def mouseclick(pos):
    card_id = pos[0] / 50
    global state_of_cards, num_of_cards, turns
    state = state_of_cards[card_id]
    if state == 0:
        num_of_ones = state_of_cards.count(1)
        if num_of_ones < 2:
            state_of_cards[card_id] = 1
        else:
            idx0 = state_of_cards.index(1)
            idx1 = state_of_cards.index(1, idx0 + 1)
            if num_of_cards[idx0] == num_of_cards[idx1]:
                state_of_cards[idx0] = 2
                state_of_cards[idx1] = 2
            else:
                state_of_cards[idx0] = 0
                state_of_cards[idx1] = 0
            state_of_cards[card_id] = 1
        turns += 1
    
                        
# cards are logically 50x100 pixels in size    
def draw(canvas):
    for i in range(KCARDS):
        pts = [(i * 50, 0), ((i + 1) * 50, 0),
               ((i + 1) * 50, 100), (i * 50, 100)]
        if state_of_cards[i] == 0:
            canvas.draw_polygon(pts, 1, 'Black', 'Green')
        else:
            canvas.draw_polygon(pts, 1, 'Green', 'Black')
            canvas.draw_text(str(num_of_cards[i]), (50 * i, 90), 100, 'Red')
    global label, turns
    label.set_text('Turns = %d' %(turns / 2))

# create frame and add a button and labels
frame = simplegui.create_frame("Memory", 800, 100)
frame.add_button("Reset", new_game)
label = frame.add_label("Turns = 0")

# register event handlers
frame.set_mouseclick_handler(mouseclick)
frame.set_draw_handler(draw)

# get things rolling
new_game()
frame.start()


# Always remember to review the grading rubric
