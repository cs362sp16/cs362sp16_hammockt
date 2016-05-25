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

unittest%: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

cardtest%: dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

unittestresults.out: unittest1 unittest2 unittest3 unittest4 cardtest1 cardtest2 cardtest3 cardtest4
	bash runUnitTests.sh > $@

randomtestadventurer.out: dominion.o
	gcc -o randomtestadventurer randomtestadventurer.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)
	randomtestadventurer 42 1 > $@
	bash bashRandomCoverage.sh adventurer >> $@

randomtestcard1.out: dominion.o
	gcc -o randomtestcard1 randomtestcard1.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)
	randomtestcard1 42 1 > $@
	bash bashRandomCoverage.sh baron >> $@

randomtestcard2.out: dominion.o
	gcc -o randomtestcard2 randomtestcard2.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)
	randomtestcard2 42 1 > $@
	bash bashRandomCoverage.sh seaHag >> $@

testdominion: testdominion.c dominion.o
	gcc -o $@ $@.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

testdominion.out: testdominion
	./testdominion 42 > $@

clean:
	rm -f *.o *.out playdom player unittest1 unittest2 unittest3 unittest4 cardtest*[^.c] randomtestadventurer randomtestcard*[^.c] testdominion *.gcov *.gcda *.gcno *.so *.a *.dSYM
