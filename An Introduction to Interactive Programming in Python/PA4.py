# Implementation of classic arcade game Pong

import simplegui
import random
 
# initialize globals - pos and vel encode vertical info for paddles
WIDTH = 600
HEIGHT = 400       
BALL_RADIUS = 20
PAD_WIDTH = 8
PAD_HEIGHT = 80
HALF_PAD_WIDTH = PAD_WIDTH / 2
HALF_PAD_HEIGHT = PAD_HEIGHT / 2
PADDLE_SPEED = 2
SPEED_MUL = 1.1
LEFT = False
RIGHT = True
ball_pos = [WIDTH / 2, HEIGHT / 2]
ball_vel = [0, 0]
paddle1_pos = HEIGHT / 2
paddle2_pos = HEIGHT / 2
paddle1_vel = 0
paddle2_vel = 0

# helper function
def paddle_pos(pos, vec):
    pos += vec
    if pos < HALF_PAD_HEIGHT:
        pos = HALF_PAD_HEIGHT
    elif pos + HALF_PAD_HEIGHT > HEIGHT:
        pos = HEIGHT - HALF_PAD_HEIGHT
    return pos

def get_quadrilateral(posx, posy):
    return [(posx - HALF_PAD_WIDTH, posy - HALF_PAD_HEIGHT),
            (posx - HALF_PAD_WIDTH, posy + HALF_PAD_HEIGHT),
            (posx + HALF_PAD_WIDTH, posy + HALF_PAD_HEIGHT),
            (posx + HALF_PAD_WIDTH, posy - HALF_PAD_HEIGHT)]

def draw_ball(canvas, pos, vec):
    # pos, vec is a list with 2 elements.
    if pos[1] <= BALL_RADIUS:
        pos[1] = BALL_RADIUS
        vec[1] = -vec[1]
    elif pos[1] >= HEIGHT - BALL_RADIUS:
        pos[1] = HEIGHT - BALL_RADIUS
        vec[1] = -vec[1]
    canvas.draw_circle(pos, BALL_RADIUS, 1, 'White', 'White')

def touch_paddle(paddle_pos, ball_pos):
    top = paddle_pos - HALF_PAD_HEIGHT
    bot = paddle_pos + HALF_PAD_HEIGHT
    return top <= ball_pos <= bot
# initialize ball_pos and ball_vel for new bal in middle of table
# if direction is RIGHT, the ball's velocity is upper right, else upper left
def spawn_ball(direction):
    global ball_pos, ball_vel # these are vectors stored as lists
    ball_pos = [WIDTH / 2, HEIGHT / 2]
    ball_vel = [random.randrange(60, 180), -random.randrange(120, 240)]
    ball_vel[0] /= 60.0
    ball_vel[1] /= 60.0
    if direction == LEFT:
        ball_vel[0] *= -1
    
# define event handlers
def new_game():
    global paddle1_pos, paddle2_pos, paddle1_vel, paddle2_vel  # these are numbers
    global score1, score2  # these are ints
    score2 = score1 = 0
    paddle1_pos = paddle2_pos = HEIGHT / 2
    paddle1_vel = paddle2_vel = 0
    spawn_ball(LEFT)

def draw(canvas):
    global score1, score2, paddle1_pos, paddle2_pos, ball_pos, ball_vel
    # draw mid line and gutters
    canvas.draw_line([WIDTH / 2, 0],[WIDTH / 2, HEIGHT], 1, "White")
    canvas.draw_line([PAD_WIDTH, 0],[PAD_WIDTH, HEIGHT], 1, "White")
    canvas.draw_line([WIDTH - PAD_WIDTH, 0],[WIDTH - PAD_WIDTH, HEIGHT], 1, "White")
    # update ball
    ball_pos[0] += ball_vel[0]
    ball_pos[1] += ball_vel[1]
    # draw ball
    draw_ball(canvas, ball_pos, ball_vel)
    # update paddle's vertical position, keep paddle on the screen
    paddle1_pos = paddle_pos(paddle1_pos, paddle1_vel)
    paddle2_pos = paddle_pos(paddle2_pos, paddle2_vel)
    # draw paddles
    canvas.draw_polygon(get_quadrilateral(HALF_PAD_WIDTH, paddle1_pos), 1, 'White', 'White')
    canvas.draw_polygon(get_quadrilateral(WIDTH - HALF_PAD_WIDTH, paddle2_pos), 1, 'White', 'White')
    # touches/collides detection
    if ball_pos[0] <= PAD_WIDTH + BALL_RADIUS:
        if touch_paddle(paddle1_pos, ball_pos[1]):
            ball_vel[0] *= -SPEED_MUL
            ball_vel[1] *= SPEED_MUL
        else:
            score2 += 1
            spawn_ball(RIGHT)
    elif ball_pos[0] >= WIDTH - PAD_WIDTH - BALL_RADIUS:
        if touch_paddle(paddle2_pos, ball_pos[1]):
            ball_vel[0] *= -SPEED_MUL
            ball_vel[1] *= SPEED_MUL
        else:
            score1 += 1
            spawn_ball(LEFT)
    # draw scores
    canvas.draw_text(str(score1), [WIDTH / 2 - 30, HEIGHT / 4], 30, 'Red')
    canvas.draw_text(str(score2), [WIDTH / 2 + 30, HEIGHT / 4], 30, 'Green')
        
def keydown(key):
    global paddle1_vel, paddle2_vel
    if key == simplegui.KEY_MAP['up']:
        paddle2_vel = -PADDLE_SPEED
    elif key == simplegui.KEY_MAP['down']:
        paddle2_vel = PADDLE_SPEED
    elif key == simplegui.KEY_MAP['w']:
        paddle1_vel = -PADDLE_SPEED
    elif key == simplegui.KEY_MAP['s']:
        paddle1_vel = PADDLE_SPEED
   
def keyup(key):
    global paddle1_vel, paddle2_vel
    if key == simplegui.KEY_MAP['up'] or key == simplegui.KEY_MAP['down']:
        paddle2_vel = 0
    elif key == simplegui.KEY_MAP['w'] or key == simplegui.KEY_MAP['s']:
        paddle1_vel = 0

# create frame
frame = simplegui.create_frame("Pong", WIDTH, HEIGHT)
frame.set_draw_handler(draw)
frame.set_keydown_handler(keydown)
frame.set_keyup_handler(keyup)

# start frame
new_game()
restart_button = frame.add_button('Restart', new_game)
frame.start()

