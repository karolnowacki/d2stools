#include <sys/stat.h>
#include <sys/types.h>

typedef unsigned long int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

struct _D2S_itemformat; //exotic forward declaration of struct, ba ale jak trzeba

typedef struct {
  u16 id;
  u32 stat1;
  u32 stat2;
  u32 stat3;
  u32 stat4;
} D2S_property;

typedef struct {
  int count;
  D2S_property **property;
} D2S_propertylist;

typedef struct {
  u8 ear_class;
  u8 ear_level;
  u8 ear_name[20];
} D2S_itemformat_ear;

typedef struct {
  u8 type[5];
  
  //ext1
  u8 gems;
  u32 GUID;
  u8 level;
  u8 qual;
  u8 vgfx;
  u16 class;
  u8 chSuf;
  u16 charm;
  u8 tome;
  u16 monster;
  u8 subQual;
  u16 prefix;
  u16 suffix;
  u16 setid;
  u8 Rname1;
  u8 Rname2;
  u16 pref1;
  u16 suff1;
  u16 pref2;
  u16 suff2;
  u16 pref3;
  u16 suff3;
  u16 UID;
  u16 runeword;
  u8 iname[17];
  //endext1
  
  u16 goldqty;
  
  u32 rand1;
  u32 rand2;
  u32 rand3;
  
  //ext2
  u16 def;
  u8 maxDur;
  u8 curDur;
  
  u16 qty;
  u8 sock;
  
  u8 set1;
  u8 set2;
  u8 set3;
  u8 set4;
  u8 set5;

  D2S_propertylist prop1;
  D2S_propertylist prop2;
  D2S_propertylist prop3;
  D2S_propertylist prop4;
  D2S_propertylist prop5;
  D2S_propertylist prop6;
  D2S_propertylist prop7;
  
  struct _D2S_itemformat **socketeditems;
  
} D2S_itemformat_info;

typedef struct _D2S_itemformat {
  u8 bytes[1024];
  int blen;
  
  u16 magick; //JM 0x4d4a
  u32 flags;
  
  char f_quest;
  char f_identified;
  char f_disabled;
  char f_dup;
  char f_socketed;
  char f_illegal;
  char f_ear;
  char f_starter;
  char f_simple;
  char f_ethereal;
  char f_inscribed;
  char f_runeword;
  
  u8 version;
  
  char location;
  char bodypos;
  char row;
  char col;
  char storedin;
  
  //4bits in buff;  
  
  D2S_itemformat_ear *ear;
  D2S_itemformat_info *info;
  
  int descfile;
  char type[6];
  char width;
  char height;
  char nodurability;
  char stackable;
  
} D2S_itemformat;

typedef struct {
  u16 magick; //JM 0x4d4a
  u16 count; 
  D2S_itemformat **item;
} D2S_itemlist;
      

typedef struct {
  u32 fileID; // 0xaa55aa55
  u32 fileVersion;
  u32 fileSize;
  u32 fileCRC;
  u32 weaponSet;

  u8 characterName[17]; //orginal 16!!
  
  u8 characterType;
  u8 characterTitle;

  u16 unknown1; // 00 00

  u8 characterClass;
  u16 unknown2; // 10 1E

  u8 level;
  u32 unknown3; // 00 00 00 00
  u32 timestamp;
  u32 unknown4; //FF FF FF FF
  
  u32 skillKeys[16];
  
  u32 leftSkill1;
  u32 leftSkill2;
  u32 rightSkill1;
  u32 rightSkill2;
  
  u8 outfit[16];
  u8 colors[16];
  
  u8 town1;
  u8 town2;
  u8 town3;
  
  u32 mapSeed;
  
  u16 unknown5;
  
  u8 mercDead;
  u8 unknown6;
  u32 mercControl;
  u16 mercName;
  u16 mercType;
  u32 mercExp;
  
  u8 unknown7[0x90];
  
} D2S_header;

#pragma pack(push)
#pragma pack(1) 
typedef struct {
  u16 start;
  u16 questStatus[6];
  u16 end;
} D2S_actinfo;
      
typedef struct {
  u32 magick; //Woo! 0x216f6f57
  u32 acts;
  u16 size;
} D2S_questinfoheader;

typedef struct {
  u16 magick; // WS 0x5357
  u8 unknown[6];
  u32 wp[3*6];
} D2S_waypoints;

typedef struct {
  u16 magick; // 0x7701
  u16 size;
  u32 normal[2];
  u32 nightmare[2];
  u32 hell[2];
  u32 normal1[2];
  u32 nightmare1[2];
  u32 hell1[2];
} D2S_npcstate;

typedef struct {
  u16 magick; //if 0x6669
  u8 skill[30];
} D2S_skills;
#pragma pack(pop)


typedef struct {
  u16 magick; //gf 0x6667
  u32 strength;
  u32 energy;
  u32 dexterity;
  u32 vitality;
  u32 statPoints;
  u32 skillPoints;
  u32 currentLife;
  u32 maximumLife;
  u32 currentMana;
  u32 maximumMana;
  u32 currentStamina;
  u32 maximumStamina;
  u32 level;
  u32 experience;
  u32 personalGold;
  u32 stashGold; 
} D2S_stats;

typedef struct {
  D2S_header *header;
  D2S_questinfoheader *questheader;
  D2S_actinfo **act;
  D2S_waypoints *waypoints;
  D2S_npcstate *npcstate;
  D2S_stats *stats;
  D2S_skills *skills;
  D2S_itemlist *itemlist;
  
  u16 corpses;
  u8 corpses_info[12];
  D2S_itemlist *corpseslist;
  
  u16 mercMagick; //jf 0x666a
  D2S_itemlist *merc_itemlist;
  
  u16 golemMagick; //kf 0x666b
  u8 hasGolem;
  D2S_itemformat *golemItem;

} D2S;
