#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/d2slib.h"

int main(int argc, char *argv[]) {

	FILE *output = stdout;
	D2S_itemformat *item;
	int itemkey;
	char buff[16384];

	if (argc != 3) {
		fprintf(stderr,
				"BRAK PARAMETRU\n\n\tPrawidlowe uzycie: %s file.d2s ITEM_KEY\n\n",
				argv[0]);
		return 0;
	}

	fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	D2S * save = D2Sloadsave(argv[1]);
	if (save == NULL) {
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		return 0;
	}

	itemkey = atoi(argv[2]);

	item = D2SdropItem(save->itemlist, itemkey);
	if (item == NULL) {
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		D2Sfree(save);
		return 0;
	}

	D2SitemToXML(buff, item, -1);

	if (D2Ssave(argv[1], save)) {
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		D2Sitemfree(item);
		D2Sfree(save);
		return 0;
	}

	fprintf(output, "%s\n", buff);

	D2Sitemfree(item);
	D2Sfree(save);
	return 0;
}

