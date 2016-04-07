#ifndef _CARD_EFFECTS_H
#define _CARD_EFFECTS_H

#include "dominion.h"
#include "dominion_helpers.h"

int adventureEffect(struct gameState* state, int currentPlayer, int handPos);
int councilRoomEffect(struct gameState* state, int currentPlayer, int handPos);
int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos);
int seaHagEffect(struct gameState* state, int currentPlayer, int handPos);
int treasureMapEffect(struct gameState* state, int currentPlayer, int handPos);

#endif