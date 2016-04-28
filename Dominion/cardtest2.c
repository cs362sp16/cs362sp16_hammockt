#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>

//test baron
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, baron, cutpurse, sea_hag, tribute, smithy};
	struct gameState g, orig;
	int r;

	//for consistancy make randomSeed=1
	initializeGame(2, kCards, 1, &orig);

	//turn 1 of player 0 is {4 coppers, 1 estate}. 4 coins
	gainCard(baron, &orig, 2, 0);
	memcpy(&g, &orig, sizeof(struct gameState));

	//play baron normal, discard estate. get 4 coins
	r = supplyCount(estate, &g);
	playCard(5, 1, 0, 0, &g);
	myAssert(r == supplyCount(estate, &g), "Baron gained estate while discarding", __LINE__);
	myAssert(numHandCards(&g) == 4, "Did not properly discard", __LINE__);
	for(int i = 0; i < numHandCards(&g); ++i)
		myAssert(handCard(i, &g) == copper, "Did not properly discard", __LINE__);
	//have 8 coins total
	r = buyCard(gold, &g);
	myAssert(r == 0, "Did not have 6 coins to buy gold", __LINE__);
	r = buyCard(silver, &g);
	myAssert(r == -1, "Bought silver, had more than 8 coins", __LINE__);
	r = buyCard(estate, &g);
	myAssert(r == 0, "Did not have 8 coins or 2 buys for estate", __LINE__);
	r = buyCard(copper, &g);
	myAssert(r == -1, "Got more than +1 buy from baron", __LINE__);

	//play baron normal, dont discard estate so gain one
	memcpy(&g, &orig, sizeof(struct gameState));
	r = supplyCount(estate, &g);
	playCard(5, 0, 0, 0, &g);
	myAssert(r-1 == supplyCount(estate, &g), "Baron did not gain estate", __LINE__);
	myAssert(handCard(4, &g) == estate, "Baron discarded estate in hand while gaining", __LINE__);
	r = buyCard(duchy, &g);
	myAssert(r == -1, "Bought duchy, gained coins when gaining estate", __LINE__);

	//play baron, discard estate but have none so gain one
	discardCard(4, 0, &orig, 0);
	r = supplyCount(estate, &orig);
	playCard(4, 1, 0, 0, &orig);
	r = myAssert(r-1 == supplyCount(estate, &orig), "Did not gain with no estates in hand", __LINE__);

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
