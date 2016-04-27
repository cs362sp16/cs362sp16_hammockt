#include "myAssert.h"
#include "dominion.h"

//test scoreFor()
int main()
{
	int kCards[10] = {adventurer, gardens, embargo, village, minion, mine, cutpurse, sea_hag, tribute, smithy};
	struct gameState g;
	int r;

	//for consistancy make randomSeed=1
	initializeGame(3, kCards, 1, &g);

	//first hand of player 0 is {4 coppers, 1 estate}. 4 coins
	buyCard(gardens, &g); //player 0 gets a garden
	endTurn(&g);
	buyCard(curse, &g); //player 1 gets a curse
	endTurn(&g);
	//player 2 gets nothing (control)
	//get invalid score
	r = scoreFor(3, &g);
	myAssert(r == -9999, "Incorrect score for nonexistant player", __LINE__);

	//get control score
	r = scoreFor(2, &g);
	myAssert(r == 3, "Control did not score 3", __LINE__);

	//get score of player with curse
	r = scoreFor(1, &g);
	myAssert(r == 2, "Player with curse did not score 2", __LINE__);

	//get score of player with garden
	r = scoreFor(0, &g);
	r = myAssert(r == 4, "Player with garden did not score 4", __LINE__);

	//done
	if(r == 0)
		printf("Tests completed successfully!\n");

	return 0;
}
