#ifndef _MY_ASSERT_H
#define _MY_ASSERT_H

#include <stdio.h>

int myAssert(int r, char* msg, int lineNum)
{
	static int failed = 0;

	if(r == 0)
	{
		printf("FAILED ASSERTION (line#%d): %s\n", lineNum, msg);
		failed = 1;
	}

	return failed;
}

#endif
