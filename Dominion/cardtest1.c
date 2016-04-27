#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"

//test ambassador
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, ambassador, cutpurse, sea_hag, tribute, smithy};
	struct gameState g;
	int r, count, supp;

	//for consistancy make randomSeed=1
	initializeGame(3, kCards, 1, &g);

	//turn 1 of player 0 is {4 coppers, 1 estate}. 4 coins
	//turn 1 of player 1 is {2 estate, 3 coppers}. 3 coins
	//turn 1 of player 2 is {1 estate, 2 coppers, 1 estate, 1 copper}. 3 coins
	gainCard(ambassador, &g, 2, 0);
	//play ambassador, reveal 1 estate, try to return 2, but only have 1
	r = playCard(5, 6, 0, 0, &g);
	myAssert(r == -1, "Revealed invalid handPos", __LINE__);
	r = playCard(5, 4, 3, 0, &g);
	myAssert(r == -1, "Choice2 cannot be > 2", __LINE__);
	r = playCard(5, 4, 5, 0, &g);
	myAssert(r == -1, "Played ambassador on itself", __LINE__);
	count = fullDeckCount(1, &g);
	supp = supplyCount(estate, &g);
	r = playCard(5, 4, 2, 0, &g);
	myAssert(r == 0, "Vaild execution of ambassador failed", __LINE__);
	myAssert(numHandCards(&g) == 4, "Did not remove correct amount", __LINE__);
	for(int i = 0; i < 4; ++i)
		myAssert(handCard(i, &g) == copper, "Incorrect cards were removed", __LINE__);
	myAssert(count+1 == fullDeckCount(1, &g), "Other players lost an estate", __LINE__);
	//1 returned but 2 gained by other players (-1)
	myAssert(supp-1 == supplyCount(estate, &g), "Improper # were added and removed from supply", __LINE__);

	//play ambassador, reveal 1 estate, return 2
	endTurn(&g);
	gainCard(ambassador, &g, 2, 1);
	supp = supplyCount(estate, &g);
	r = playCard(5, 0, 2, 0, &g);
	myAssert(r == 0, "Vaild execution of ambassador failed", __LINE__);
	myAssert(numHandCards(&g) == 3, "Did not remove correct amount", __LINE__);
	//2 returned but 2 gained (+0)
	myAssert(supp == supplyCount(estate, &g), "Improper # were added and removed from supply", __LINE__);

	//play ambassador, reveal 1 copper, return 0
	endTurn(&g);
	gainCard(ambassador, &g, 2, 2);
	supp = supplyCount(copper, &g);
	r = playCard(5, 1, 0, 0, &g);
	myAssert(r == 0, "Vaild execution of ambassador failed", __LINE__);
	//0 returned, 2 gained by other players (-2)
	myAssert(supp-2 == supplyCount(copper, &g), "Improper # were added and removed from supply", __LINE__);
	r = myAssert(numHandCards(&g) == 5, "Did not remove correct amount", __LINE__);

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
