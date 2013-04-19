import bisect
import sys

def get_word_list(language) :
    words = set()
    for sentence in language:
        words |= set(sentence)
    words_list = sorted([word for word in words])
    return words_list

def initial(english_words, foreign_words, english, foreign, null):

    # condition[(f, e)] = t(f | e) probability of f given e
    condition = dict()
    e_size = len(english_words)
    f_size = len(foreign_words)

    # word_set[ith] contain the foreign word in the same corpus with ith word 
    word_set = [set() for i in range(e_size)]

    for e_sentence, f_sentence in zip(english, foreign):
        for e in e_sentence:
            word_set[e] |= set(f_sentence)

    for e in range(e_size):
        l = len(word_set[e])
        for f in word_set[e]:
            condition[(f, e)] = 1.0 / l
    return condition

# tranfrom string to integer label
def str2int(word_list, language):
    return [[bisect.bisect_left(word_list, word) for word in sentence] for sentence in language]

def EM_algo_IBM1(english, foreign, times = 5):
    # initialization
    english_words = get_word_list(english)
    foreign_words = get_word_list(foreign)

    english = str2int(english_words, english)
    foreign = str2int(foreign_words, foreign)

    e_size = len(english_words)
    f_size = len(foreign_words)
    null   = bisect.bisect_left(english_words, '_NULL_')

    condition = initial(english_words, foreign_words, english, foreign, null)

    # run times(= 5) iteration
    for t in range(times):

        # c_counter[(i,j)] the number of ith foreign_word align to jth english_words
        # e_counter[i]    the number of ith english_words
        c_counter = dict()
        e_counter = [0.0] * e_size

        for e_sentence, f_sentence in zip(english, foreign):
            for f in f_sentence:

                # sigma(t(f | e)) for e in english sentence
                sigma   = 0.0
                for e in e_sentence:
                    sigma  += condition[(f, e)]
                for e in e_sentence:
                    pair    = (f, e)
                    __sigma = condition[pair] / sigma
                    c_counter.setdefault(pair, 0.0)
                    e_counter[e]    += __sigma
                    c_counter[pair] += __sigma

        # recalculate t(f | e)
        for key in condition:
            condition[key] = c_counter[key] / e_counter[key[1]]

    return english_words, foreign_words, condition

if __name__ == '__main__':

    # read training set
    try:
        train_english_file = open('corpus.en', 'r')
        train_foreign_file = open('corpus.es', 'r')
        english = [['_NULL_'] + data.strip().split() for data in train_english_file]
        foreign = [data.strip().split() for data in train_foreign_file]
    except IOError:
        sys.stderr.write('ERROR : Cannot read input file\n')
        sys.exit(1)
    finally:
        train_english_file.close()
        train_foreign_file.close()

    english_words, foreign_words, condition = EM_algo_IBM1(english, foreign)
    e_size = len(english_words)
    f_size = len(foreign_words)

    # translate
    try:
        dev_english_file = open('test.en', 'r')
        dev_foreign_file = open('test.es', 'r')
        dev_key_file = open('alignment_test.p1.out', 'w')

        english = str2int(english_words, [data.strip().split() for data in dev_english_file])
        foreign = str2int(foreign_words, [data.strip().split() for data in dev_foreign_file])
        null = bisect.bisect_left(english_words, '_NULL_')

        id = 0
        for e_sentence, f_sentence in zip(english, foreign):
            id += 1
            for i, f in enumerate(f_sentence):
                align = 0
                v = condition[(f, null)]
                for argv, e in enumerate(e_sentence):
                    pair = (f, e)
                    if condition[pair] > v:
                        v = condition[pair]
                        align = argv + 1
                if align != 0:
                    dev_key_file.write('%d %d %d\n' %(id, align, i + 1))
    except IOError:
        sys.stderr.write('ERROR : Cannot read dev file\n')
        sys.exit(1)
    finally:
        dev_english_file.close()
        dev_foreign_file.close()
        dev_key_file.close()
