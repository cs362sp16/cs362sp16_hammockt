#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

//seaHag
int main(int argc, char* args[])
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, ambassador, cutpurse, sea_hag, tribute, smithy};
	struct gameState g, g2, orig;
	int r;

	if(argc != 3)
	{
		printf("Usage: randomtestcard2 seed secondsToRun\n");
		return -1;
	}
	int seed = atoi(args[1]);
	int secondsLeft = atoi(args[2]);

	srand(seed);
	initializeGame(4, kCards, seed, &orig);

	printf("Running randomtestcard2 for %d seconds\n", secondsLeft);
	clock_t start = clock();
	while(1)
	{
		clock_t totalTime = (clock() - start) / CLOCKS_PER_SEC;
		if(totalTime >= secondsLeft)
			break;


		memcpy(&g, &orig, sizeof(struct gameState));

		gainCard(sea_hag, &g, 2, 0);

		for(int j = 1; j < 4; ++j)
		{
			int numDeck = rand() % 10;
			for(int i = 0; i < numDeck; ++i)
			{
				int randCard = rand() % (treasure_map+1);
				g.deck[j][i] = randCard;
			}
			g.deckCount[j] = numDeck;

			int numDiscard = rand() % 10;
			for(int i = 0; i < numDiscard; ++i)
			{
				int randCard = rand() % (treasure_map+1);
				g.discard[j][i] = randCard;
			}
			g.discardCount[j] = numDiscard;
		}
		memcpy(&g2, &g, sizeof(struct gameState));


		int oldTotalCards = fullDeckCount(0, &g);
		r = playCard(numHandCards(&g)-1, 0, 0, 0, &g);
		myAssert(r == 0, "Played cards failed", __LINE__);

		for(int i = 1; i < 4; ++i)
		{
			myAssert(fullDeckCount(i, &g2)+1 == fullDeckCount(i, &g), "A player did not gain an curse", __LINE__);
			if(g2.deckCount[i] > 0)
				myAssert(g2.deck[i][g2.deckCount[i]-1] == g.discard[i][g.discardCount[i]-1], "A player did not discard top of deck", __LINE__);
			//I cannot align g2's rng and g's rng perfectly so checking g2's deck after a reshuffle is not consistant
			myAssert(g.deck[i][g.deckCount[i]-1] == curse, "Top card of players deck is not a curse", __LINE__);
		}

		endTurn(&g);
		r = myAssert(oldTotalCards == fullDeckCount(0, &g), "Cards were lost", __LINE__);
		if(r != 0)
			break;
	}

	r = myAssert(1, "", __LINE__);
	if(r == 0)
		printf("Tests completed successfully\n");

	return 0;
}
