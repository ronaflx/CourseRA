@import Foundation;
#import "Card.h"

@interface Deck : NSObject

- (void)addCard:(Card*) card atTop:(BOOL) atTop;
- (void)addCard:(Card*) card;
- (Card*) drawRandomCard;

@end
