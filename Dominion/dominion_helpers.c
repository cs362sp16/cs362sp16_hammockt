#include "dominion_helpers.h"
#include <string.h>

//modified
int drawCard(int player, struct gameState *state)
{
	//Deck is empty
	if(state->deckCount[player] <= 0)
	{
		//if deck and discard are empty then we cannot draw
		if(state->discardCount[player] <= 0)
			return -1;

		//Step 1 Shuffle the discard pile back into a deck
		//Move discard to deck & clear discard to -1 (for now)
		memcpy(state->deck[player], state->discard[player], state->discardCount[player] * sizeof(int));
		memset(state->discard[player], -1, state->discardCount[player] * sizeof(int)); //nbytes = count * int

		state->deckCount[player] = state->discardCount[player];
		state->discardCount[player] = 0;//Reset discard

		//Shuffle the deck
		shuffle(player, state);//Shuffle the deck up and make it so that we can draw
		#if DEBUG
			printf("Deck count now: %d\n", state->deckCount[player]);
		#endif
	}

	//Draw the Card
	int count = state->handCount[player];//Get current player's hand count
	#if DEBUG
		printf("Current hand count: %d\n", count);
	#endif

	int deckCounter = state->deckCount[player];//Create a holder for the deck count

	//can do this all in one line but will be long and not pretty
	state->hand[player][count] = state->deck[player][deckCounter - 1];//Add card to hand
	state->deckCount[player]--;
	state->handCount[player]++;//Increment hand count

	return 0;
}

//modified
int updateCoins(int player, struct gameState* state, int bonus)
{
	//reset coin count
	state->coins = 0;

	for(int i = 0; i < state->handCount[player]; ++i)
	{
		switch(state->hand[player][i])
		{
			case copper: state->coins++; break;
			case silver: state->coins += 2; break;
			case gold:   state->coins += 3; break;
		}
	}

	//add bonus
	state->coins += bonus;

	return 0;
}

//modified
int discardCard(int handPos, int currentPlayer, struct gameState *state, int trashFlag)
{
	//if card is not trashed, add to Played pile
	if(trashFlag < 1)
	{
		state->playedCards[state->playedCardCount] = state->hand[currentPlayer][handPos];
		state->playedCardCount++;
	}

	//remove card from player's hand (decrement hand)
	state->handCount[currentPlayer]--;
	//replace discarded card with last card in hand if not last
	if(handPos != state->handCount[currentPlayer])
		state->hand[currentPlayer][handPos] = state->hand[currentPlayer][state->handCount[currentPlayer]]; //safe cause dec

	//set last card to -1
	state->hand[currentPlayer][state->handCount[currentPlayer]] = -1; //safe cause dec

	return 0;
}

//modified
int gainCard(int supplyPos, struct gameState *state, int toFlag, int player)
{
	//Note: supplyPos is enum of choosen card

	//check if supply pile is empty (0) or card is not used in game (-1)
	if(supplyCount(supplyPos, state) < 1)
		return -1;

	//added card for [whoseTurn] current player:
	// toFlag = 0 : add to discard
	// toFlag = 1 : add to deck
	// toFlag = 2 : add to hand
	switch(toFlag)
	{
		case 0:
			state->discard[player][state->discardCount[player]] = supplyPos;
			state->discardCount[player]++;
			break;
		case 1:
			state->deck[player][state->deckCount[player]] = supplyPos;
			state->deckCount[player]++;
			break;
		case 2:
			state->hand[player][state->handCount[player]] = supplyPos;
			state->handCount[player]++;
			break;
	}

	//decrease number in supply pile
	state->supplyCount[supplyPos]--;

	return 0;
}

//fixed
int getCost(int cardNumber)
{
	switch(cardNumber)
	{
		case curse:
		case copper: return 0;

		case estate:
		case embargo: return 2;

		case silver:
		case village:
		case great_hall:
		case steward:
		case ambassador: return 3;

		case feast:
		case gardens:
		case remodel:
		case smithy:
		case baron:
		case cutpurse:
		case salvager:
		case sea_hag:
		case treasure_map: return 4;

		case duchy:
		case council_room:
		case mine:
		case minion:
		case tribute:
		case outpost: return 5;

		case gold:
		case adventurer: return 6;

		case province: return 8;
	}

	return -1;
}