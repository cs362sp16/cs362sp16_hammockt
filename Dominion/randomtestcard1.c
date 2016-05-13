#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

//baron
int main(int argc, char* args[])
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, baron, cutpurse, sea_hag, tribute, smithy};
	struct gameState g, orig;
	int r;

	if(argc != 3)
	{
		printf("Usage: randomtestcard1 seed secondsToRun\n");
		return -1;
	}
	int seed = atoi(args[1]);
	int secondsLeft = atoi(args[2]);

	srand(seed);
	initializeGame(2, kCards, seed, &orig);

	printf("Running randomtestcard1 for %d seconds\n", secondsLeft);
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
		gainCard(baron, &g, 2, 0);

		int numEstates = 0;
		for(int i = 0; i < numHandCards(&g); ++i)
			if(handCard(i, &g) == estate)
				numEstates++;

		
		int oldTotalCards = fullDeckCount(0, &g);
		int totalCoins = updateCoins(0, &g);
		int estateSupp = supplyCount(estate, &g);
		int randOption = rand() % 2;
		r = playCard(numHandCards(&g)-1, randOption, 0, 0, &g);
		int newCoins = updateCoins(0, &g);
		myAssert(r == 0, "Played card failed", __LINE__);
		myAssert(g.numBuys == 2, "Did not get a buy", __LINE__);

		int newEstates = 0;
		for(int i = 0; i < numHandCards(&g); ++i)
			if(handCard(i, &g) == estate)
				newEstates++;

		//move playedCards into discard
		endTurn(&g);

		//wants to discard and has estates
		if(randOption && numEstates)
		{
			myAssert(newEstates == numEstates-1, "Did not discard estate when had/wanted to", __LINE__);
			myAssert(newCoins == totalCoins+4, "Did not get 4 coins for discarding estate", __LINE__);
			r = myAssert(oldTotalCards == fullDeckCount(0, &g), "Lost cards", __LINE__);
		}
		//wants to discard but has no estates
		else if(randOption && !numEstates)
		{
			myAssert(estateSupp-1 == supplyCount(estate, &g), "Did not gain estate when had none to discard", __LINE__);
			myAssert(newCoins == totalCoins, "Coins are not the same after forced to gain an estate", __LINE__);
			r = myAssert(oldTotalCards+1 == fullDeckCount(0, &g), "Incorrect total card count", __LINE__);
		}
		//did not want to discard
		else
		{
			myAssert(estateSupp-1 == supplyCount(estate, &g), "Did not gain estate", __LINE__);
			myAssert(newCoins == totalCoins, "Coins are not the same after gaining estate", __LINE__);
			r = myAssert(oldTotalCards+1 == fullDeckCount(0, &g), "Incorrect total card count", __LINE__);
		}
		
		if(r != 0)
			break;
	}

	r = myAssert(1, "", __LINE__);
	if(r == 0)
		printf("Tests completed successfully\n");

	return 0;
}
