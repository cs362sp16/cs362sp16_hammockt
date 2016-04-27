#include "myAssert.h"
#include "dominion.h"
#include <string.h>

//test getWinners()
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
	int p[MAX_PLAYERS];
	struct gameState g, orig;
	int r;

	//for consistancy make randomSeed=1
	initializeGame(3, kCards, 1, &orig);
	//keep copy of original state for testing purposes
	memcpy(&g, &orig, sizeof(struct gameState));

	//have only player 0 win
	buyCard(estate, &g);
	//get to player 2's turn and end
	for(int i = 0; i < 2; ++i)
		endTurn(&g);
	getWinners(p, &g);
	myAssert(p[0] == 1, "Player 0 did not win", __LINE__);
	myAssert(p[1] == 0 && p[2] == 0, "Player 1 and 2 did not lose", __LINE__);
	myAssert(p[3] == -9999, "Player 4 was not invalid", __LINE__);

	//have a tie between player 1 and 2 and end on player 2
	memcpy(&g, &orig, sizeof(struct gameState));
	endTurn(&g);
	buyCard(estate, &g);
	endTurn(&g);
	buyCard(estate, &g);
	getWinners(p, &g);
	myAssert(p[0] == 0, "Player 0 did not lose", __LINE__);
	myAssert(p[1] == 1 && p[2] == 1, "Player 1 and 2 did not win", __LINE__);

	//have a tie but player 2 wins cause had less turns
	//advance to player 1's turn then end
	for(int i = 0; i < 2; ++i)
		endTurn(&g);
	getWinners(p, &g);
	myAssert(p[0] == 0 && p[1] == 0, "Player 0 and 1 did not lose", __LINE__);
	r = myAssert(p[2], "Player 1 did not win", __LINE__);

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
