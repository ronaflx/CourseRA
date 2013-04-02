import json
import sys
from counter import parser_counter
class CKY:
    def __init__(self, counter, sentence):
        self.counter = counter
        self.symb = counter.get_symb()
        self.sent = sentence
        self.rule = counter.get_rule()

        self.m = len(self.symb)
        self.n = len(self.sent)
        n = self.n
        m = self.m

        self.dp = [[[0.0 for i in range(n)] for i in range(n)] for i in range(m)]
        self.prev = [[[[-1, -1, -1] for i in range(n)] for i in range(n)] for i in range(m)]

    def write_tree(self, state, beg, end):
        cur = self.symb[state]
        k, i, j = self.prev[state][beg][end]
        if beg == end:
            return [cur, self.sent[beg]]
        else:
            return [cur, self.write_tree(i, beg, k), self.write_tree(j, k + 1, end)]

    def solve(self):
        for j in range(self.m):
            for start in range(self.n):
                i = self.counter.get_term_index(self.sent[start])
                self.dp[j][start][start] = self.counter.unary_prob(j, i)

        for length in range(2, self.n + 1):
            for start in range(0, self.n - length + 1):
                end = start + length - 1
                for mid in range(start, end):
                    for k in range(self.m):
                        for i, j, p in self.rule[k]:
                            p *= self.dp[i][start][mid] * self.dp[j][mid + 1][end]
                            if self.dp[k][start][end] < p:
                                self.dp[k][start][end] = p
                                self.prev[k][start][end] = mid, i, j

        state = self.symb.index("SBARQ")
        tree = self.write_tree(state, 0, self.n - 1)
        return json.dumps(tree)

import time
class Timer:

    def __enter__(self):
        self.start = time.clock()
        return self

    def __exit__(self,*args):
        self.end = time.clock()
        self.interval = self.end-self.start

if __name__ == "__main__":
    train_path = 'parse_train.counts.out'
    input_path = 'parse_test.dat' 
    output_path = 'parse_test.p2.out'
    with Timer() as t:
        try:
            train_filp = open(train_path, 'r')
            train_data = [data.strip() for data in train_filp]
            counter = parser_counter(train_data)
            train_filp.close()

            ifstream = open(input_path, 'r')
            dev_data = [data.strip() for data in ifstream]
            ifstream.close()

            ofstream = open(output_path, 'w')
            for data in dev_data:
                cky = CKY(counter, data.split())
                ofstream.write(cky.solve() + '\n')
            ofstream.close()

        except IOError:
            sys.stderr.write("ERROR: Cannot read inputfile.\n")
            sys.exit(1)
    print t.interval
