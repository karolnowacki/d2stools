#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/d2slib.h"

int main(int argc, char *argv[]) {

	FILE *output = stdout;
	D2S_itemformat *item;
	u8 buff[1024];
	char xmlbuff[16384];

	if (argc != 2) {
		fprintf(stderr,
				"BRAK PARAMETRU\n\n\tPrawidlowe uzycie: %s Base64_D2I\n\n",
				argv[0]);
		return 0;
	}

	fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	if (!D2Sbase64decode(argv[1], buff, 1024)) {
		fprintf(output, "<error>Invalid base64 format</error>\n");
		return 0;
	}

	item = D2SbuffToItem(buff, 1024);
	if (item == NULL) {
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		return 0;
	}

	D2SitemToXML(xmlbuff, item, -1);

	fprintf(output, "%s", xmlbuff);
	D2Sitemfree(item);
	return 0;
}
