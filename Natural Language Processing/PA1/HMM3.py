import sys
import os
import counter

def replace(train_data, output):
    word_count = dict()
    for raw_data in train_data:
        if raw_data == '\n' : 
            continue
        word, tags = raw_data.split()
        if word_count.get(word, 0) == 0:
            word_count[word] = 1
        else :
            word_count[word] += 1
    for raw_data in train_data:
        if raw_data == '\n':
            output.write('\n')
            continue
        word, tags = raw_data.split()
        if word_count.get(word, 0) >= 5: 
            output.write('%s %s\n' %(word, tags))
        else :
            output.write('%s %s\n' %('_RARE_', tags))

def recursion(prev, i, j, length, sentence, output, wordCounter):
    if length == 1:
        return
    recursion(prev, prev[length][i][j], i, length - 1, sentence, output, wordCounter)
    output.write('%s %s\n' %(sentence[length], wordCounter.get_ith_tags(j)))

def Viterbi(sentence, output, wordCounter):
    sentence = ['*', '*'] + sentence
    m = len(sentence)
    n = wordCounter.tags_size()
    start = wordCounter.get_tags_index('*')
    end   = wordCounter.get_tags_index('STOP')
    dp    = [[[0.0 for i in range(n)] for j in range(n)] for k in range(m)]
    prev  = [[[0   for i in range(n)] for j in range(n)] for k in range(m)]
    dp[1][start][start] = 1.0
    for length in range(2, m):
        word = sentence[length]
        if wordCounter.contain_word(word):
            iword = wordCounter.get_word_index(word)
        else:
            iword = wordCounter.get_word_index('_RARE_')
        for i in range(n):
            for j in range(n):
                for k in range(n):
                    rate = wordCounter.getE(iword, k) * wordCounter.getTrigram(i, j, k)
                    if  dp[length][j][k] < dp[length - 1][i][j] * rate:
                        dp[length][j][k] = dp[length - 1][i][j] * rate
                        prev[length][j][k] = i
    maxi, maxj = 0, 0
    maxr = 0.0
    for i in range(n):
        for j in range(n):
            rate = wordCounter.getTrigram(i, j, end)
            if maxr < dp[m - 1][i][j] * rate:
                maxi, maxj = i, j
                maxr = dp[m - 1][i][j] * rate
    recursion(prev, maxi, maxj, m - 1, sentence, output, wordCounter)

def HMM3(input_data, output, wordCounter):
    n = wordCounter.tags_size()
    sentence = []
    for i in input_data:
        if i == '\n':
            Viterbi(sentence, output, wordCounter)
            sentence[:] = []
            output.write('\n')
        else:
            sentence.append(i.split()[0])

if __name__ == "__main__":
    train_path0 = 'gene.train'
    train_path2 = 'gene.train.2'
    train_count = 'gene.count.2'
    replace([data for data in open(train_path0, 'r')], open(train_path2, 'w'))

    os.system('python count_freqs.py %s > %s' %(train_path2, train_count))
    wordCounter = counter.WordCount([data for data in open(train_count, 'r')])
    input_path  = 'gene.test'
    output_path = 'gene_test.p2.out'

    try:
        input_data = [i for i in file(input_path, "r")]
        HMM3(input_data, open(output_path, 'w'), wordCounter)
    except IOError:
        sys.stderr.write("ERROR: Cannot read inputfile %s.\n" % arg)
        sys.exit(1)
    finally:
        os.system('rm %s' %(train_path2))
        os.system('rm %s' %(train_count))

