#import <Foundation/Foundation.h>
#import "Deck.h"

@interface CardMatchingGame : NSObject

// designated initializer
- (instancetype) initWithCardCount:(NSUInteger) count
                            usingDeck:(Deck *) deck;

- (void) chooseCardIndex:(NSUInteger)index;

- (Card*) cardAtIndex:(NSInteger)index;

@property (nonatomic, readonly) NSInteger score;
@end
