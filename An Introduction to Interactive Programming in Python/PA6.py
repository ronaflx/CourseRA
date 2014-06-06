# Mini-project #6 - Blackjack

import simplegui
import random

# load card sprite - 949x392 - source: jfitz.com
CARD_SIZE = (73, 98)
CARD_CENTER = (36.5, 49)
card_images = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/cards.jfitz.png")

CARD_BACK_SIZE = (71, 96)
CARD_BACK_CENTER = (35.5, 48)
card_back = simplegui.load_image("http://commondatastorage.googleapis.com/codeskulptor-assets/card_back.png")    

# initialize some useful global variables
in_play = False
outcome = ""
score = 0

# define globals for cards
SUITS = ('C', 'S', 'H', 'D')
RANKS = ('A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K')
VALUES = {'A':1, '2':2, '3':3, '4':4, '5':5, '6':6, '7':7, '8':8, '9':9, 'T':10, 'J':10, 'Q':10, 'K':10}


# define card class
class Card:
    def __init__(self, suit, rank):
        if (suit in SUITS) and (rank in RANKS):
            self.suit = suit
            self.rank = rank
        else:
            self.suit = None
            self.rank = None
            print "Invalid card: ", suit, rank

    def __str__(self):
        return self.suit + self.rank

    def get_suit(self):
        return self.suit

    def get_rank(self):
        return self.rank

    def draw(self, canvas, pos):
        card_loc = (CARD_CENTER[0] + CARD_SIZE[0] * RANKS.index(self.rank), 
                    CARD_CENTER[1] + CARD_SIZE[1] * SUITS.index(self.suit))
        canvas.draw_image(card_images, card_loc, CARD_SIZE, [pos[0] + CARD_CENTER[0], pos[1] + CARD_CENTER[1]], CARD_SIZE)
        
# define hand class
class Hand:
    def __init__(self):
        self.cards = []

    def __str__(self):
        return 'Hand contains ' + ' '.join(map(str, self.cards))

    def add_card(self, card):
        if card:
            self.cards.append(card)

    def get_value(self):
        value = 0
        num_ace = 0
        for card in self.cards:
            rank = card.get_rank()
            if rank == 'A':
                num_ace += 1
            value += VALUES[rank]
        for i in range(num_ace):
            if value + 10 > 21:
                break
            value += 10
        return value
    
    def draw(self, canvas, pos):
        i = 0
        for card in self.cards:
            card.draw(canvas, (pos[0] + i * CARD_SIZE[0], pos[1]))
            i += 1
        
# define deck class 
class Deck:
    def __init__(self):
        self.cards = [Card(suit, rank) for suit in SUITS for rank in RANKS]

    def shuffle(self):
        random.shuffle(self.cards)

    def deal_card(self):
        if self.cards:
            idx = random.randrange(len(self.cards))
            return self.cards.pop(idx)
    
    def __str__(self):
        return 'Deck contains ' + ' '.join(map(str, self.cards))

deck = Deck()
player = Hand()
dealer = Hand()
#define event handlers for buttons
def deal():
    global outcome, in_play, score

    # your code goes here
    global deck, player, dealer
    deck = Deck()
    deck.shuffle()
    
    player = Hand()
    player.add_card(deck.deal_card())
    
    dealer = Hand()
    dealer.add_card(deck.deal_card())
    if in_play:
        score -= 1
    in_play = True
    outcome = 'Hit or stand?'

def hit():
    global in_play, outcome, score
    global player, deck
 
    # if the hand is in play, hit the player
    if in_play and player.get_value() <= 21:
        player.add_card(deck.deal_card())
    # if busted, assign a message to outcome, update in_play and score
    if in_play and player.get_value() > 21:
        outcome = 'You have busted. New deal?'
        in_play = False
        score -= 1
        
    print 'Player:', player, player.get_value()

def stand():
    global dealer, deck, player
    global in_play, score, outcome
    # if hand is in play, repeatedly hit dealer until his hand has value 17 or more
    # assign a message to outcome, update in_play and score
    if in_play:
        while dealer.get_value() < 17:
            dealer.add_card(deck.deal_card())
        print 'Player:', player, player.get_value()
        print 'Dealer:', dealer, dealer.get_value()
        if dealer.get_value() > 21:
            outcome = 'Dealer busted. New deal?'
            score += 1
        elif dealer.get_value() >= player.get_value():
            outcome = 'Dealer win. New deal?'
            score -= 1
        else:
            outcome = 'You win. New deal?'
            score += 1
        in_play = False

# draw handler    
def draw(canvas):
    # draw title
    canvas.draw_text('Blackjack', (250, 50), 36, 'Red')
    
    # draw hands
    global player, dealer
    player.draw(canvas, (0, 100))
    dealer.draw(canvas, (0, 100 + CARD_SIZE[1]))
    global outcome, score, in_play
    # player's hole card
    if in_play:
        card_loc = (CARD_BACK_CENTER[0], CARD_BACK_CENTER[1])
        canvas.draw_image(card_back, card_loc, CARD_BACK_SIZE, [CARD_BACK_CENTER[0], CARD_BACK_CENTER[1] + 100], CARD_BACK_SIZE)
    # dealer's hole card
    if in_play:
        card_loc = (CARD_BACK_CENTER[0], CARD_BACK_CENTER[1])
        canvas.draw_image(card_back, card_loc, CARD_BACK_SIZE, [CARD_BACK_CENTER[0], CARD_BACK_CENTER[1] + 100 + CARD_SIZE[1]], CARD_BACK_SIZE)
    
    # draw information
    canvas.draw_text(outcome, (100, 350), 36, 'Red')
    canvas.draw_text(str(score), (300, 400), 36, 'Red')


# initialization frame
frame = simplegui.create_frame("Blackjack", 600, 600)
frame.set_canvas_background("Green")

#create buttons and canvas callback
frame.add_button("Deal", deal, 200)
frame.add_button("Hit",  hit, 200)
frame.add_button("Stand", stand, 200)
frame.set_draw_handler(draw)


# get things rolling
deal()
frame.start()


# remember to review the gradic rubric
