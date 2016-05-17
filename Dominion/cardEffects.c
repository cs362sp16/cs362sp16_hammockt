#include "cardEffects.h"
#include <stdio.h>

int adventurerEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int origSize = state->playedCardCount;

	for(int drawntreasure = 0; drawntreasure < 2; )
	{
		//draw a card. Automatically puts discard into deck but quit if discard and deck are both empty
		if(drawCard(currentPlayer, state) == -1)
			break;

		int cardDrawn = TOP(hand, currentPlayer); //top card of hand is most recently drawn card.
		if(isTreasure(cardDrawn))
			drawntreasure++;
		else
		{
			//discard all cards in play that have been drawn into a "reveal pile"
			//if we put straight into discard then we can create an infinite loop
			state->playedCards[state->playedCardCount++] = cardDrawn;
			POP(hand, currentPlayer); //this should just remove the top card (the most recently drawn one).
		}
	}
	moveFromOffset(state->discard[currentPlayer], state->playedCards,
					state->discardCount+currentPlayer, &state->playedCardCount, origSize);

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

int tributeEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int nextPlayer = (currentPlayer + 1) % state->numPlayers;
	int j = state->handCount[nextPlayer]; //pos of interest

	drawCards(nextPlayer, state, 2); //try to draw/reveal 2 cards

	//handle drawing 2 identical cards. Need to discard [j+1] from hand if true
	if(state->handCount[nextPlayer] == (j+2) && state->hand[nextPlayer][j] == state->hand[nextPlayer][j+1])
		PUSH(discard, nextPlayer, POP_R(hand, nextPlayer));

	//decrementing so I can process and remove at the same time
	for(int i = state->handCount[nextPlayer]-1; i >= j; --i)
	{
		//cards can be multiple types
		if(isTreasure(state->hand[nextPlayer][i])) //Treasure cards
			state->coins += 2;
		if(isVictory(state->hand[nextPlayer][i])) //Victory cards
			drawCards(currentPlayer, state, 2);
		if(isAction(state->hand[nextPlayer][i])) //Action cards
			state->numActions += 2;

		//discard the processed card. Its the top card so we can pop it
		PUSH(discard, nextPlayer, POP_R(hand, nextPlayer));
	}

	//discard played card from hand
	discardCard(handPos, currentPlayer, state, 0);

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

	int numOccur = 0, card = state->hand[currentPlayer][choice1];
	for(int i = 0; i < choice2; ++i)
		if(safeDiscard(card, currentPlayer, state, 1) != -1)
			++numOccur;

	//in case they have less then choice2
	state->supplyCount[card] += numOccur;

	//each other player gains a copy of revealed card
	for(int i = (currentPlayer+1)%state->numPlayers; i != currentPlayer; i = (i+1)%state->numPlayers)
		gainCard(card, state, 0, i);

	//discard played card from hand
	safeDiscard(ambassador, currentPlayer, state, 0);
	return 0;
}

int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos)
{
	state->coins += 2; //+2 coins

	for(int i = 0; i < state->numPlayers; ++i)
	{
		if(i == currentPlayer) //don't process ourselves
			continue;

		//try to discard 1 copper and continue if we found one
		if(safeDiscard(copper, i, state, 0) != -1)
			continue;

		//did we check every card? if safeDiscard == -1 then yes
		#if DEBUG
			for(int j = 0; j < state->handCount[i]; ++j)
				printf("Player %d reveals card number %d\n", i, state->hand[i][j]);
		#endif
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
	for(int i = (currentPlayer+1)%state->numPlayers; i != currentPlayer; i = (i+1)%state->numPlayers)
	{
		//add to discard by taking from deck
		//puts in hand and deals with deck being empty. Dont discard if deck and discard are empty
		if(drawCard(i, state) != -1) //drawed a card so discard it and pop from hand
			PUSH(discard, i, POP_R(hand, i));

		//try to add curse to top of deck
		gainCard(curse, state, 1, i);
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
