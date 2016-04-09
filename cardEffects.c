#include "cardEffects.h"

//everything that adds/messes with coins needs to put it into bonus
//draws until you get 2 treasure cards (copper, silver, or gold). Discards all other cards
int adventureEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int drawntreasure = 0, cardDrawn;

	while(drawntreasure < 2)
	{
		drawCard(currentPlayer, state); //draw a card. Automatically puts discard into deck

		cardDrawn = state->hand[currentPlayer][state->handCount[currentPlayer]-1];//top card of hand is most recently drawn card.
		if(cardDrawn == copper || cardDrawn == silver || cardDrawn == gold) //can optimize this but could lead to undefined
			drawntreasure++;
		else //this assumes first drawn cards are discarded first. Not last drawn cards are discarded first
		{
			state->discard[currentPlayer][state->discardCount[currentPlayer]++] = cardDrawn; // discard all cards in play that have been drawn
			state->handCount[currentPlayer]--; //this should just remove the top card (the most recently drawn one).
		}
	}

	//put played card in discard pile
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

//did draw 4 for player, then 1 for everyone else
int councilRoomEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int i;

	//Everyone gets 1 card
	for(i = 0; i < state->numPlayers; ++i)
		drawCard(i, state);

	//+3 cards for the player. +4 total for player
	for(i = 0; i < 3; ++i)
		drawCard(currentPlayer, state);

	//+1 Buy
	state->numBuys++;

	//put played card in played card pile
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

//+2 coins, everyone else must dispose 1 copper...if not then reveal hand
int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int i, j;

	updateCoins(currentPlayer, state, 2); //+2 coins
	for(i = 0; i < state->numPlayers; ++i)
	{
		if(i == currentPlayer) //don't process ourselves
			continue;

		for(j = 0; j < state->handCount[i]; ++j)
		{
			if(state->hand[i][j] == copper)
			{
				discardCard(j, i, state, 0); //handPos, currentPlayer
				break;
			}
		}

		//did we check every card? if yes then j will be...
		if(j == state->handCount[i])
		{
			for(j = 0; j < state->handCount[i]; ++j) //can do optimize here but code might set (handCount < 0)
			{
				#if DEBUG
					printf("Player %d reveals card number %d\n", i, state->hand[i][j]);
				#endif
			}
		}
	}

	//discard played card from hand
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

//every other player discards top card of deck. Then replaces it with a curse
int seaHagEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int i;

	for(i = 0; i < state->numPlayers; ++i)
	{
		if(i != currentPlayer)
		{
			//add to discard by taking from deck
			state->discard[i][state->discardCount[i]++] = state->deck[i][state->deckCount[i]-1];
			//replace top deck card with curse. Count remains the same
			state->deck[i][state->deckCount[i]-1] = curse;//Top card now a curse
		}
	}

	//discard played card from hand
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

//trash this and another copy from hand. if success then add 4 gold coins to deck
int treasureMapEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int i, index = -1;

	//search hand for another treasure_map
	for(i = 0; i < state->handCount[currentPlayer]; ++i)
	{
		if(i != handPos && state->hand[currentPlayer][i] == treasure_map)
		{
			index = i;
			break;
		}
	}

	//trash this treasure card
	discardCard(handPos, currentPlayer, state, 1);

	//index is only set when when we find a treasure_map. Maybe trash before this
	if(index != -1)
	{
		//trash other treasure card
		discardCard(index, currentPlayer, state, 1);

		//gain 4 Gold cards
		for(i = 0; i < 4; ++i)
			gainCard(gold, state, 1, currentPlayer);

		//return success
		return 1;
	}

	//no second treasure_map found in hand
	return -1;
}