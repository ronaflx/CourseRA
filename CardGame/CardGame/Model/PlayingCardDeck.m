//
//  PlayingCardDeck.m
//  CS193P
//
//  Created by Lixin Fan on 1/25/14.
//  Copyright (c) 2014 Lixin Fan. All rights reserved.
//

#import "PlayingCardDeck.h"
#import "PlayingCard.h"

@implementation PlayingCardDeck

- (instancetype) init {
    self = [super init];
    if (self) {
        for (NSString* suit in [PlayingCard validSuits]) {
            for (NSUInteger rank = 1; rank <= [PlayingCard maxRank]; rank++) {
                PlayingCard* card = [[PlayingCard alloc] init];
                card.suit = suit;
                card.rank = rank;
                [self addCard: card];
            }
        }
    }
    return self;
}
@end
