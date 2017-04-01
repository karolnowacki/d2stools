#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/d2slib.h"

int main(int argc, char *argv[]) {

	if (argc != 3) {
		fprintf(stderr,
				"BRAK PARAMETRU\n\n\tPrawidlowe uzycie: %s input.d2s output.d2s\n\n",
				argv[0]);
		return 0;
	}

	D2S * save = D2Sloadsave(argv[1]);
	if (save == NULL) {
		fprintf(stderr, "ERROR: %s\n", D2SgetLastError());
		return 0;
	}

	if (D2Ssave(argv[2], save)) {
		fprintf(stderr, "ERROR: %s\n", D2SgetLastError());
		return 0;
	}

	D2Sfree(save);

	return 0;

}
