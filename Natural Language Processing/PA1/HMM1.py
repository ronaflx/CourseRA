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

def HMM1(input_data, output, wordCounter):
    n = wordCounter.tags_size()
    for raw_data in input_data:
        if len(raw_data) == 1:
            output.write('\n')
        else:
            info = raw_data.split()[0]
            res = 0.0
            tag = ''
            if not wordCounter.contain_word(info):
                j = wordCounter.get_word_index('_RARE_')
            else:
                j = wordCounter.get_word_index(info)
            for i in range(n):
                if res  < wordCounter.getE(j, i):
                    res = wordCounter.getE(j, i)
                    tag = wordCounter.get_ith_tags(i)
            output.write('%s %s\n' %(info, tag))

if __name__ == "__main__":
    train_path0 = 'gene.train'
    train_path1 = 'gene.train.1'
    train_count = 'gene.count.1'
    replace([data for data in open(train_path0, 'r')], open(train_path1, 'w'))

    os.system('python count_freqs.py %s > %s' %(train_path1, train_count))
    wordCounter = counter.WordCount([data for data in open(train_count, 'r')])
    input_path  = 'gene.test'
    output_path = 'gene_test.p1.out'

    try:
        input_data = [i for i in file(input_path, "r")]
        HMM1(input_data, open(output_path, 'w'), wordCounter)
    except IOError:
        sys.stderr.write("ERROR: Cannot read inputfile %s.\n" % arg)
        sys.exit(1)
    finally:
        os.system('rm %s' %(train_path1))
        os.system('rm %s' %(train_count))

