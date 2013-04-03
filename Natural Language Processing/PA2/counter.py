from collections import Counter

class parser_counter:
    def __init__(self, train_data):
        symb_dic = Counter()
        term_set = set()

        for key in train_data:
            data = key.split()
            if data[1] == 'NONTERMINAL':
                symb_dic[data[2]] += int(data[0])
            elif data[1] == 'UNARYRULE':
                term_set.add(data[3])

        self.symb  = symb_dic.keys()
        self.term  = list(term_set)
        self.start = self.symb.index("SBARQ")
        self.rare  = self.term.index("_RARE_")

        n = len(self.symb)
        m = len(self.term)
        self.symb_length, self.term_length = n, m

        self.unary = [[0.0 for i in range(m)] for i in range(n)]
        self.rule  = [list() for i in range(n)]

        for key in train_data:
            data = key.split()
            if data[1] == 'UNARYRULE':
                i = self.symb.index(data[2])
                j = self.term.index(data[3])
                self.unary[i][j] = float(data[0]) / float(symb_dic[data[2]])
            elif data[1] == 'BINARYRULE':
                i = self.symb.index(data[2])
                j = self.symb.index(data[3])
                k = self.symb.index(data[4])
                self.rule[i].append((j, k, float(data[0]) / float(symb_dic[data[2]])))

    def get_term_index(self, term):
        try:
            i = self.term.index(term)
            return i
        except:
            return self.term.index('_RARE_')
