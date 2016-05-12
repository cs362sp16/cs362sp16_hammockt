#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* args[])
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, ambassador, cutpurse, sea_hag, tribute, smithy};
	struct gameState g, orig;

	if(argc != 3)
	{
		printf("Usage: randomtestadventurer seed secondsToRun\n");
		return -1;
	}
	int seed = atoi(args[1]);
	int secondsLeft = atoi(args[2]);

	srand(seed);
	initializeGame(2, kCards, seed, &orig);

	printf("Running randomtestadventurer for %d seconds\n", secondsLeft);
	clock_t start = clock();
	while(1)
	{
		clock_t totalTime = (clock() - start) / CLOCKS_PER_SEC;
		if(totalTime >= secondsLeft)
			break;

		memcpy(&g, &orig, sizeof(struct gameState));

		int numHand = rand() % 10;
		for(int i = 0; i < numHand; ++i)
		{
			int randCard = rand() % (treasure_map+1);
			g.hand[0][i] = randCard;
		}
		g.handCount[0] = numHand;
		gainCard(adventurer, &g, 2, 0);

		int numDeck = rand() % 20;
		for(int i = 0; i < numDeck; ++i)
		{
			int randCard = rand() % (treasure_map+1);
			g.deck[0][i] = randCard;
		}
		g.deckCount[0] = numDeck;

		playCard(numHandCards(&g)-1, 0, 0, 0, &g);
	}

	return 0;
}