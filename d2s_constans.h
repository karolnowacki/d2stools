typedef struct {      
  char * type;
  char * code;
  char nodurability;
  char invwidth;
  char invheight;
  char stackable;
} D2S_item_description;

#define D2S_MISC_COUNT 151
extern const D2S_item_description D2S_MISC[D2S_MISC_COUNT];

#define D2S_ARMORS_COUNT 202
extern const D2S_item_description D2S_ARMORS[D2S_ARMORS_COUNT];

#define D2S_WEAPONS_COUNT 306
extern const D2S_item_description D2S_WEAPONS[D2S_WEAPONS_COUNT];

extern const int D2S_PROPERTY[][5];
