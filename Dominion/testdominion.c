#include <stdlib.h>
#include <stdio.h>
#include "randomGame.h"

int main(int argc, char* args[])
{
	if(argc != 2)
	{
		printf("Usage: testdominion seed\n");
		return -1;
	}

	randomGame(atoi(args[1]));

	return 0;
}
