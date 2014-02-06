#import "ViewController.h"
#import "Deck.h"
#import "PlayingCardDeck.h"
#import "CardMatchingGame.h"

@interface ViewController ()
@property (strong, nonatomic) IBOutletCollection(UIButton) NSArray *cardButtons;
@property (nonatomic, strong) Deck *deck;
@property (nonatomic, strong) CardMatchingGame *game;
@property (weak, nonatomic) IBOutlet UILabel *scoreLabel;
@end

@implementation ViewController

- (Deck *) createDeck {
    return [[PlayingCardDeck alloc] init];
}

- (CardMatchingGame *) game {
    if (!_game) {
        _game = [[CardMatchingGame alloc] initWithCardCount:[self.cardButtons count]
                                                  usingDeck:[self createDeck]];
    }
    return _game;
}

- (Deck *) deck {
    if (!_deck) {
        _deck = [self createDeck];
    }
    return _deck;
}

- (IBAction)touchCardButton:(UIButton *)sender {
    int cardIndex = [self.cardButtons indexOfObject:sender];
    [self.game chooseCardIndex:cardIndex];
    [self updateUI];
}

- (void) updateUI {
    for (UIButton * cardButton in self.cardButtons) {
        int cardIndex = [self.cardButtons indexOfObject:cardButton];
        Card *card = [self.game cardAtIndex:cardIndex];
        [cardButton setTitle:[self titleForCard:card]
                    forState:UIControlStateNormal];
        [cardButton setBackgroundImage:[self backgroundImageForCard:card]
                              forState:UIControlStateNormal];
        cardButton.enabled = !card.isMatched;
    }
    self.scoreLabel.text = [NSString stringWithFormat:@"Score: %d", self. game.score];
}

- (NSString *)titleForCard:(Card *)card {
    return card.isChosen ? card.contents : @"";
}

- (UIImage *) backgroundImageForCard:(Card *)card {
    return [UIImage imageNamed:card.isChosen ? @"cardfront" : @"cardback"];
}

@end
