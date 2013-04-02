from collections import Counter

class parser_counter:
    def __init__(self, train_data):
        self.symb_counter = Counter()
        self.term_counter = Counter()
        for key in train_data:
            data = key.split()
            if data[1] == 'NONTERMINAL':
                self.symb_counter[data[2]] += int(data[0])
            elif data[1] == 'UNARYRULE':
                self.term_counter[data[3]] += int(data[0])
        self.symb = self.symb_counter.keys()
        self.term = self.term_counter.keys()

        n = len(self.symb)
        m = len(self.term)
        self.unary_counter = [[0.0 for i in range(m)] for i in range(n)]
        self.rule = [list() for i in range(n)]

        for key in train_data:
            data = key.split()
            if data[1] == 'UNARYRULE':
                i = self.symb.index(data[2])
                j = self.term.index(data[3])
                self.unary_counter[i][j] += float(data[0])
            elif data[1] == 'BINARYRULE':
                i = self.symb.index(data[2])
                j = self.symb.index(data[3])
                k = self.symb.index(data[4])
                self.rule[i].append([j, k, float(data[0]) / float(self.symb_counter[self.symb[i]])])

    def unary_prob(self, symb, term):
        return self.unary_counter[symb][term] / self.symb_counter[self.symb[symb]]

    def get_symb(self):
        return self.symb
    def get_rule(self):
        return self.rule

    def get_term_index(self, term):
        try:
            i = self.term.index(term)
            return i
        except:
            return self.term.index('_RARE_')
