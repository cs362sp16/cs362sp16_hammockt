#include "cardEffects.h"

//everything that adds/messes with coins needs to put it into bonus
//draws until you get 2 treasure cards (copper, silver, or gold). Discards all other cards
int adventureEffect(struct gameState* state, int currentPlayer, int handPos)
{
	for(int drawntreasure = 0; drawntreasure < 2; )
	{
		//draw a card. Automatically puts discard into deck but quit if discard and deck are both empty
		if(drawCard(currentPlayer, state) == -1)
			break;

		int cardDrawn = state->hand[currentPlayer][state->handCount[currentPlayer]-1]; //top card of hand is most recently drawn card.
		if(cardDrawn >= copper && cardDrawn <= gold) //is treasure?
			drawntreasure++;
		else
		{
			state->discard[currentPlayer][state->discardCount[currentPlayer]++] = cardDrawn; //discard all cards in play that have been drawn
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
	//Everyone gets 1 card
	for(int i = 0; i < state->numPlayers; ++i)
		drawCard(i, state);

	//+3 cards for the player. +4 total for player
	for(int i = 0; i < 3; ++i)
		drawCard(currentPlayer, state);

	//+1 Buy
	state->numBuys++;

	//put played card in played card pile
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

//+2 coins, everyone else must dispose 1 copper...if not then reveal hand
//there could be a problem with discard order
int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos)
{
	int j;

	//+2 coins does not work and will need to modify state struct
	updateCoins(currentPlayer, state, 2); //+2 coins
	for(int i = 0; i < state->numPlayers; ++i)
	{
		if(i == currentPlayer) //don't process ourselves
			continue;

		//try to discard 1 copper and stop
		for(j = 0; j < state->handCount[i]; ++j) //can be used for safeDiscard
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
	for(int i = 0; i < state->numPlayers; ++i)
	{
		if(i != currentPlayer)
		{
			//add to discard by taking from deck
			//puts in hand and deals with deck being empty. Dont discard if deck and discard are empty
			if(drawCard(i, state) != -1) //drawed a card so discard it and pop from hand
				state->discard[i][state->discardCount[i]++] = state->hand[i][--state->handCount[i]];

			//try to add curse to top of deck
			gainCard(curse, state, 1, i);
		}
	}

	//discard played card from hand
	discardCard(handPos, currentPlayer, state, 0);

	return 0;
}

//trash this and another copy from hand. if success then add 4 gold coins to deck
//problem here for discard order
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
	discardCard(handPos, currentPlayer, state, 1);

	return 0;
}
