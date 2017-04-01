#include "d2s_file_format.h"
#include "d2s_constans.h"

void D2SsetError(char *fmt, ...);
const char * D2SgetLastError();

u32 D2Sreadbits(int bits, u8 *buff, int * buffpos, int * bitpos, FILE *fh);
void D2Ssetbits(int bits, u32 value, u8 * buff, int * buffpos, int * bitpos);

D2S_property * D2SreadProperty(u8 * buff, int *blen, int *bitpos, FILE *fh);

int D2SreadPropertylist(D2S_propertylist *pl, u8 * buff, int *blen,
		int *bitpos, FILE *fh);

D2S_itemformat * D2Sreaditem(FILE *fh);

D2S_itemlist * D2Sreaditemlist(FILE *fh);

D2S * D2Sloadsave(const char *file);

int D2SitemToXML(char *buff, D2S_itemformat *item, int key);

int D2SpropertylistToXML(char *buff, D2S_propertylist pl, char * root);

int D2SpropertyToXML(char *buff, D2S_property *prop);

int D2Sbase64encode(const void* data_buf, size_t dataLength, char* result,
		size_t resultSize);

size_t D2Sbase64decode(char *buff, u8 * result, size_t resultSize);

void D2Sfree(D2S *d2s);

void D2Sitemfree(D2S_itemformat *d2s);

void D2Sitemlistfree(D2S_itemlist *ilist);

void D2Spropertylistfree(D2S_propertylist *plist);

D2S_itemformat * D2SbuffToItem(u8 *buff, size_t buffsize);

void D2Spropertyfree(D2S_property *prop);

u32 D2Sfilechecksum(FILE *fh);

#define D2S_ITEM_NONE 0
#define D2S_ITEM_ARMOR 1
#define D2S_ITEM_WEAPON 2
#define D2S_ITEM_MISC 3

int D2SgetItemInfo(char *code, char *type, char * nodurability, char * width,
		char * height, char * stackable);

int D2Ssave(const char * file, D2S *d2s);
int D2SwriteItemList(D2S_itemlist *il, FILE *fh);

D2S_itemformat * D2SdropItem(D2S_itemlist *il, int itemkey);

int D2SspaceMap(D2S * d2s, int inventory[10][4], int stash[6][8]);

int D2SfindFreeSpace(D2S * d2s, int width, int height, int * col, int * row, int * storedin);

int D2SpickupItem(D2S * d2s, D2S_itemformat * item, int * newcol, int * newrow, int * newstoredin);

int D2SsetItemPlace(D2S_itemformat * item, int col, int row, int storein);


