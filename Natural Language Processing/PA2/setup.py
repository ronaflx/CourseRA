import json
import sys
import os
from collections import Counter

def get_tree(data):
    for i in data:
        yield json.loads(i)

def get_word(counter, tree):
    if type(tree) != type([]):
        counter[tree] += 1
    elif len(tree) == 2:
        get_word(counter, tree[1])
    elif len(tree) == 3:
        get_word(counter, tree[1])
        get_word(counter, tree[2])

def get_counter(data):
    counter = Counter()
    for tree in get_tree(data):
        get_word(counter, tree)
    return counter
def replace(counter, tree):
    if type(tree) != type([]):
        if counter[tree] < 5:
            return '_RARE_'
        else:
            return tree
    elif len(tree) == 2:
        return [tree[0], replace(counter, tree[1])]
    elif len(tree) == 3:
        return [tree[0], replace(counter, tree[1]), replace(counter, tree[2])]

def write_tree(filestream, data, counter):
    for tree in get_tree(data):
        new_tree = replace(counter, tree)
        filestream.write(json.dumps(new_tree) + '\n')
    filestream.close()

if __name__ == "__main__":
    train_path_old = 'parse_train.dat'
    train_path_new = 'parse_train.dat.new'
    train_count = 'parse_train.counts.out'

    try:
        train_in  = [data.strip() for data in open(train_path_old, "r")]
        train_out = open(train_path_new, "w")
        counter = get_counter(train_in)
        write_tree(train_out, train_in, counter)
        os.system("python count_cfg_freq.py %s > %s" %(train_path_new, train_count))
    except IOError:
        sys.stderr.write("ERROR: Cannot read inputfile %s.\n" % arg)
        sys.exit(1)
    finally:
        os.system("rm %s" %(train_path_new))

