#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>

//test endTurn()
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, outpost, cutpurse, sea_hag, tribute, smithy};
	struct gameState g, orig;
	int r, count;

	//for consistancy make randomSeed=1
	initializeGame(3, kCards, 1, &orig);
	//keep copy of original state for testing purposes
	memcpy(&g, &orig, sizeof(struct gameState));

	//make sure cards are properly cycling around hand, discard, deck, and playedCards
	//this is a shortcut, to achieve this without shortcut would require an absurd # of operations
	gainCard(smithy, &g, 2, 0);
	count = fullDeckCount(0, &g);
	//play smithy (which puts it into playedCards)
	playCard(5, 0, 0, 0, &g);
	endTurn(&g);
	myAssert(count == fullDeckCount(0, &g), "endTurn lost some cards!", __LINE__);
	myAssert(g.handCount[0] == 5, "Player 0 did not draw 5 cards", __LINE__);
	myAssert(whoseTurn(&g) == 1, "endTurn did not advance player", __LINE__);

	//make sure it cycles back to 0 after it hits numPlayers
	endTurn(&g);
	endTurn(&g);
	myAssert(whoseTurn(&g) == 0, "endTurn cannot cycle back to player 0", __LINE__);

	//endTurn needs to handle outpost, does all of its magic
	memcpy(&g, &orig, sizeof(struct gameState));
	gainCard(outpost, &g, 2, 0);
	playCard(5, 0, 0, 0, &g);
	endTurn(&g);
	myAssert(whoseTurn(&g) == 0, "Player 0 did not get a second turn", __LINE__);
	myAssert(g.handCount[0] == 3, "Player 0 did not get 3 cards for extra turn", __LINE__);
	//outpost needs to remain in play until end of extra turn
	myAssert(g.playedCardCount == 1 && g.playedCards[0] == outpost, "Outpost did not remain in play", __LINE__);

	//you only get one extra turn with outpost!
	gainCard(outpost, &g, 2, 0);
	playCard(3, 0, 0, 0, &g);
	endTurn(&g);
	myAssert(whoseTurn(&g) != 0, "Player 0 got more than 1 extra turn!", __LINE__);
	myAssert(whoseTurn(&g) == 1, "Did not advance to player 1 when second outpost was played", __LINE__);

	//do not advance if gameOver is over
	//game ends when province pile is empty
	orig.supplyCount[province] = 0;
	endTurn(&orig);
	r = myAssert(whoseTurn(&orig) == 0, "Continues to advance when game is over!", __LINE__);

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
