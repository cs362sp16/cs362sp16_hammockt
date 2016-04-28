#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>

//test salvager
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, salvager, cutpurse, sea_hag, tribute, smithy};
	struct gameState g, orig;
	int r, count;

	//for consistancy make randomSeed=1
	initializeGame(2, kCards, 1, &orig);

	//turn 1 of player 0 is {4 coppers, 1 estate}. 4 coins
	gainCard(salvager, &orig, 2, 0);
	memcpy(&g, &orig, sizeof(struct gameState));

	//invalid input
	r = playCard(5, -1, 0, 0, &g);
	myAssert(r == -1, "Invalid input did not fail", __LINE__);
	r = playCard(5, 6, 0, 0, &g);
	myAssert(r == -1, "Invalid input did not fail", __LINE__);
	r = playCard(5, 5, 0, 0, &g);
	myAssert(r == -1, "Salvager is not the only card in hand", __LINE__);

	//normal input, trash estate. Have 6 total coins
	count = fullDeckCount(0, &g);
	r = playCard(5, 4, 0, 0, &g);
	myAssert(r == 0, "Normal usage failed", __LINE__);
	myAssert(numHandCards(&g) == 4, "Did not discard properly", __LINE__);
	for(int i = 0; i < numHandCards(&g); ++i)
		myAssert(handCard(i, &g) == copper, "Did not discard properly", __LINE__);
	buyCard(silver, &g);
	r = buyCard(sea_hag, &g);
	myAssert(r == -1, "Bought sea_hag and silver >=7 coins", __LINE__);
	r = buyCard(silver, &g);
	myAssert(r == 0, "Could not buy 2 silvers", __LINE__);
	endTurn(&g);
	//trash 1 estate but gained 2 silver (+1)
	myAssert(count+1 == fullDeckCount(0, &g), "Invalid deck count", __LINE__);

	//make salvager only card in hand to trigger empty hand case
	count = fullDeckCount(0, &orig);
	while(numHandCards(&orig) > 1)
	{
		r = safeDiscard(copper, 0, &orig, 0);
		if(r == -1)
			safeDiscard(estate, 0, &orig, 0);
		
	}
	r = playCard(0, 0, 0, 0, &orig);
	myAssert(r == 0, "Have only salvager and playing it fails", __LINE__);
	endTurn(&orig);
	r = myAssert(count == fullDeckCount(0, &orig), "Cards were trashed, which was unexpected", __LINE__);

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
