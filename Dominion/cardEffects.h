#ifndef _CARD_EFFECTS_H
#define _CARD_EFFECTS_H

#include "dominion.h"
#include "dominion_helpers.h"

//draws until you get 2 treasure cards (copper, silver, or gold). Discards all other cards
int adventureEffect(struct gameState* state, int currentPlayer, int handPos);

//draw 4 for player, then 1 for everyone else
int councilRoomEffect(struct gameState* state, int currentPlayer, int handPos);

//+2 coins, everyone else must dispose 1 copper...if not then reveal hand
int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos);

//every other player discards top card of deck. Then replaces it with a curse
int seaHagEffect(struct gameState* state, int currentPlayer, int handPos);

//trash this and another copy from hand. if success then add 4 gold coins to deck
int treasureMapEffect(struct gameState* state, int currentPlayer, int handPos);

#endif
