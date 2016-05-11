#include "dominion_helpers.h"
#include <string.h>

int drawCard(int player, struct gameState* state)
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

		state->deckCount[player] = state->discardCount[player];
		state->discardCount[player] = 0;//Reset discard

		//Shuffle the deck up and make it so that we can draw
		shuffle(player, state);
	}

	//Draw the Card
	PUSH(hand, player, POP_R(deck, player));

	return 0;
}

void drawCards(int player, struct gameState* state, int numCards)
{
	for(int i = 0; i < numCards; ++i)
		drawCard(player, state);
}

int updateCoins(int player, struct gameState* state)
{
	int totalCoins = 0;

	for(int i = 0; i < state->handCount[player]; ++i)
	{
		switch(state->hand[player][i])
		{
			case copper: totalCoins++; break;
			case silver: totalCoins += 2; break;
			case gold:   totalCoins += 3; break;
		}
	}

	totalCoins += state->coins;

	return totalCoins;
}

int discardCard(int handPos, int currentPlayer, struct gameState* state, int trashFlag)
{
	//if card is not trashed, add to Played pile
	if(trashFlag < 1)
		state->playedCards[state->playedCardCount++] = state->hand[currentPlayer][handPos];

	//remove card from player's hand
	POP(hand, currentPlayer);
	//replace discarded card with last card in hand if not last
	if(handPos != state->handCount[currentPlayer])
		state->hand[currentPlayer][handPos] = state->hand[currentPlayer][state->handCount[currentPlayer]]; //safe cause dec

	return 0;
}

//most calls to discardCard are unsafe after the first, this fixes that
int safeDiscard(int card, int currentPlayer, struct gameState* state, int trashFlag)
{
	for(int i = 0; i < state->handCount[currentPlayer]; ++i)
		if(state->hand[currentPlayer][i] == card)
			return discardCard(i, currentPlayer, state, trashFlag);

	return -1;
}

int gainCard(int supplyPos, struct gameState* state, int toFlag, int player)
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
		case 0: PUSH(discard, player, supplyPos); break;
		case 1: PUSH(deck, player, supplyPos);    break;
		case 2: PUSH(hand, player, supplyPos);    break;
	}

	//decrease number in supply pile
	state->supplyCount[supplyPos]--;

	return 0;
}

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

int isVictory(int card)
{
	switch(card)
	{
		case estate:
		case duchy:
		case province:
		case great_hall:
		case gardens: return 1;
	}

	return 0;
}

int isTreasure(int card)
{
	return (card >= copper && card <= gold);
}

int isBasic(int card)
{
	switch(card)
	{
		case copper:
		case silver:
		case gold:

		case estate:
		case duchy:
		case province:

		case curse: return 1;	
	}

	return 0;
}

int isKingdom(int card)
{
	return (!isBasic(card) && card >= curse && card <= treasure_map);
}

int isAction(int card)
{
	return (card >= adventurer && card <= treasure_map && card != gardens);
}

void moveAll(int d[], int s[], int* dc, int* sc)
{
	memcpy(d + *dc, s, *sc * sizeof(int));
	*dc += *sc;
	*sc = 0;
}

void moveFromOffset(int d[], int s[], int* dc, int* sc, int offset)
{
	int numToMove = *sc - offset;

	memcpy(d + *dc, s + offset, numToMove * sizeof(int));
	*dc += numToMove;
	*sc -= numToMove;
}
