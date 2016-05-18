#include "dominion.h"
#include "dominion_helpers.h"
#include "rngs.h"
#include "cardEffects.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

struct gameState* newGame()
{
	return (struct gameState*)malloc(sizeof(struct gameState));
}

int initializeGame(int numPlayers, int kingdomCards[10], int randomSeed, struct gameState* state)
{
	int j;
	//set up random number generator
	SelectStream(1);
	PutSeed((long)randomSeed);

	//check number of players
	if(numPlayers > MAX_PLAYERS || numPlayers < 2)
		return -1;

	//set number of players
	state->numPlayers = numPlayers;

	//check that kingdom cards are actually kingdom cards
	//check selected kingdom cards are different
	for(int i = 0; i < 10; ++i)
	{
		if(!isKingdom(kingdomCards[i]))
			return -1;
		for(j = i+1; j < 10; ++j)
			if(kingdomCards[j] == kingdomCards[i])
				return -1;
	}

	//initialize supply
	///////////////////////////////
	memset(state->supplyCount, -1, sizeof(state->supplyCount));

	//set number of Curse cards. f(x) = 10x - 10, so long as x > 1 (which we have)
	//f(2) = 10, f(3) = 20, f(4) = 30...
	state->supplyCount[curse] = 10*numPlayers - 10;

	int startingNum = (numPlayers == 2)? 8: 12;
	//set number of Victory cards
	state->supplyCount[estate] = startingNum;
	state->supplyCount[duchy] = startingNum;
	state->supplyCount[province] = startingNum;

	//set number of Treasure cards
	state->supplyCount[copper] = 60 - (7 * numPlayers);
	state->supplyCount[silver] = 40;
	state->supplyCount[gold] = 30;

	//set number of Kingdom cards
	for(int i = 0; i < 10; ++i)
	{
		int kCard = kingdomCards[i];
		//victory kingdom card?
		state->supplyCount[kCard] = (isVictory(kCard))? startingNum: 10;
	}

	////////////////////////
	//supply intilization complete

	//set player decks
	state->deckCount[0] = 10;
	for(j = 0; j < 3; ++j) //first 3 are estate
		state->deck[0][j] = estate;
	for(; j < 10; ++j) //last 7 are copper
		state->deck[0][j] = copper;
	for(int i = 1; i < numPlayers; ++i)
	{
		state->deckCount[i] = 10;
		memcpy(state->deck[i], state->deck[0], state->deckCount[i] * sizeof(int));
	}

	//shuffle player decks
	for(int i = 0; i < numPlayers; ++i)
		if(shuffle(i, state) < 0)
			return -1;

	//initialize hands and discards size's to zero
	memset(state->handCount, 0, numPlayers * sizeof(int));
	memset(state->discardCount, 0, numPlayers * sizeof(int));

	//set embargo tokens to 0 for all supply piles
	memset(state->embargoTokens, 0, sizeof(state->embargoTokens));

	//initialize first player's turn
	state->outpostTurn = 0;
	state->outpostPlayed = 0;
	state->phase = 0;
	state->numActions = 1;
	state->coins = 0;
	state->numBuys = 1;
	state->playedCardCount = 0;
	state->whoseTurn = 0;

	//draw cards here, only drawing at the start of a turn
	for(int i = 0; i < numPlayers; ++i)
		drawCards(i, state, 5);

	return 0;
}

int shuffle(int player, struct gameState* state)
{
	int newDeck[MAX_DECK];
	int newDeckPos = 0;

	if(state->deckCount[player] < 1)
		return -1;

	qsort((void*)(state->deck[player]), state->deckCount[player], sizeof(int), compare);
	/* SORT CARDS IN DECK TO ENSURE DETERMINISM! */

	while(state->deckCount[player] > 0)
	{
		int card = floor(Random() * state->deckCount[player]);
		newDeck[newDeckPos] = state->deck[player][card];
		newDeckPos++;
		for(int i = card; i < state->deckCount[player]-1; ++i)
			state->deck[player][i] = state->deck[player][i+1];

		state->deckCount[player]--;
	}

	memcpy(state->deck[player], newDeck, newDeckPos * sizeof(int));
	state->deckCount[player] = newDeckPos;

	return 0;
}

int playCard(int handPos, int choice1, int choice2, int choice3, struct gameState* state)
{
	//check if it is the right phase or player has enough actions
	if(state->phase != 0 || state->numActions < 1)
		return -1;

	//get card played
	int card = handCard(handPos, state);

	//check if selected card is an action
	if(card < adventurer || card > treasure_map) //good idea?
		return -1;

	//play card
	if(cardEffect(card, choice1, choice2, choice3, state, handPos) < 0)
		return -1;

	//reduce number of actions
	state->numActions--;

	return 0;
}

int buyCard(int supplyPos, struct gameState* state)
{
	//I don't know what to do about the phase thing. Should check phase
	int totalCoins = updateCoins(state->whoseTurn, state);

	if(state->numBuys < 1 || supplyCount(supplyPos, state) < 1 || totalCoins < getCost(supplyPos))
		return -1;
	else
	{
		state->phase = 1;
		gainCard(supplyPos, state, 0, state->whoseTurn); //card goes in discard
		for(int i = 0; i < state->embargoTokens[supplyPos]; ++i) //can maybe optimize when curses run out
			gainCard(curse, state, 0, state->whoseTurn);

		state->coins -= getCost(supplyPos);
		state->numBuys--;
	}

	return 0;
}

int numHandCards(struct gameState* state)
{
	return state->handCount[whoseTurn(state)];
}

int handCard(int handPos, struct gameState* state)
{
	int currentPlayer = whoseTurn(state);
	return state->hand[currentPlayer][handPos];
}

int supplyCount(int card, struct gameState* state)
{
	if(card < 0 || card > treasure_map)
		return -1;
	return state->supplyCount[card];
}

int fullDeckCount(int player, struct gameState* state)
{
	return state->deckCount[player] + state->handCount[player] + state->discardCount[player];
}

int whoseTurn(struct gameState* state)
{
	return state->whoseTurn;
}

//outpostPlayed is only set if played on non-extra turn
int endTurn(struct gameState* state)
{
	int currentPlayer = whoseTurn(state);

	//move hand into discard
	moveAll(state->discard[currentPlayer], state->hand[currentPlayer],
			state->discardCount+currentPlayer, state->handCount+currentPlayer);
	//move playedCards into discard
	moveAll(state->discard[currentPlayer], state->playedCards,
			state->discardCount+currentPlayer, &state->playedCardCount);

	//trashed outpost for performance so put it back into playedCards
	if(state->outpostPlayed)
		state->playedCards[state->playedCardCount++] = outpost;

	if(isGameOver(state))
	{
		//playedCards needs to be empty when the game is over
		if(state->outpostPlayed)
			PUSH(discard, currentPlayer, state->playedCards[--state->playedCardCount]);
		return 0;
	}

	if(!state->outpostPlayed)
	{
		//player draws new hand
		drawCards(currentPlayer, state, 5);

		//Code for determining the next player
		state->whoseTurn = (currentPlayer + 1) % state->numPlayers;
		state->outpostTurn = 0;
	}
	else
	{
		drawCards(currentPlayer, state, 3);
		state->outpostTurn = 1;
	}

	state->outpostPlayed = 0;
	state->phase = 0;
	state->numActions = 1;
	state->coins = 0;
	state->numBuys = 1;

	return 0;
}

int isGameOver(struct gameState* state)
{
	int emptySupply = 0;

	//if stack of Province cards is empty, the game ends
	if(state->supplyCount[province] == 0)
		return 1;

	//if three supply pile are at 0, the game ends
	for(int i = 0; i <= treasure_map; ++i) //this is dangerous here! needs var
		if(state->supplyCount[i] == 0)
			emptySupply++;

	return (emptySupply >= 3);
}

static int scoreHelper(int card, int gardenVal)
{
	switch(card)
	{
		case curse:		 return -1;
		case estate:	 return 1;
		case duchy:		 return 3;
		case province:	 return 6;
		case great_hall: return 1;
		case gardens:	 return gardenVal;
	}
	return 0; //if none do not do anything
}

int scoreFor(int player, struct gameState* state)
{
	int score = 0, gardenVal = fullDeckCount(player, state) / 10;

	if(player < 0 || player >= state->numPlayers)
		return -9999;

	//score from hand
	for(int i = 0; i < state->handCount[player]; ++i)
		score += scoreHelper(state->hand[player][i], gardenVal);

	//score from discard
	for(int i = 0; i < state->discardCount[player]; ++i)
		score += scoreHelper(state->discard[player][i], gardenVal);

	//score from deck
	for(int i = 0; i < state->deckCount[player]; ++i)
		score += scoreHelper(state->deck[player][i], gardenVal);

	return score;
}

int getWinners(int players[MAX_PLAYERS], struct gameState* state)
{
	int i, highScore = -9999, lessTurns = 0;

	//get score for each player and set unused player scores to -9999
	for(i = 0; i < state->numPlayers; ++i)
	{
		players[i] = scoreFor(i, state);
		if(players[i] > highScore)
			highScore = players[i];
	}

	//can remove now thanks to scoreFor
	for(; i < MAX_PLAYERS; ++i)
		players[i] = -9999;

	//add 1 to players who had less turns
	for(i = whoseTurn(state)+1; i < state->numPlayers; ++i)
	{
		if(players[i] == highScore)
		{
			players[i]++;
			lessTurns = 1; //set to true
		}
	}

	if(lessTurns)
		highScore++;

	//set winners in array to 1 and rest to 0
	for(i = 0; i < state->numPlayers; ++i)
		players[i] = (players[i] == highScore);

	return 0;
}

//Leaving this in dominion.c for now
int cardEffect(int card, int choice1, int choice2, int choice3, struct gameState* state, int handPos)
{
	int currentPlayer = whoseTurn(state);

	//uses switch to select card and perform actions
	switch(card)
	{
		case adventurer:
			return adventurerEffect(state, currentPlayer, handPos);

		case council_room:
			return councilRoomEffect(state, currentPlayer, handPos);

		case feast:
			return feastEffect(state, currentPlayer, handPos, choice1);

		case mine:
			return mineEffect(state, currentPlayer, handPos, choice1, choice2);

		case remodel:
			return remodelEffect(state, currentPlayer, handPos, choice1, choice2);

		case smithy:
			return smithyEffect(state, currentPlayer, handPos);

		case village:
			return villageEffect(state, currentPlayer, handPos);

		case baron:
			return baronEffect(state, currentPlayer, handPos, choice1);

		case great_hall:
			return greatHallEffect(state, currentPlayer, handPos);

		case minion:
			return minionEffect(state, currentPlayer, handPos, choice1);

		case steward:
			return stewardEffect(state, currentPlayer, handPos, choice1, choice2, choice3);

		case tribute:
			return tributeEffect(state, currentPlayer, handPos);

		case ambassador:
			return ambassadorEffect(state, currentPlayer, handPos, choice1, choice2);

		case cutpurse:
			return cutpurseEffect(state, currentPlayer, handPos);

		case embargo:
			return embargoEffect(state, currentPlayer, handPos, choice1);

		case outpost:
			return outpostEffect(state, currentPlayer, handPos);

		case salvager:
			return salvagerEffect(state, currentPlayer, handPos, choice1);

		case sea_hag:
			return seaHagEffect(state, currentPlayer, handPos);

		case treasure_map:
			return treasureMapEffect(state, currentPlayer, handPos);
	}

	return -1;
}

//end of dominion.c
