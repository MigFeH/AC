#include <stdio.h>
#include "1-1copy.h"
#include <stdlib.h>

int main()
{
	char * source = "abc";
	unsigned int lengthDestination = 3;
	char * destination = (char *)malloc(3);


	copy(source, destination, lengthDestination);
	
	printf("Cadena a copiar: %s; Cadena copiada: %s\n", source, destination);
	
	free(destination);
	return 0;
}
