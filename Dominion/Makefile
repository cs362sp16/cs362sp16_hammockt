CFLAGS = -std=c99 -Wall -fpic -coverage -lm

%.o: %.c %.h
	gcc -c $< -g $(CFLAGS)

dominion.o: dominion.h dominion.c rngs.o cardEffects.o dominion_helpers.o
	gcc -c dominion.c -g $(CFLAGS)

playdom: dominion.o playdom.c
	gcc -o playdom playdom.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

player: interface.o dominion.o player.c
	gcc -o player player.c -g  dominion.o rngs.o interface.o cardEffects.o dominion_helpers.o $(CFLAGS)

all: playdom player

unittest1: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

unittest2: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

unittest3: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

unittest4: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

cardtest1: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

cardtest2: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

cardtest3: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

cardtest4: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

unittestresults.out: unittest1 unittest2 unittest3 unittest4 cardtest1 cardtest2 cardtest3 cardtest4
	bash runUnitTests.sh > $@

clean:
	rm -f *.o playdom player unittest1 unittest2 unittest3 unittest4 cardtest1 cardtest2 cardtest3 cardtest4 *.gcov *.gcda *.gcno *.so *.a *.dSYM
