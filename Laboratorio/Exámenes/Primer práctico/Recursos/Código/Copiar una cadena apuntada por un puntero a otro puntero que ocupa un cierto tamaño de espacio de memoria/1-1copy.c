#include "1-1copy.h"

int copy(char * source, char * destination, unsigned int lengthDestination)
{
	for(int i = 0; i < lengthDestination; i++)
	{
		*destination = *source;
		destination++;
		source++;
	}
	*destination = '\0';
	return 0;
}
