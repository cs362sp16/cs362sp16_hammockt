#include "myAssert.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* args[])
{
	int kCards[10];
	struct gameState g;
	int r;

	if(argc != 2)
	{
		printf("Usage: testdominion seed\n");
		return -1;
	}
	int seed = atoi(args[1]);

	//seed random number generator
	srand(seed);

	//generate 10 unique kingdom cards
	for(int i = 0; i < 10; ++i)
	{
	PICK_KINGDOM:;
		//generate a kingdom card which is any card between adventurer and treasure_map
		int rCard = (rand() % (treasure_map+1-adventurer)) + adventurer;

		//make sure that we do not have any duplicates
		//if we do then get a new random card
		for(int j = 0; j <= i; ++j)
			if(kCards[j] == rCard)
				goto PICK_KINGDOM;

		//no duplicates so add it array
		kCards[i] = rCard;
	}

	//randomly assign 2-4 players
	int numPlayers = (rand() % 3) + 2;
	r = initializeGame(numPlayers, kCards, seed, &g);

	printf("Running testdominion\n");
	printf("Using cards: ");
	for(int i = 0; i < 9; ++i)
		printf("%d, ", kCards[i]);
	printf("%d\n\n", kCards[9]);

	myAssert(r != -1, "init failed", __LINE__);
		
	while(!isGameOver(&g))
	{
		//have a 3/4 chance of playing an action
		while(g.numActions > 0 && (rand() % 4) != 3)
		{
			//to pick a random action card store them in temp array then pick
			int numHand = numHandCards(&g);
			int temp[numHand], tempSize = 0;

			//iterate over hand and if action then store its position
			for(int i = 0; i < numHand; ++i)
				if(isAction(handCard(i, &g)))
					temp[tempSize++] = i;

			//if we run out or have no actions cards, then stop playing them
			//even if we have more actions to use
			if(tempSize == 0)
				break;

			//pick a random action card position and play it
			int rActionCard = rand() % tempSize;

			//1 in 10 chance that we will try illegal input
			if((rand() % 10) == 9)
				playCard(temp[rActionCard], -1, -1, -1, &g);
			else
			{
				//pick randomish choices
				int rc[3];
				for(int i = 0; i < 3; ++i)
					rc[i] = rand() % numHand;

				playCard(temp[rActionCard], rc[0], rc[1], rc[2], &g);
			}
		}

		//have a 3/4 chance of buying a card
		while(g.numBuys > 0 && (rand() % 4) != 3)
		{
		PICK_BUY:;
			//pick a random card from supply to buy
			int supplyPos = rand() % (treasure_map+1);

			//if card is not in current supply, has run out, or we do not have enough coins to buy
			//then pick a new random card to buy
			if(supplyCount(supplyPos, &g) < 1 || updateCoins(whoseTurn(&g), &g) < getCost(supplyPos))
				goto PICK_BUY;

			buyCard(supplyPos, &g);

			//1 in 20 chance of doing something illegal
			if((rand() % 20) == 19)
			{
				int numHand = numHandCards(&g);
				//pick randomish choices
				int rc[3];
				for(int i = 0; i < 3; ++i)
					rc[i] = rand() % numHand;
				r = playCard(rand() % numHand, rc[0], rc[1], rc[2], &g);
				myAssert(r == -1, "Played a card during the buy phase", __LINE__);
			}
		}

		//go to the next players turn
		endTurn(&g);
	}

	int results[MAX_PLAYERS];
	getWinners(results, &g);

	printf("The results are...\n");
	for(int i = 0; i < numPlayers; ++i)
	{
		if(results[i])
			printf("Player %d Won!  score: %d\n", i, scoreFor(i, &g));
		else
			printf("Player %d Lost. score: %d\n", i, scoreFor(i, &g));
	}

	return 0;
}
