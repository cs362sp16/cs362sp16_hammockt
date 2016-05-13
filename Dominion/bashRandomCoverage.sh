#!/bin/bash -e

echo
echo "*******************"
echo "*    Coverage     *"
echo "*******************"
echo
gcov dominion.c dominion_helpers.c cardEffects.c
echo
gcov -fb cardEffects | grep $1 -A 2
echo
echo "*******************"
echo "*       END       *"
echo "*******************"
