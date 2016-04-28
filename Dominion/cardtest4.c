#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"

//test council room
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, council_room, cutpurse, sea_hag, tribute, smithy};
	struct gameState g;
	int r;

	//for consistancy make randomSeed=1
	initializeGame(4, kCards, 1, &g);

	//turn 1 of player 0 is {4 coppers, 1 estate}. 4 coins
	gainCard(council_room, &g, 2, 0);
	r = playCard(5, 0, 0, 0, &g);
	myAssert(r == 0, "councilRoom failed", __LINE__);
	for(int i = 0; i < numHandCards(&g); ++i)
		myAssert(handCard(i, &g) != council_room, "Did not discard council room", __LINE__);
	for(int i = 0; i < 2; ++i)
	{
		r = buyCard(copper, &g);
		myAssert(r == 0, "Does not have 2 buy's. Should have got extra buy from council room", __LINE__);
	}
	myAssert(numHandCards(&g) == 9, "Player 0 does not have 9 cards", __LINE__);
	for(int i = 0; i < 3; ++i)
	{
		endTurn(&g);
		r = myAssert(numHandCards(&g) == 6, "Other players did not draw an extra card", __LINE__);
	}

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
