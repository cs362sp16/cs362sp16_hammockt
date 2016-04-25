#include "dominion.h"
#include "dominion_helpers.h"
#include "rngs.h"
#include "cardEffects.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//modified
static int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

//modified
struct gameState* newGame()
{
	return (struct gameState*)malloc(sizeof(struct gameState));
}

//modified
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

	//check selected kingdom cards are different
	for(int i = 0; i < 9; ++i) //10 - 1
		for(j = i+1; j < 10; ++j)
			if(kingdomCards[j] == kingdomCards[i])
				return -1;

	//initialize supply
	///////////////////////////////

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
	for(int i = adventurer; i <= treasure_map; ++i) //loop all cards
	{
		for(j = 0; j < 10; ++j) //loop chosen cards
		{
			if(kingdomCards[j] == i)
			{
				//check if card is a 'Victory' Kingdom card
				if(kingdomCards[j] == great_hall || kingdomCards[j] == gardens)
					state->supplyCount[i] = startingNum;
				else
					state->supplyCount[i] = 10;

				break;
			}
			else    //card is not in the set choosen for the game
				state->supplyCount[i] = -1;
		}
	}

	////////////////////////
	//supply intilization complete

	//set player decks
	for(int i = 0; i < numPlayers; ++i)
	{
		state->deckCount[i] = 10;
		for(j = 0; j < 3; ++j) //first 3 are estate
			state->deck[i][j] = estate;
		for(; j < 10; ++j) //last 7 are copper
			state->deck[i][j] = copper;
	}

	//shuffle player decks
	for(int i = 0; i < numPlayers; ++i)
		if(shuffle(i, state) < 0)
			return -1;

	//initialize hands and discards size's to zero
	memset(state->handCount, 0, numPlayers * sizeof(int));
	memset(state->discardCount, 0, numPlayers * sizeof(int));

	//set embargo tokens to 0 for all supply piles
	memset(state->embargoTokens, 0, (treasure_map+1) * sizeof(int));

	//initialize first player's turn
	state->outpostPlayed = 0;
	state->phase = 0;
	state->numActions = 1;
	state->coins = 0;
	state->numBuys = 1;
	state->playedCardCount = 0;
	state->whoseTurn = 0;

	//draw cards here, only drawing at the start of a turn
	for(int i = 0; i < numPlayers; ++i)
		for(int j = 0; j < 5; ++j)
			drawCard(i, state);

	return 0;
}

//formatted
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

//modified
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

//formatted
//does not deal with embargo
int buyCard(int supplyPos, struct gameState* state)
{
	//I don't know what to do about the phase thing.
	int totalCoins = updateCoins(state->whoseTurn, state);

	if(state->numBuys < 1)
		return -1;
	else if(supplyCount(supplyPos, state) < 1)
		return -1;
	else if(totalCoins < getCost(supplyPos))
		return -1;
	else
	{
		state->phase = 1;
		gainCard(supplyPos, state, 0, state->whoseTurn); //card goes in discard

		state->coins -= getCost(supplyPos);
		state->numBuys--;
	}

	return 0;
}

//formatted
int numHandCards(struct gameState* state)
{
	return state->handCount[whoseTurn(state)];
}

//formatted
int handCard(int handPos, struct gameState* state)
{
	int currentPlayer = whoseTurn(state);
	return state->hand[currentPlayer][handPos];
}

//modified
int supplyCount(int card, struct gameState* state)
{
	if(card < 0 || card > treasure_map)
		return -1;
	return state->supplyCount[card];
}

//fixed
int fullDeckCount(int player, struct gameState* state)
{
	return state->deckCount[player] + state->handCount[player] + state->discardCount[player];
}

//formatted
int whoseTurn(struct gameState* state)
{
	return state->whoseTurn;
}

static void moveAll(int d[], int s[], int* dc, int* sc)
{
	memcpy(d + *dc, s, *sc * sizeof(int));
	memset(s, -1, *sc * sizeof(int)); //for now keeping this but will remove later on
	*dc += *sc;
	*sc = 0;
}

//modified
int endTurn(struct gameState* state)
{
	int currentPlayer = whoseTurn(state);

	//move hand into discard
	moveAll(state->discard[currentPlayer], state->hand[currentPlayer],
			state->discardCount+currentPlayer, state->handCount+currentPlayer);
	//move playedCards into discard
	moveAll(state->discard[currentPlayer], state->playedCards,
			state->discardCount+currentPlayer, &state->playedCardCount);

	//player draws new hand
	for(int i = 0; i < 5; ++i)
		drawCard(currentPlayer, state);

	//Code for determining the next player
	state->whoseTurn = (currentPlayer + 1) % state->numPlayers;

	state->outpostPlayed = 0;
	state->phase = 0;
	state->numActions = 1;
	state->coins = 0;
	state->numBuys = 1;

	return 0;
}

//modified
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

	if(emptySupply >= 3)
		return 1;

	return 0;
}

static int scoreHelper(int card, int player, struct gameState* state)
{
	switch(card)
	{
		case curse:		 return -1;
		case estate:	 return 1;
		case duchy:		 return 3;
		case province:	 return 6;
		case great_hall: return 1;
		case gardens:	 return fullDeckCount(player, state) / 10;
	}
	return 0; //if none do not do anything
}

//modified
int scoreFor(int player, struct gameState* state)
{
	int score = 0;

	//score from hand
	for(int i = 0; i < state->handCount[player]; ++i)
		score += scoreHelper(state->hand[player][i], player, state);

	//score from discard
	for(int i = 0; i < state->discardCount[player]; ++i)
		score += scoreHelper(state->discard[player][i], player, state);

	//score from deck
	for(int i = 0; i < state->deckCount[player]; ++i)
		score += scoreHelper(state->deck[player][i], player, state);

	return score;
}

//modified
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

//modified. Leaving this in dominion.c for now
//Also must discard high pos cards first. Which is a problem almost everywhere in here
int cardEffect(int card, int choice1, int choice2, int choice3, struct gameState* state, int handPos)
{
	int i, j;
	int currentPlayer = whoseTurn(state);
	int nextPlayer = (currentPlayer + 1) % state->numPlayers;

	//uses switch to select card and perform actions
	switch(card)
	{
		case adventurer:
			return adventureEffect(state, currentPlayer, handPos);

		case council_room:
			return councilRoomEffect(state, currentPlayer, handPos);

		case feast: //gain card with cost up to 5
			//does exist or have supply?
			if(supplyCount(choice1, state) <= 0)
				return -1;

			//Gain the card (in discard) if <= 5
			if(getCost(choice1) <= 5)
				gainCard(choice1, state, 0, currentPlayer);

			//must trash it
			discardCard(handPos, currentPlayer, state, 1);

			return 0;

		case mine: //trash treasure card and get another that costs up to 3 more
			//valid pos in hand
			if(choice1 < 0 || choice1 >= state->handCount[currentPlayer])
				return -1;

			//needs to be a treasure card. If mine is only card then this returns
			if(state->hand[currentPlayer][choice1] < copper || state->hand[currentPlayer][choice1] > gold)
				return -1;

			//need to gain a treasure card (nothing else). Handles out of bounds input. Handles empty supply
			if(supplyCount(choice2, state) <= 0 || choice2 < copper || choice2 > gold)
				return -1;

			//can trade a gold/silver for copper
			if((getCost(choice2) - 3) > getCost(state->hand[currentPlayer][choice1]))
				return -1;

			//puts card in hand
			gainCard(choice2, state, 2, currentPlayer); //don't need to check thanks to choice2 if

			//discard card from hand and trash treasure
			discardCard(handPos, currentPlayer, state, 0);
			discardCard(choice1, currentPlayer, state, 1);

			return 0;

		case remodel: //trash card from hand and get one that costs 3 more
			//valid pos in hand and remodel is not the only card
			if(choice1 < 0 || choice1 >= state->handCount[currentPlayer] || state->handCount[currentPlayer] <= 1)
				return -1;

			//check if that choice2 is available or valid
			if(supplyCount(choice2, state) <= 0)
				return -1;

			//can trade for a lower card (if they want to)
			if((getCost(choice2) - 2) > getCost(state->hand[currentPlayer][choice1]))
				return -1;

			//puts card in discard
			gainCard(choice2, state, 0, currentPlayer);  //don't need to check thanks to choice2 if

			//discard card from hand and trash other
			discardCard(handPos, currentPlayer, state, 0);
			discardCard(choice1, currentPlayer, state, 0);

			return 0;

		case smithy:
			//+3 Cards
			for(i = 0; i < 3; ++i)
				drawCard(currentPlayer, state);

			//discard card from hand
			discardCard(handPos, currentPlayer, state, 0);
			return 0;

		case village:
			//+1 Card
			drawCard(currentPlayer, state);

			//+2 Actions
			state->numActions += 2;

			//discard played card from hand
			discardCard(handPos, currentPlayer, state, 0);
			return 0;

		case baron: //+1 buy. discard estate and get +4 coins...or gain an estate
			state->numBuys++;

			if(choice1) //Boolean true or going to discard an estate
			{
				for(i = 0; i < state->handCount[currentPlayer]; ++i)
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

		case great_hall:
			//+1 Card
			drawCard(currentPlayer, state);

			//+1 Actions
			state->numActions++;

			//discard card from hand
			discardCard(handPos, currentPlayer, state, 0);
			return 0;

		case minion: //what if choice1 is not 1 or 2
			//+1 action
			state->numActions++;

			//discard card from hand
			discardCard(handPos, currentPlayer, state, 0);

			if(choice1 == 1) //+2 coins
			{
				state->coins += 2;
				return 0;
			}

			//discard hand, redraw 4, other players with 5+ cards discard hand and draw 4
			while(state->handCount[currentPlayer] > 0)
				discardCard(0, currentPlayer, state, 0);

			//draw 4
			for(i = 0; i < 4; ++i)
				drawCard(currentPlayer, state);

			//other players discard hand and redraw if hand size >= 5
			for(i = 0; i < state->numPlayers; ++i)
			{
				if(i != currentPlayer && state->handCount[i] >= 5)
				{
					//discard hand
					while(state->handCount[i] > 0)
						discardCard(0, i, state, 0);

					//draw 4
					for(j = 0; j < 4; ++j)
						drawCard(i, state);
				}
			}

			return 0;

		case steward: //there is a problem here possibly with how cards need to be discarded
			if(choice1 == 1) //+2 cards
			{
				drawCard(currentPlayer, state);
				drawCard(currentPlayer, state);
			}
			else if(choice1 == 2) //+2 coins
				state->coins += 2;
			else //need to check choice2 and choice3 here
			{
				//trash 2 cards in hand. Can trash 1 (or 0?) if have less than 2
				discardCard(choice2, currentPlayer, state, 1); //removes cards from play
				discardCard(choice3, currentPlayer, state, 1);
			}

			//discard card from hand
			discardCard(handPos, currentPlayer, state, 0);

			return 0;

		case tribute: //some code duplication here but works
			j = state->handCount[nextPlayer]; //pos of interest
			for(i = 0; i < 2; ++i) // try to draw/reveal 2 cards
				drawCard(nextPlayer, state);

			//handle drawing 2 identical cards. Need to discard [j+1] from hand if true
			if(state->handCount[nextPlayer] == (j+2) && state->hand[nextPlayer][j] == state->hand[nextPlayer][j+1])
				PUSH(discard, nextPlayer, POP_R(hand, nextPlayer));

			//decrementing so I can process and remove at the same time
			for(i = state->handCount[nextPlayer]-1; i >= j; --i)
			{
				//problem here, cards can be multiple types
				if(isTreasure(state->hand[nextPlayer][i])) //Treasure cards
					state->coins += 2;
				else if(isVictory(state->hand[nextPlayer][i])) //Victory cards
				{
					drawCard(currentPlayer, state);
					drawCard(currentPlayer, state);
				}
				else //Action Card
					state->numActions += 2;

				//discard the processed card. Its the top card so we can pop it
				PUSH(discard, nextPlayer, POP_R(hand, nextPlayer));
			}

			//discard played card from hand
			discardCard(handPos, currentPlayer, state, 0);

			return 0;

		case ambassador:
			j = 0;		//used to check if player has enough cards to discard

			if(choice2 > 2 || choice2 < 0 || choice1 == handPos)
				return -1;

			for(i = 0; i < state->handCount[currentPlayer]; i++)
				if(i != handPos && i == state->hand[currentPlayer][choice1] && i != choice1)
					j++;

			if(j < choice2)
				return -1;

			if(DEBUG)
				printf("Player %d reveals card number: %d\n", currentPlayer, state->hand[currentPlayer][choice1]);

			//increase supply count for choosen card by amount being discarded
			state->supplyCount[state->hand[currentPlayer][choice1]] += choice2;

			//each other player gains a copy of revealed card
			for(i = 0; i < state->numPlayers; i++)
				if(i != currentPlayer)
					gainCard(state->hand[currentPlayer][choice1], state, 0, i);

			//discard played card from hand
			discardCard(handPos, currentPlayer, state, 0);

			//trash copies of cards returned to supply
			for(j = 0; j < choice2; j++)
			{
				for(i = 0; i < state->handCount[currentPlayer]; i++)
				{
					if(state->hand[currentPlayer][i] == state->hand[currentPlayer][choice1])
					{
						discardCard(i, currentPlayer, state, 1);
						break;
					}
				}
			}

			return 0;

		case cutpurse:
			return cutpurseEffect(state, currentPlayer, handPos);

		case embargo:
			//+2 Coins
			state->coins += 2;

			//see if selected pile is in play. This is a bug
			if(state->supplyCount[choice1] == -1)
				return -1;

			//add embargo token to selected supply pile
			state->embargoTokens[choice1]++;

			//trash card
			discardCard(handPos, currentPlayer, state, 1);
			return 0;

		case outpost:
			//set outpost flag
			state->outpostPlayed++;

			//discard card
			discardCard(handPos, currentPlayer, state, 0);
			return 0;

		case salvager: //if choice is less than salvager then discard problem
			//+1 buy
			state->numBuys++;

			if(choice1)
			{
				//gain coins equal to trashed card
				state->coins += getCost(handCard(choice1, state));
				//trash card
				discardCard(choice1, currentPlayer, state, 1);
			}

			//discard card
			discardCard(handPos, currentPlayer, state, 0);
			return 0;

		case sea_hag:
			return seaHagEffect(state, currentPlayer, handPos);

		case treasure_map:
			return treasureMapEffect(state, currentPlayer, handPos);
	}

	return -1;
}

//end of dominion.c
