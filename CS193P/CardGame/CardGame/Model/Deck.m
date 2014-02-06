#import "Deck.h"

@interface Deck()
@property (strong, nonatomic) NSMutableArray *cards;
@end

@implementation Deck

- (NSMutableArray*) cards {
    if (!_cards) {
        _cards = [[NSMutableArray alloc] init];
    }
    return _cards;
}
- (void)addCard:(Card*) card atTop:(BOOL) atTop {
    if (atTop) {
        [self.cards insertObject: card atIndex:0];
    } else {
        [self.cards addObject:card];
    }
}

- (Card*) drawRandomCard {
    Card *randomCard = nil;
    unsigned long counter = [self.cards count];
    if (counter != 0) {
        unsigned index = arc4random() % [self.cards count];
        randomCard = self.cards[index];
        [self.cards removeObjectAtIndex:index];
    }
    return randomCard;
}

- (void)addCard:(Card *)card {
    [self addCard:card atTop:NO];
}
@end
