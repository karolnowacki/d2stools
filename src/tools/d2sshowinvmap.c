#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/d2slib.h"

int main(int argc, char *argv[]) {

	int inventory[10][4], stash[6][8], fs, i, j,c,r,s;

	if (argc != 2) {
		fprintf(stderr,
				"BRAK PARAMETRU\n\n\tPrawidlowe uzycie: %s file.d2s\n\n",
				argv[0]);
		return 0;
	}

	D2S * save = D2Sloadsave(argv[1]);
	if (save == NULL) {
		fprintf(stderr, "ERROR: %s\n", D2SgetLastError());
		return 0;
	}

	fs = D2SspaceMap(save, inventory, stash);

	printf("INVENTORY:\n+");
	for (i=0; i<10; ++i)
		printf("-");
	printf("+\n");
	for (j=0; j<4; ++j) {
		printf("|");
		for (i=0; i<10; ++i)
			printf("%c", inventory[i][j] == -1 ? '.' : 'X');
		printf("|\n");
	}
	printf("+");
	for (i=0; i<10; ++i)
			printf("-");
	printf("+\n\nSTASH:\n+");
	for (i=0; i<6; ++i)
		printf("-");
	printf("+\n");
	for (j=0; j<8; ++j) {
		printf("|");
		for (i=0; i<6; ++i)
			printf("%c", stash[i][j] == -1 ? '.' : 'X');
		printf("|\n");
	}
	printf("+");
	for (i=0; i<6; ++i)
		printf("-");
	printf("+\n\nFREE SPACE: %d\n\n", fs);

	for (i=1; i<5; i++)
		for (j=1; j<5; j++)
			if (D2SfindFreeSpace(save, i, j, &c, &r, &s))
				printf("FreeSpace %dx%d: %s %dx%d\n", i, j, s == 1 ? "inv" : "stash", c, r);

	D2Sfree(save);

	return 0;
}
