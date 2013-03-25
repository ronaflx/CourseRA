class WordCount:
    def __init__(self, train_data):
        # init tags_list and word_list
        self.tags_set = set(['*', 'STOP'])
        self.word_set = set()

        for raw_data in train_data:
            info = raw_data.split()
            if info[1] == 'WORDTAG':
                count, label, tags, word = info[0:4]
                self.tags_set.add(tags)
                self.word_set.add(word)
        self.tags_list = [tags for tags in self.tags_set]
        self.word_list = [word for word in self.word_set]

        # init tags_index and word_index
        n = len(self.tags_set)
        m = len(self.word_set)
        self.tags_index = dict(zip(self.tags_set, range(n)))
        self.word_index = dict(zip(self.word_set, range(m)))

        # init tags_count
        self.tags_count = [0] * n
        for raw_data in train_data:
            info = raw_data.split()
            if info[1] == 'WORDTAG':
                count, label, tags, word = info[0:4]
                self.tags_count[self.tags_index[tags]] += int(count)

        # init probability
        self.e = [[0.0 for i in range(n)] for j in range(m)]
        self.bigram  = [[0.0 for i in range(n)] for j in range(n)]
        self.trigram = [[[0.0 for i in range(n)] for j in range(n)] for k in range(n)]
        self.train(train_data)

    def train(self, train_data):
        for raw_data in train_data:
            info = raw_data.split()
            if info[1] == "WORDTAG" :
                count, label, tags, word = info[0:4]
                iword = self.word_index[word]
                itags = self.tags_index[tags]
                self.e[iword][itags] = float(count) / float(self.tags_count[itags])
            elif info[1] == '2-GRAM':
                i, j = self.tags_index[info[2]], self.tags_index[info[3]]
                self.bigram[i][j] = int(info[0])
            elif info[1] == '3-GRAM':
                i, j, k = self.tags_index[info[2]], self.tags_index[info[3]], self.tags_index[info[4]]
                self.trigram[i][j][k] = int(info[0])

    def getE(self, iword, itags):
        return self.e[iword][itags]

    def getTrigram(self, i, j, k):
        if self.bigram[i][j] == 0:
            return 0
        else:
            return float(self.trigram[i][j][k]) / float(self.bigram[i][j])

    def tags_size(self):
        return len(self.tags_list)
    def word_size(self):
        return len(self.word_list)

    def get_ith_tags(self, i):
        return self.tags_list[i]
    def get_ith_word(self, i):
        return self.word_list[i]

    def contain_tags(self, tags):
        return tags in self.tags_set
    def contain_word(self, word):
        return word in self.word_set

    def get_tags_index(self, tags):
        return self.tags_index[tags]
    def get_word_index(self, word):
        return self.word_index[word]
