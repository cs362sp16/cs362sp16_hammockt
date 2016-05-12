#ifndef _CARD_EFFECTS_H
#define _CARD_EFFECTS_H

#include "dominion.h"
#include "dominion_helpers.h"

//draws until you get 2 treasure cards (copper, silver, or gold). Discards all other cards
int adventurerEffect(struct gameState* state, int currentPlayer, int handPos);

//draw 4 for player, then 1 for everyone else
int councilRoomEffect(struct gameState* state, int currentPlayer, int handPos);

//+1 buy. discard estate and get +4 coins...or gain an estate
int baronEffect(struct gameState* state, int currentPlayer, int handPos, int choice1);

//next player reveals/discards top 2 cards of deck. For each different card get...
//+2 actions for action card, +2 coins for treasure cards, +2 cards for victory cards
int tributeEffect(struct gameState* state, int currentPlayer, int handPos);

//reveal a card from hand, return up to 2 of it from hand to supply, then each other player gains a copy of it
int ambassadorEffect(struct gameState* state, int currentPlayer, int handPos, int choice1, int choice2);

//+2 coins, everyone else must dispose 1 copper...if not then reveal hand
int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos);

//+1 buy. Must trash a card but get coins equal to its cost
int salvagerEffect(struct gameState* state, int currentPlayer, int handPos, int choice1);

//every other player discards top card of deck. Then replaces it with a curse
int seaHagEffect(struct gameState* state, int currentPlayer, int handPos);

//trash this and another copy from hand. if success then add 4 gold coins to deck
int treasureMapEffect(struct gameState* state, int currentPlayer, int handPos);

#endif
