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

		int randOption = rand() % 2;
		playCard(numHandCards(&g)-1, randOption, 0, 0, &g);
	}

	return 0;
}