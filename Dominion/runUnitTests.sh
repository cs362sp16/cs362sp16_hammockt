#!/bin/bash -e

echo "*******************"
echo "* Executing Tests *"
echo "*******************"
echo
echo "*******************"
echo "*   Unit Test 1   *"
echo "*******************"
echo
unittest1
echo
echo "*******************"
echo "*   Unit Test 2   *"
echo "*******************"
echo
unittest2
echo
echo "*******************"
echo "*   Unit Test 3   *"
echo "*******************"
echo
unittest3
echo
echo "*******************"
echo "*   Unit Test 4   *"
echo "*******************"
echo
unittest4
echo
echo "*******************"
echo "*   Card Test 1   *"
echo "*******************"
echo
cardtest1
echo
echo "*******************"
echo "*   Card Test 2   *"
echo "*******************"
echo
cardtest2
echo
echo "*******************"
echo "*   Card Test 3   *"
echo "*******************"
echo
cardtest3
echo
echo "*******************"
echo "*   Card Test 4   *"
echo "*******************"
echo
cardtest4
echo
echo "*******************"
echo "*    Coverage     *"
echo "*******************"
echo
echo "//Cards that were tested were moved from dominion.c into cardEffects.c..."
echo "//so coverage is higher than it should be for dominion"
echo
gcov dominion.c
gcov cardEffects.c
gcov -f dominion | grep "buyCard" -A 2
gcov -f dominion | grep "scoreFor" -A 2
gcov -f dominion | grep "getWinners" -A 2
gcov -f dominion | grep "endTurn" -A 2
gcov -f cardEffects | grep "ambassador" -A 2
gcov -f cardEffects | grep "baron" -A 2
gcov -f cardEffects | grep "salvager" -A 2
gcov -f cardEffects | grep "council" -A 2
echo "*******************"
echo "*       END       *"
echo "*******************"
