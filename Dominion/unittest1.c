#include "myAssert.h"
#include "dominion.h"
#include <string.h>

//test buyCard()
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
	struct gameState orig, g;
	int r, count;

	//for consistancy make randomSeed=1
	initializeGame(2, kCards, 1, &orig);
	//keep copy of original state for testing purposes
	memcpy(&g, &orig, sizeof(struct gameState));

	//first hand of player 0 is {4 coppers, 1 estate}. 4 coins
	count = supplyCount(curse, &g);
	r = buyCard(-1, &g);
	myAssert(r == -1, "Bought a non-existant card", __LINE__);
	r = buyCard(council_room, &g);
	myAssert(r == -1, "Bought council room which is not in supply", __LINE__);
	r = buyCard(gold, &g);
	myAssert(r == -1, "Gold costs 6 coins, bought with 4", __LINE__);
	r = buyCard(embargo, &g);
	myAssert(r == 0, "Could not buy an embargo with 4 coins", __LINE__);
	r = buyCard(copper, &g);
	myAssert(r == -1, "Bought more than one card on first turn", __LINE__);
	myAssert(count == supplyCount(curse, &g), "curse gained before embargo played", __LINE__);

	//player 0 gets embargo in hand buy turn 4 in handPos 3
	//advance to turn 4 for player 0
	for(int i = 0; i < 6; ++i)
		endTurn(&g);
	myAssert(handCard(3, &g) == embargo, "Did not properly gain bought embargo", __LINE__);
	playCard(3, copper, 0, 0, &g); //put an embargo on coppers
	endTurn(&g);
	//hand of player 1 turn 4 is {4 coppers, 1 estate}. 4 coins
	count = supplyCount(curse, &g);
	r = buyCard(copper, &g);
	myAssert(r == 0, "Try Buying a copper with a curse on it", __LINE__);
	myAssert(count-1 == supplyCount(curse, &g), "Improper amount of curses removed from supply", __LINE__);
	//advance to turn 5 where curse should appear in handPos 0
	for(int i = 0; i < 2; ++i)
		endTurn(&g);
	r = handCard(0, &g);
	myAssert(r == curse, "Player 1 did not recieve curse", __LINE__);

	//restart game
	buyCard(smithy, &orig);
	//smithy will show up in turn 4 in handPos 3
	//turn 4 player 0 hand is {4 coppers, 1 smithy}
	for(int i = 0; i < 6; ++i)
		endTurn(&orig);
	buyCard(silver, &orig);
	r = playCard(3, 0, 0, 0, &orig);
	r = myAssert(r == -1, "buyCard could not advance the phase", __LINE__);

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
