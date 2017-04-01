#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/d2slib.h"

int main(int argc, char *argv[]) {
	int i;
	FILE *output = stdout;
	char buff[16384];

	if (argc != 2) {
		fprintf(stderr,
				"BRAK PARAMETRU\n\n\tPrawidlowe uzycie: %s save.d2s\n\n",
				argv[0]);
		return 0;
	}

	fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	D2S * save = D2Sloadsave(argv[1]);
	if (save == NULL) {
		fprintf(output, "<error>%s</error>\n", D2SgetLastError());
		return 0;
	}

	fprintf(output, "<character");
	fprintf(output, " name=\"%s\"", save->header->characterName);

	fprintf(output, " newcharacter=\"%d\"", save->header->characterType.flags.newcharacter);
	fprintf(output, " hardcore=\"%d\"", save->header->characterType.flags.hardcore);
	fprintf(output, " hasdied=\"%d\"", save->header->characterType.flags.hasdied);
	fprintf(output, " expansion=\"%d\"", save->header->characterType.flags.expansion);
	fprintf(output, " ladder=\"%d\"", save->header->characterType.flags.lader);

	fprintf(output, " title=\"%d\"", save->header->characterTitle);
	fprintf(output, " class=\"%d\"", save->header->characterClass);
	fprintf(output, " level=\"%d\"", save->header->level);
	fprintf(output, ">\n");

	fprintf(output, "<stats>\n");
	fprintf(output, "\t<strength>%lu</strength>\n",
			save->stats->strength);
	fprintf(output, "\t<energy>%lu</energy>\n",
			save->stats->energy);
	fprintf(output, "\t<dexterity>%lu</dexterity>\n",
			save->stats->dexterity);
	fprintf(output, "\t<vitality>%lu</vitality>\n",
			save->stats->vitality);
	fprintf(output, "\t<statPoints>%lu</statPoints>\n",
			save->stats->statPoints);
	fprintf(output, "\t<skillPoints>%lu</skillPoints>\n",
			save->stats->skillPoints);

	fprintf(output, "\t<currentLife div=\"256\">%lu</currentLife>\n",
			save->stats->currentLife);
	fprintf(output, "\t<maximumLife div=\"256\">%lu</maximumLife>\n",
			save->stats->maximumLife);
	fprintf(output, "\t<currentMana div=\"256\">%lu</currentMana>\n",
			save->stats->currentMana);
	fprintf(output, "\t<maximumMana div=\"256\">%lu</maximumMana>\n",
			save->stats->maximumMana);
	fprintf(output, "\t<currentStamina div=\"256\">%lu</currentStamina>\n",
			save->stats->currentStamina);
	fprintf(output, "\t<maximumStamina div=\"256\">%lu</maximumStamina>\n",
			save->stats->maximumStamina);

	fprintf(output, "\t<level>%lu</level>\n",
			save->stats->level);
	fprintf(output, "\t<experience>%lu</experience>\n",
			save->stats->experience);
	fprintf(output, "\t<personalGold>%lu</personalGold>\n",
			save->stats->personalGold);
	fprintf(output, "\t<stashGold>%lu</stashGold>\n",
			save->stats->stashGold);

	fprintf(output, "</stats>\n");

	fprintf(output, "<items>\n");
	for (i = 0; i < save->itemlist->count; ++i) {
		D2SitemToXML(buff, save->itemlist->item[i], i);
		fprintf(output, "\t%s\n", buff);
	}
	fprintf(output, "</items>\n");

	fprintf(output, "</character>\n");

	D2Sfree(save);

	return 0;

}
