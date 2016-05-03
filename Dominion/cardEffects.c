#include "cardEffects.h"

int adventurerEffect(struct gameState* state, int currentPlayer, int handPos)
{
	for(int drawntreasure = 0; drawntreasure < 2; )
	{
		//draw a card. Automatically puts discard into deck but quit if discard and deck are both empty
		if(drawCard(currentPlayer, state) == -1)
			break;

		int cardDrawn = TOP(hand, currentPlayer); //top card of hand is most recently drawn card.
		if(isTreasure(cardDrawn)) //is treasure?
			drawntreasure++;
		else
		{
			PUSH(discard, currentPlayer, cardDrawn); //discard all cards in play that have been drawn
			POP(hand, currentPlayer); //this should just remove the top card (the most recently drawn one).
		}
	}

	//put played card in discard pile
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

int councilRoomEffect(struct gameState* state, int currentPlayer, int handPos)
{
	//Everyone else gets 1 card
	for(int i = 0; i < state->numPlayers; ++i)
		if(i != currentPlayer)
			drawCard(i, state);

	//+4 cards for player
	drawCards(currentPlayer, state, 4);

	//+1 Buy
	state->numBuys++;

	//put played card in played card pile
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

int baronEffect(struct gameState* state, int currentPlayer, int handPos, int choice1)
{
	state->numBuys++;

	if(choice1) //Boolean true or going to discard an estate
	{
		for(int i = 0; i < state->handCount[currentPlayer]; ++i)
		{
			if(state->hand[currentPlayer][i] == estate)
			{
				state->coins += 4;
				discardCard(i, currentPlayer, state, 0); //discard estate
				safeDiscard(baron, currentPlayer, state, 0); //discard baron
				return 0;
			}
		}

		//could not find estate. Must acquire one
	}

	gainCard(estate, state, 0, currentPlayer); //try to get estate

	discardCard(handPos, currentPlayer, state, 0); //discard baron
	return 0;
}

//can be problem here if card chosen is not in supply but not currently possible due to card selection
int ambassadorEffect(struct gameState* state, int currentPlayer, int handPos, int choice1, int choice2)
{
	//choice1 = pos of card to reveal, choice2 = # to return to supply
	if(choice1 < 0 || choice1 >= state->handCount[currentPlayer])
		return -1;
	//passes this then we have at least 2 cards in hand
	if(choice2 > 2 || choice2 < 0 || choice1 == handPos)
		return -1;

	#if DEBUG
		printf("Player %d reveals card number: %d\n", currentPlayer, state->hand[currentPlayer][choice1]);
	#endif

	int card = state->hand[currentPlayer][choice1];
	int numOccur = 0;
	for(int i = 0; i < choice2; ++i)
		if(safeDiscard(card, currentPlayer, state, 1) != -1)
			++numOccur;

	//in case they have less then choice2
	state->supplyCount[card] += numOccur;

	//each other player gains a copy of revealed card
	//its suppose to start after currentPlayer and matters when supply runs out
	for(int i = 0; i < state->numPlayers; ++i)
		if(i != currentPlayer)
			gainCard(card, state, 0, i);

	//discard played card from hand
	safeDiscard(ambassador, currentPlayer, state, 0);
	return 0;
}

//everyone discards at most one card so discard is safe
int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos)
{
	state->coins += 2; //+2 coins

	for(int i = 0; i < state->numPlayers; ++i)
	{
		if(i == currentPlayer) //don't process ourselves
			continue;

		//try to discard 1 copper and stop
		for(int j = 0; j < state->handCount[i]; ++j)
		{
			if(state->hand[i][j] == copper)
			{
				discardCard(j, i, state, 0); //handPos, currentPlayer
				goto LOOP_UPDATE; //skip over card reveal
			}
		}

		//did we check every card? if yes then goto will not skip over this
		for(int j = 0; j < state->handCount[i]; ++j) //can optimize here but code might break if handCount < 0
		{
			#if DEBUG
				printf("Player %d reveals card number %d\n", i, state->hand[i][j]);
			#endif
		}
		LOOP_UPDATE:;
	}

	//discard played card from hand
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

int salvagerEffect(struct gameState* state, int currentPlayer, int handPos, int choice1)
{
	if(choice1 < 0 || choice1 >= state->handCount[currentPlayer])
		return -1;
	if(choice1 == handPos && state->handCount[currentPlayer] > 1)
		return -1;

	state->numBuys++;

	if(choice1 != handPos)
	{
		//gain coins equal to trashed card
		state->coins += getCost(state->hand[currentPlayer][choice1]);
		//trash card
		discardCard(choice1, currentPlayer, state, 1);
	}

	//discard salvager
	safeDiscard(salvager, currentPlayer, state, 0);
	return 0;
}

int seaHagEffect(struct gameState* state, int currentPlayer, int handPos)
{
	for(int i = 0; i < state->numPlayers; ++i)
	{
		if(i != currentPlayer)
		{
			//add to discard by taking from deck
			//puts in hand and deals with deck being empty. Dont discard if deck and discard are empty
			if(drawCard(i, state) != -1) //drawed a card so discard it and pop from hand
				PUSH(discard, i, POP_R(hand, i));

			//try to add curse to top of deck
			gainCard(curse, state, 1, i);
		}
	}

	//discard played card from hand
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

int treasureMapEffect(struct gameState* state, int currentPlayer, int handPos)
{
	//search hand for another treasure_map
	for(int i = 0; i < state->handCount[currentPlayer]; ++i)
	{
		if(i != handPos && state->hand[currentPlayer][i] == treasure_map)
		{
			//trash other treasure card
			discardCard(i, currentPlayer, state, 1);

			//try to gain 4 Gold cards
			for(i = 0; i < 4; ++i)
				gainCard(gold, state, 1, currentPlayer);

			break; //stop trashing treasure_maps
		}
	}

	//always trash this treasure card
	safeDiscard(treasure_map, currentPlayer, state, 1);

	return 0;
}
