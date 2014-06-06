# template for "Stopwatch: The Game"
import simplegui
# define global variables
# 0 : RESET, 1 : START, 2 : STOP
state = 0
ticks = 0
correct_guess = 0
total_guess = 0

# define helper function format that converts time
# in tenths of seconds into formatted string A:BC.D
def format(t):
    return '%d:%02d.%d' %(t / 600, t % 600 / 10, t % 10)
    
# define event handlers for buttons; "Start", "Stop", "Reset"
def stop_handler():
    global ticks, state, correct_guess, total_guess
    if state == 2 or state == 0:
        return None
    if ticks % 10 == 0:
        correct_guess += 1
    total_guess += 1
    state = 2

def start_handler():
    global state
    state = 1

def reset_handler():
    global state, correct_guess, total_guess
    correct_guess = 0
    total_guess = 0
    state = 0

# define event handler for timer with 0.1 sec interval
def time_handler():
    global ticks
    global state
    if state == 0:
        ticks = 0
    elif state == 1:
        ticks  += 1
    elif state == 2:
        pass
    else:
        print 'Error: state equal to', state
    print format(ticks)

# define draw handler
def draw_handler(canvas):
    global ticks
    canvas.draw_text(format(ticks), (200, 200), 50, 'Red')
    canvas.draw_text('%d/%d' %(correct_guess, total_guess),
                     (450, 20), 20, 'Green')
    
# create frame
timer = simplegui.create_timer(100, time_handler)
frame = simplegui.create_frame('Timer', 500, 500)

# register event handlers
start_button = frame.add_button('Start', start_handler, 200)
stop_button = frame.add_button('Stop', stop_handler, 200)
reset_button = frame.add_button('Reset', reset_handler, 200)
frame.set_draw_handler(draw_handler)
# start frame
timer.start()
frame.start()

# Please remember to review the grading rubric

