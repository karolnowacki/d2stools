#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/d2slib.h"

int main(int argc, char *argv[]) {

	FILE *output = stdout;
	D2S_itemformat *item;
	u8 buff[1024];

	int col,row,storedin;

	if (argc != 3) {
		fprintf(stderr,
				"BRAK PARAMETRU\n\n\tPrawidlowe uzycie: %s file.d2s Base64_D2I\n\n",
				argv[0]);
		return 0;
	}

	fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	if (!D2Sbase64decode(argv[2], buff, 1024)) {
		fprintf(output, "<error>Invalid base64 format</error>\n");
		return 0;
	}

	item = D2SbuffToItem(buff, 1024);
	if (item == NULL) {
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		return 0;
	}

	D2S * save = D2Sloadsave(argv[1]);
	if (save == NULL) {
		D2Sitemfree(item);
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		return 0;
	}

	if (!D2SpickupItem(save, item, &col, &row, &storedin)) {
		D2Sitemfree(item);
		D2Sfree(save);
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		return 0;
	}

	if (D2Ssave(argv[1], save)) {
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		D2Sfree(save);
		return 0;
	}

	fprintf(output, "<ok storedin=\"%s\" col=\"%d\" row=\"%d\" />\n",
			storedin == 5 ? "stash" : "inventory",
			col, row);

	D2Sfree(save);
	return 0;
}
