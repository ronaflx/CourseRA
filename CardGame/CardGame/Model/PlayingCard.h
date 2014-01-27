#import "Card.h"

@interface PlayingCard : Card

@property (strong, nonatomic) NSString* suit;
@property (nonatomic) NSUInteger rank;

+ (NSArray*) validSuits;
+ (NSUInteger) maxRank;

@end
