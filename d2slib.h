#include "d2s_file_format.h"
#include "d2s_constans.h"

u32 D2Sreadbits(int bits, u8 *buff, int * buffpos, int * bitpos, FILE *fh);
D2S_property * D2SreadProperty(u8 * buff, int *blen, int *bitpos, FILE *fh);
void D2SreadPropertylist(D2S_propertylist *pl, u8 * buff, int *blen, int *bitpos, FILE *fh);
D2S_itemformat * D2Sreaditem(FILE *fh, int skipmagick);
D2S_itemlist * D2Sreaditemlist(FILE *fh);
D2S * D2Sloadsave(const char *file);
int D2StestMagick(D2S *d2s);
int D2SitemToXML(char *buff, D2S_itemformat *item, int key);
int D2SpropertylistToXML(char *buff, D2S_propertylist pl, char * root);
int D2SpropertyToXML(char *buff, D2S_property *prop);
int D2Sbase64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize);
void D2Sfree(D2S *d2s);
void D2Sitemfree(D2S_itemformat *d2s);
void D2Sitemlistfree(D2S_itemlist *ilist);
void D2Spropertylistfree(D2S_propertylist *plist);
void D2Spropertyfree(D2S_property *prop);

#define D2S_ITEM_NONE 0
#define D2S_ITEM_ARMOR 1
#define D2S_ITEM_WEAPON 2
#define D2S_ITEM_MISC 3

int D2SgetItemInfo(char *code, char *type, char * nodurability, char * width, char * height, char * stackable);