#ifndef _CARD_EFFECTS_H
#define _CARD_EFFECTS_H

#include "dominion.h"
#include "dominion_helpers.h"

//draws until you get 2 treasure cards (copper, silver, or gold). Discards all other cards
int adventurerEffect(struct gameState* state, int currentPlayer, int handPos);

//draw 4 for player, then 1 for everyone else
int councilRoomEffect(struct gameState* state, int currentPlayer, int handPos);

//gain card with cost up to 5
int feastEffect(struct gameState* state, int currentPlayer, int handPos, int choice1);

//trash treasure card and get another that costs up to 3 more
int mineEffect(struct gameState* state, int currentPlayer, int handPos, int choice1, int choice2);

//trash card from hand and get one that costs 3 more
int remodelEffect(struct gameState* state, int currentPlayer, int handPos, int choice1, int choice2);

//draw 3 cards
int smithyEffect(struct gameState* state, int currentPlayer, int handPos);

//draw 1 card, +2 actions
int villageEffect(struct gameState* state, int currentPlayer, int handPos);

//+1 buy. discard estate and get +4 coins...or gain an estate
int baronEffect(struct gameState* state, int currentPlayer, int handPos, int choice1);

//draw 1 card, +1 action
int greatHallEffect(struct gameState* state, int currentPlayer, int handPos);

//+1 action. get 2 coins or discard hand, draw 4, then everyone with >= 5 cards discards and draws 4 cards
int minionEffect(struct gameState* state, int currentPlayer, int handPos, int choice1);

//choose one: draw 2 cards, +2 coins, or trash 2 cards from hand
int stewardEffect(struct gameState* state, int currentPlayer, int handPos, int choice1, int choice2, int choice3);

//next player reveals/discards top 2 cards of deck. For each different card get...
//+2 actions for action card, +2 coins for treasure cards, +2 cards for victory cards
int tributeEffect(struct gameState* state, int currentPlayer, int handPos);

//reveal a card from hand, return up to 2 of it from hand to supply, then each other player gains a copy of it
int ambassadorEffect(struct gameState* state, int currentPlayer, int handPos, int choice1, int choice2);

//+2 coins, everyone else must dispose 1 copper...if not then reveal hand
int cutpurseEffect(struct gameState* state, int currentPlayer, int handPos);

//+2 coins and place embargo token on a supply pile
int embargoEffect(struct gameState* state, int currentPlayer, int handPos, int choice1);

//gives an extra turn (only one though)
int outpostEffect(struct gameState* state, int currentPlayer, int handPos);

//+1 buy. Must trash a card but get coins equal to its cost
int salvagerEffect(struct gameState* state, int currentPlayer, int handPos, int choice1);

//every other player discards top card of deck. Then replaces it with a curse
int seaHagEffect(struct gameState* state, int currentPlayer, int handPos);

//trash this and another copy from hand. if success then add 4 gold coins to deck
int treasureMapEffect(struct gameState* state, int currentPlayer, int handPos);

#endif
