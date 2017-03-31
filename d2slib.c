#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdarg.h>
#include <inttypes.h>

#include "d2slib.h"

void D2Sdebug(char *fmt, ...) {
#ifdef DEBUG 
#if DEBUG == 1
   va_list args;
   va_start(args,fmt);
   fprintf(stderr,"DEBUG: ");
   vfprintf(stderr,fmt,args);
   fprintf(stderr,"\n");
   fflush(stderr);
   va_end(args);
#endif
#endif
}
                        
int D2SgetItemInfo(char *code, char *type, char * nodurability, char * width, char * height, char * stackable) {
  int i;
  
  for (i=0; i<D2S_ARMORS_COUNT; i++)
    if (strcmp(code, D2S_ARMORS[i].code) == 0) {
      if (type)
        strcpy(type, D2S_ARMORS[i].type);
      if (nodurability)
        *nodurability = D2S_ARMORS[i].nodurability;
      if (width)
        *width = D2S_ARMORS[i].invwidth;
      if (height)
        *height = D2S_ARMORS[i].invheight;
      if (stackable)
        *stackable = D2S_ARMORS[i].stackable;
      return D2S_ITEM_ARMOR;
    }
    
  for (i=0; i<D2S_WEAPONS_COUNT; i++)
    if (strcmp(code, D2S_WEAPONS[i].code) == 0) {
      if (type)
        strcpy(type, D2S_WEAPONS[i].type);
      if (nodurability)
        *nodurability = D2S_WEAPONS[i].nodurability;
      if (width)
        *width = D2S_WEAPONS[i].invwidth;
      if (height)
        *height = D2S_WEAPONS[i].invheight;
      if (stackable)
        *stackable = D2S_WEAPONS[i].stackable;
      return D2S_ITEM_WEAPON;
    }

  for (i=0; i<D2S_MISC_COUNT; i++)
    if (strcmp(code, D2S_MISC[i].code) == 0) {
      if (type)
        strcpy(type, D2S_MISC[i].type);
      if (nodurability)
        *nodurability = D2S_MISC[i].nodurability;
      if (width)
        *width = D2S_MISC[i].invwidth;
      if (height)
        *height = D2S_MISC[i].invheight;
      if (stackable)
        *stackable = D2S_MISC[i].stackable;
      return D2S_ITEM_MISC;
    }

  return D2S_ITEM_NONE;  
}

u32 D2Sreadbits(int bits, u8 *buff, int * buffpos, int * bitpos, FILE *fh) {
  u32 ret = 0;
  int mbits = bits;
  while (bits > 0) {  
    if (*bitpos > 7) {
      buff[(*buffpos)++] = fgetc(fh);
      *bitpos = 0;
    }
    ret |= (buff[(*buffpos)-1] & (1 << (*bitpos)) ? 1 : 0) << (mbits-bits);
    (*bitpos)++;bits--;
  }
  return ret;  
}

D2S_property * D2SreadProperty(u8 * buff, int *blen, int *bitpos, FILE *fh) {
  D2S_property *prop = malloc(sizeof(*prop));
  prop->id = D2Sreadbits(9, buff, blen, bitpos, fh);
  
  D2Sdebug("PID = %d",  prop->id);
  
  assert(prop->id == 511 || prop->id < 370);
  
  prop->stat1 = prop->stat2 = prop->stat3 = prop->stat4 = 0;
  
  if (prop->id < 511) {
    if (D2S_PROPERTY[prop->id][0] > 0)
      prop->stat1 = D2Sreadbits(D2S_PROPERTY[prop->id][1], buff, blen, bitpos, fh);
    if (D2S_PROPERTY[prop->id][0] > 1)
      prop->stat2 = D2Sreadbits(D2S_PROPERTY[prop->id][2], buff, blen, bitpos, fh);
    if (D2S_PROPERTY[prop->id][0] > 2)
      prop->stat3 = D2Sreadbits(D2S_PROPERTY[prop->id][3], buff, blen, bitpos, fh);
    if (D2S_PROPERTY[prop->id][0] > 3)
      prop->stat4 = D2Sreadbits(D2S_PROPERTY[prop->id][4], buff, blen, bitpos, fh);
  } 
  return prop;  
}

void D2SreadPropertylist(D2S_propertylist *pl, u8 * buff, int *blen, int *bitpos, FILE *fh) {
  D2S_property * lastprop = NULL;
  do {
    lastprop = D2SreadProperty(buff, blen, bitpos, fh);
    
    D2Sdebug("ReadProp[%d]", lastprop->id);
    
    if (pl->count <= 0) {
      pl->count = 1;
      pl->property = malloc(pl->count * sizeof(*(pl->property)));
    } else {
      pl->count++;
      pl->property = realloc(pl->property, pl->count * sizeof(*(pl->property)));
    }
    pl->property[pl->count-1] = lastprop;
  } while(lastprop->id < 511);
}

D2S_itemformat * D2Sreaditem(FILE *fh, int skipmagick) {
  int bitpos,i;
  
  D2S_itemformat *item = malloc(sizeof(*item));
  
  if (skipmagick)
     memcpy(item->bytes, "JM", 2);
  else
     fread(item->bytes, 1, 2, fh);
     
  memcpy(&(item->magick), item->bytes, 2);
  
  item->descfile = 0;
  item->type[0] = 0;;
  item->width = 1;
  item->height = 1;
  item->nodurability = 1;
  item->stackable = 0;
  
  bitpos = 8;
  item->blen = 2;
  
  item->flags = D2Sreadbits(32, item->bytes, &(item->blen), &bitpos, fh);
  item->version = D2Sreadbits(10, item->bytes, &(item->blen), &bitpos, fh);
  
  item->f_quest = item->flags & 1 ?1:0;
  item->f_identified = item->flags & 1<<4 ?1:0;
  item->f_disabled = item->flags & 1<<8 ?1:0;
  item->f_dup = item->flags & 1<<10 ?1:0;
  item->f_socketed = item->flags & 1<<11 ?1:0;
  item->f_illegal = item->flags & 1<<14 ?1:0;
  item->f_ear = item->flags & 1<<16 ?1:0;
  item->f_starter = item->flags & 1<<17 ?1:0;
  item->f_simple = item->flags & 1<<21 ?1:0;
  item->f_ethereal = item->flags & 1<<22 ?1:0;
  item->f_inscribed = item->flags & 1<<24 ?1:0;
  item->f_runeword = item->flags & 1<<26 ?1:0;
    
  item->location = D2Sreadbits(3, item->bytes, &(item->blen), &bitpos, fh);
  item->bodypos = D2Sreadbits(4, item->bytes, &(item->blen), &bitpos, fh);
  item->row = D2Sreadbits(4, item->bytes, &(item->blen), &bitpos, fh);
  item->col = D2Sreadbits(4, item->bytes, &(item->blen), &bitpos, fh);
  item->storedin = D2Sreadbits(3, item->bytes, &(item->blen), &bitpos, fh);
  
  
  D2Sdebug(" == magick: %x", item->magick);
  D2Sdebug(" == flags: %lx", item->flags);
  D2Sdebug("\t == f_quest %d", item->f_quest);
  D2Sdebug("\t == f_identied %d", item->f_identified);
  D2Sdebug("\t == f_dup %d", item->f_dup);
  D2Sdebug("\t == f_socketedt %d", item->f_socketed);
  D2Sdebug("\t == f_illegal %d", item->f_illegal);
  D2Sdebug("\t == f_ear %d", item->f_ear);
  D2Sdebug("\t == f_starter %d", item->f_starter);
  D2Sdebug("\t == f_simple %d", item->f_simple);
  D2Sdebug("\t == f_ethereal %d", item->f_ethereal);
  D2Sdebug("\t == f_inscribed %d", item->f_inscribed);
  D2Sdebug("\t == f_runeword %d", item->f_runeword);
  
  D2Sdebug("\t == x %d", item->row);
  D2Sdebug("\t == y %d", item->location);
  D2Sdebug("\t == bodypos %d", item->bodypos);
  D2Sdebug("\t == storedin %d", item->storedin);
  
  D2Sdebug("\t == x %d", item->row);
  D2Sdebug("\t == y %d", item->col);
                        
  D2Sdebug(" == version: %x", item->version);

  if (item->f_ear) {
    item->info = NULL;
    item->ear = malloc(sizeof(*(item->ear)));
    memset(item->ear->ear_name, 0, 20);
    item->ear->ear_class = D2Sreadbits(3, item->bytes, &(item->blen), &bitpos, fh);
    item->ear->ear_level = D2Sreadbits(7, item->bytes, &(item->blen), &bitpos, fh);
    for (i=0; i<20 && (i<1 || item->ear->ear_name[i-1] > 32); i++)
      item->ear->ear_name[i] = D2Sreadbits(7, item->bytes, &(item->blen), &bitpos, fh);
      
    D2Sdebug("EAR Name: |%s|", item->ear->ear_name);
    D2Sdebug("EAR Lvl: %d", item->ear->ear_level);
    D2Sdebug("EAR Class: %d", item->ear->ear_class);
    
    // 8 bit null end
    D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
    
      
    return item;  
  }
  
  item->ear = NULL;
  item->info = malloc(sizeof(*(item->info)));
  
  for (i=0; i<4; i++)
    item->info->type[i] = D2Sreadbits(8, item->bytes, &(item->blen), &bitpos, fh);
  item->info->type[i] = 0;
  while (i > 0 && item->info->type[i]<=32) item->info->type[i--] = 0;;
    
  D2Sdebug(" == d2s item name: %s", item->info->type);
  
  item->descfile = D2SgetItemInfo((char *)item->info->type, item->type, &(item->nodurability), &(item->width), &(item->height), &(item->stackable));
  
  D2Sdebug(" == descfile: %d", item->descfile);
  D2Sdebug(" == type: %s", item->type);
  D2Sdebug(" == nodur: %d, w: %d, h: %d, stock: %d", item->nodurability, item->width, item->height, item->stackable);
    
  if (item->f_simple) {
    item->info->gems = 0;
    item->info->GUID = 0;
    item->info->level = 0;
    item->info->qual = 0;
    item->info->vgfx = 0;
    item->info->class = 0;
    item->info->chSuf = 0;
    item->info->charm = 0;
    item->info->tome = 0;
    item->info->monster = 0;
    item->info->subQual = 0;
    item->info->prefix = 0;
    item->info->suffix = 0;
    item->info->setid = 0;
    item->info->Rname1 = 0;
    item->info->Rname2 = 0;
    item->info->pref1 = 0;
    item->info->suff1 = 0;
    item->info->pref2 = 0;
    item->info->suff2 = 0;
    item->info->pref3 = 0;
    item->info->suff3 = 0;
    item->info->UID = 0;
    item->info->runeword = 0;
    memset(item->info->iname, 0, 15);
  } else {
    item->info->gems = D2Sreadbits(3, item->bytes, &(item->blen), &bitpos, fh);
    item->info->GUID = D2Sreadbits(32, item->bytes, &(item->blen), &bitpos, fh);
    item->info->level = D2Sreadbits(7, item->bytes, &(item->blen), &bitpos, fh);
    item->info->qual = D2Sreadbits(4, item->bytes, &(item->blen), &bitpos, fh);
    
    D2Sdebug(" == GUID: %lx", item->info->GUID);
    D2Sdebug(" == level: %d", item->info->level);
    D2Sdebug(" == qual: %d", item->info->qual);
    
    if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
      item->info->vgfx = D2Sreadbits(3, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->vgfx = 0;
      
    D2Sdebug(" == vgfx: %d", item->info->vgfx);
      
    if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
      item->info->class = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->class = 0;
      
    D2Sdebug(" == class: %d", item->info->class);
     
    if (item->info->qual == 2) {  
      if (
        strcmp(item->type, "play") == 0 ||
        strcmp(item->type, "char") == 0 ||
        strcmp(item->type, "scha") == 0 ||
        strcmp(item->type, "mcha") == 0 ||
        strcmp(item->type, "lcha") == 0
      ) { 
        item->info->chSuf = D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
        item->info->charm = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);
      } else {
        item->info->chSuf = 0;
        item->info->charm = 0;
      }
      
      D2Sdebug("TYPE: %s", item->type);
      
      if (strcmp(item->type, "book") == 0 || strcmp(item->type, "scro") == 0) {
        D2Sdebug(" == BOOK/SCRO == ");
        item->info->tome = D2Sreadbits(5, item->bytes, &(item->blen), &bitpos, fh);
      } else
        item->info->tome = 0;
        
      if (strcmp(item->type, "body") == 0)
        item->info->monster = D2Sreadbits(10, item->bytes, &(item->blen), &bitpos, fh);
      else
        item->info->monster = 0;
        
    } else {
      item->info->chSuf = 0;
      item->info->charm = 0;
      item->info->tome = 0;
      item->info->monster = 0;      
    }
    
    if (item->info->qual == 1 || item->info->qual == 3)
      item->info->subQual = D2Sreadbits(3, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->subQual = 0;
    
    if (item->info->qual == 4) {  
      item->info->prefix = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh); 
      item->info->suffix = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);
      
      D2Sdebug(" == prefix suffix: %d %d", item->info->prefix, item->info->suffix);
      
    } else {
      item->info->prefix = 0;
      item->info->suffix = 0;
    }
    
    if (item->info->qual == 5)
      item->info->setid = D2Sreadbits(12, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->setid = 0;
      
    if (item->info->qual == 6 || item->info->qual == 8 || item->info->qual == 9) {  
      item->info->Rname1 = D2Sreadbits(8, item->bytes, &(item->blen), &bitpos, fh);
      item->info->Rname2 = D2Sreadbits(8, item->bytes, &(item->blen), &bitpos, fh);
    } else {
      item->info->Rname1 = 0;
      item->info->Rname2 = 0;
    }
    
    D2Sdebug(" == Rname %d %d", item->info->Rname1, item->info->Rname2);
    
    item->info->pref1 = 0;
    item->info->suff1 = 0;
    item->info->pref2 = 0;
    item->info->suff2 = 0;
    item->info->pref3 = 0;
    item->info->suff3 = 0;
                                      
    if (item->info->qual == 6 || item->info->qual == 8) {
    
      if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
        item->info->pref1 = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);
        
      if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
        item->info->suff1 = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);
        
      if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
        item->info->pref2 = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);

      if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
        item->info->suff2 = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);      

      if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
        item->info->pref3 = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);

      if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh))
        item->info->suff3 = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);      
        
      D2Sdebug(" == pref1: %d", item->info->pref1);
      D2Sdebug(" == pref2: %d", item->info->pref2);
      D2Sdebug(" == pref3: %d", item->info->pref3);
      D2Sdebug(" == suff1: %d", item->info->suff1);
      D2Sdebug(" == suff2: %d", item->info->suff2);
      D2Sdebug(" == suff3: %d", item->info->suff3);
           
    } 
    
    if (item->info->qual == 7)
      item->info->UID = D2Sreadbits(12, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->UID = 0;
    
    if (item->f_runeword)
      item->info->runeword = D2Sreadbits(16, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->runeword = 0;
    
    memset(item->info->iname, 0, 15); 
    if (item->f_inscribed) {
      i=0;
      do {
        item->info->iname[i] = D2Sreadbits(7, item->bytes, &(item->blen), &bitpos, fh);
        i++;
      } while (item->info->iname[i-1]);        
      
      D2Sdebug(" == I name: |%s|", item->info->iname);
      
    }
    
  }
  
  if (strcmp(item->type, "gold") == 0) {
    D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
    item->info->goldqty = D2Sreadbits(12, item->bytes, &(item->blen), &bitpos, fh);
  } else item->info->goldqty = 0;
  D2Sdebug(" == goldqty: %d", item->info->goldqty);

  if (strcmp((char *)item->info->type, "mss") == 0) { // Mefisto SoulStone
    // cos mega dziwnego
    // chyba poziom trudnosci z jakiego pochodzi kamien
    item->info->rand1 = D2Sreadbits(4, item->bytes, &(item->blen), &bitpos, fh);   
    item->info->rand2 = 0;
    item->info->rand3 = 0;
  } else if (D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh)) {
     item->info->rand1 = D2Sreadbits(32, item->bytes, &(item->blen), &bitpos, fh);
     item->info->rand2 = D2Sreadbits(32, item->bytes, &(item->blen), &bitpos, fh);
     item->info->rand3 = D2Sreadbits(32, item->bytes, &(item->blen), &bitpos, fh);
  } else {
     item->info->rand1 = 0;
     item->info->rand2 = 0; 
     item->info->rand3 = 0;
  }
  
  D2Sdebug("== rand1: %lx", item->info->rand1);
  D2Sdebug("== rand1: %lx", item->info->rand2);
  D2Sdebug("== rand1: %lx", item->info->rand3);
  
  item->info->prop1.count = 0;
  item->info->prop1.property = NULL; 
  item->info->prop2.count = 0;
  item->info->prop2.property = NULL; 
  item->info->prop3.count = 0;
  item->info->prop3.property = NULL; 
  item->info->prop4.count = 0;
  item->info->prop4.property = NULL; 
  item->info->prop5.count = 0;
  item->info->prop5.property = NULL; 
  item->info->prop6.count = 0;
  item->info->prop6.property = NULL; 
  item->info->prop7.count = 0;
  item->info->prop7.property = NULL; 
                            
  if (item->f_simple) {
    item->info->def = 0;
    item->info->maxDur = 0;
    item->info->curDur = 0;

    item->info->qty = 0;
    item->info->sock = 0;

    item->info->set1 = 0;
    item->info->set2 = 0;
    item->info->set3 = 0;
    item->info->set4 = 0;
    item->info->set5 = 0;

  } else {

    if (item->descfile == D2S_ITEM_ARMOR)
      item->info->def = D2Sreadbits(11, item->bytes, &(item->blen), &bitpos, fh);   
    else
      item->info->def = 0;

    D2Sdebug(" == nodur: %d", item->nodurability);
    D2Sdebug(" == stack: %d", item->stackable);
      
    if (item->descfile == D2S_ITEM_ARMOR || item->descfile == D2S_ITEM_WEAPON)
      item->info->maxDur = D2Sreadbits(8, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->maxDur = 0;
      
    if (item->info->maxDur > 0)
      item->info->curDur = D2Sreadbits(9, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->curDur = 0;

    if (item->stackable)
      item->info->qty = D2Sreadbits(9, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->qty = 0;
      
    if (item->f_socketed)
      item->info->sock = D2Sreadbits(4, item->bytes, &(item->blen), &bitpos, fh);
    else
      item->info->sock = 0;

    if (item->info->qual == 5) {
      item->info->set1 = D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
      item->info->set2 = D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
      item->info->set3 = D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
      item->info->set4 = D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
      item->info->set5 = D2Sreadbits(1, item->bytes, &(item->blen), &bitpos, fh);
    } else {
      item->info->set1 = 0;
      item->info->set2 = 0;
      item->info->set3 = 0;
      item->info->set4 = 0;
      item->info->set5 = 0;
    }

    D2SreadPropertylist(&(item->info->prop1), item->bytes, &(item->blen), &bitpos, fh);
    
    if (item->info->set1)
      D2SreadPropertylist(&(item->info->prop2), item->bytes, &(item->blen), &bitpos, fh);
      
    if (item->info->set2)
      D2SreadPropertylist(&(item->info->prop3), item->bytes, &(item->blen), &bitpos, fh);
    
    if (item->info->set3)
      D2SreadPropertylist(&(item->info->prop4), item->bytes, &(item->blen), &bitpos, fh);
            
    if (item->info->set4)
      D2SreadPropertylist(&(item->info->prop5), item->bytes, &(item->blen), &bitpos, fh);
                
    if (item->info->set5)
      D2SreadPropertylist(&(item->info->prop6), item->bytes, &(item->blen), &bitpos, fh);
                    
    if (item->f_runeword)    
      D2SreadPropertylist(&(item->info->prop7), item->bytes, &(item->blen), &bitpos, fh);
          
  }
  
  if (item->info->gems == 0)
    item->info->socketeditems = NULL;
  else {
    item->info->socketeditems = malloc(item->info->gems * sizeof(*(item->info->socketeditems)));
      for (i=0; i<item->info->gems; ++i)
        item->info->socketeditems[i] = D2Sreaditem(fh,0);
  }  
  return item;                       
}

D2S_itemlist * D2Sreaditemlist(FILE *fh) {
  int i;
  D2S_itemlist *itemlist = malloc(sizeof(*itemlist));
  fread(&itemlist->magick, 2, 1, fh);
  fread(&itemlist->count, 2, 1, fh);
  
  D2Sdebug(" ==(itemlist) magic: %x\n", itemlist->magick);
  D2Sdebug(" ==(itemlist) count: %d\n", itemlist->count);
  
  itemlist->item = malloc(itemlist->count * sizeof(*(itemlist->item)));
  for (i=0; i<itemlist->count; ++i)
    itemlist->item[i] = D2Sreaditem(fh,0);
    
  return itemlist;
}

D2S * D2Sloadsave(const char *file) {
  D2S *d2s;
  FILE *fh = fopen(file, "rb");
  
  if (fh == NULL)
     return NULL;
  
  int char_propid;
  u8 buff[1024];
  int bpos,bitpos;
  
  d2s = malloc(sizeof(*d2s));
  
  d2s->header = NULL;
  d2s->questheader = NULL;
  d2s->act = NULL;
  d2s->waypoints = NULL;
  d2s->npcstate = NULL;
  d2s->stats = NULL;
  d2s->skills = NULL;
  d2s->itemlist = NULL;
  d2s->corpseslist = NULL;
  d2s->merc_itemlist = NULL;
  d2s->golemItem = NULL;
  
  d2s->header = malloc(sizeof(*(d2s->header)));
  
  fread(&(d2s->header->fileID), 4, 1, fh);
  fread(&(d2s->header->fileVersion), 4, 1, fh);
  fread(&(d2s->header->fileSize), 4, 1, fh);
  fread(&(d2s->header->fileCRC), 4, 1, fh);
  fread(&(d2s->header->weaponSet), 4, 1, fh);
  
  memset(d2s->header->characterName, 0, 17);
  fread(d2s->header->characterName, 16, 1, fh);
  
  fread(&(d2s->header->characterType), 1, 1, fh);
  fread(&(d2s->header->characterTitle), 1, 1, fh);
  
  fread(&(d2s->header->unknown1), 2, 1, fh); // 00 00
  
  fread(&(d2s->header->characterClass), 1, 1, fh);
  fread(&(d2s->header->unknown2), 2, 1, fh); // 10 1E
  
  fread(&(d2s->header->level), 1, 1, fh);
  fread(&(d2s->header->unknown3), 4, 1, fh);
  fread(&(d2s->header->timestamp), 4, 1, fh);
  fread(&(d2s->header->unknown4), 4, 1, fh);
  
  fread(d2s->header->skillKeys, 4, 16, fh); 
  
  fread(&(d2s->header->leftSkill1), 4, 1, fh); 
  fread(&(d2s->header->leftSkill2), 4, 1, fh);
  fread(&(d2s->header->rightSkill1), 4, 1, fh);
  fread(&(d2s->header->rightSkill2), 4, 1, fh);
  
  fread(d2s->header->outfit, 1, 16, fh);
  fread(d2s->header->colors, 1, 16, fh); 
  
  fread(&(d2s->header->town1), 1, 1, fh);
  fread(&(d2s->header->town2), 1, 1, fh);
  fread(&(d2s->header->town3), 1, 1, fh);
  
  fread(&(d2s->header->mapSeed), 4, 1, fh);
  fread(&(d2s->header->unknown5), 2, 1, fh);
  
  fread(&(d2s->header->mercDead), 1, 1, fh);
  fread(&(d2s->header->unknown6), 1, 1, fh);
  fread(&(d2s->header->mercControl), 4, 1, fh);
  fread(&(d2s->header->mercName), 2, 1, fh);
  fread(&(d2s->header->mercType), 2, 1, fh); 
  fread(&(d2s->header->mercExp), 4, 1, fh);

  fread(d2s->header->unknown7, 1, 0x90, fh);

  d2s->questheader = malloc(sizeof(*(d2s->questheader)));
  fread(d2s->questheader, sizeof(*(d2s->questheader)), 1, fh);
  
  d2s->act = malloc(3 * d2s->questheader->acts * sizeof(**(d2s->act)));
  fread(d2s->act, sizeof(**(d2s->act)), 3 * d2s->questheader->acts, fh);

  d2s->waypoints = malloc(sizeof(*(d2s->waypoints)));
  fread(d2s->waypoints, sizeof(*(d2s->waypoints)), 1, fh);

  d2s->npcstate = malloc(sizeof(*(d2s->npcstate)));
  fread(d2s->npcstate, sizeof(*(d2s->npcstate)), 1, fh);

  d2s->stats = malloc(sizeof(*(d2s->stats)));
  fread(&(d2s->stats->magick), 1, 2, fh);

  char_propid = 0;
  bpos = 0;
  bitpos = 8;
  
  d2s->stats->strength = 0;
  d2s->stats->energy = 0;
  d2s->stats->dexterity = 0;
  d2s->stats->vitality = 0;
  d2s->stats->statPoints = 0;
  d2s->stats->skillPoints = 0;
  d2s->stats->currentLife = 0;
  d2s->stats->maximumLife = 0;
  d2s->stats->currentMana = 0;
  d2s->stats->maximumMana = 0;
  d2s->stats->currentStamina = 0;
  d2s->stats->maximumStamina = 0;
  d2s->stats->level = 0;
  d2s->stats->experience = 0;
  d2s->stats->personalGold = 0;
  d2s->stats->stashGold = 0;
  
  while (char_propid < 16) {
    char_propid = D2Sreadbits(9, buff, &bpos, &bitpos, fh);
    switch (char_propid) {
      case 0 : d2s->stats->strength = D2Sreadbits(10, buff, &bpos, &bitpos, fh); break;
      case 1 : d2s->stats->energy = D2Sreadbits(10, buff, &bpos, &bitpos, fh); break;
      case 2 : d2s->stats->dexterity = D2Sreadbits(10, buff, &bpos, &bitpos, fh); break;
      case 3 : d2s->stats->vitality = D2Sreadbits(10, buff, &bpos, &bitpos, fh); break;
      case 4 : d2s->stats->statPoints = D2Sreadbits(10, buff, &bpos, &bitpos, fh); break;
      case 5 : d2s->stats->skillPoints = D2Sreadbits(8, buff, &bpos, &bitpos, fh); break;
      case 6 : d2s->stats->currentLife = D2Sreadbits(21, buff, &bpos, &bitpos, fh); break;
      case 7 : d2s->stats->maximumLife = D2Sreadbits(21, buff, &bpos, &bitpos, fh); break;
      case 8 : d2s->stats->currentMana = D2Sreadbits(21, buff, &bpos, &bitpos, fh); break;
      case 9 : d2s->stats->maximumMana = D2Sreadbits(21, buff, &bpos, &bitpos, fh); break;
      case 10 : d2s->stats->currentStamina = D2Sreadbits(21, buff, &bpos, &bitpos, fh); break;
      case 11 : d2s->stats->maximumStamina = D2Sreadbits(21, buff, &bpos, &bitpos, fh); break;
      case 12 : d2s->stats->level = D2Sreadbits(7, buff, &bpos, &bitpos, fh); break;
      case 13 : d2s->stats->experience = D2Sreadbits(32, buff, &bpos, &bitpos, fh); break;
      case 14 : d2s->stats->personalGold = D2Sreadbits(25, buff, &bpos, &bitpos, fh); break;
      case 15 : d2s->stats->stashGold = D2Sreadbits(25, buff, &bpos, &bitpos, fh); break;
    }
  }
  d2s->skills = malloc(sizeof(*(d2s->skills)));
  fread(d2s->skills, sizeof(*(d2s->skills)), 1, fh);

  d2s->itemlist = D2Sreaditemlist(fh);

  fread(&(d2s->corpses), 2, 1, fh);
  if (d2s->corpses == 0x4d4a) //JM ??
    fread(&(d2s->corpses), 2, 1, fh);

  if (d2s->corpses) {  
    D2Sdebug("HAS CORPSES!");
    fread(d2s->corpses_info, 1, 12, fh);
    d2s->corpseslist = D2Sreaditemlist(fh);
  }
  
  fread(&d2s->mercMagick, 1, 2, fh);

  D2Sdebug("MERC Magick: %x", d2s->mercMagick);
  D2Sdebug("MERC Dead: %d", d2s->header->mercDead);
  D2Sdebug("MERC Control: %d", d2s->header->mercControl);
  D2Sdebug("MERC Name: %d", d2s->header->mercName);
  D2Sdebug("MERC Type: %d", d2s->header->mercType);
  D2Sdebug("MERC Exp: %d", d2s->header->mercExp);
  
  if (d2s->header->mercControl || d2s->header->mercType || d2s->header->mercName)
     d2s->merc_itemlist = D2Sreaditemlist(fh);
 
  fread(&d2s->golemMagick, 1, 2, fh);
  D2Sdebug("GOLEM Magick: %x", d2s->golemMagick);
  
  fread(&(d2s->hasGolem), 1, 1, fh);
  if (d2s->hasGolem)
    d2s->golemItem = D2Sreaditem(fh, 1);
  else
    d2s->golemItem = NULL;

  fclose(fh);
  return d2s;  
}


int D2StestMagick(D2S *d2s) {
  if (d2s == NULL)
    return 0;
  
  if (d2s->header->fileID != 0xaa55aa55)
    return 0;
  
  if (d2s->questheader->magick != 0x216f6f57)
    return 0;
    
  if (d2s->waypoints->magick != 0x5357)
    return 0;
    
  if (d2s->npcstate->magick != 0x7701)
    return 0;
    
  if (d2s->stats->magick != 0x6667)
    return 0;
    
  if (d2s->skills->magick != 0x6669)
    return 0;
    
  if (d2s->itemlist->magick != 0x4d4a)
    return 0;
    
  if (d2s->mercMagick != 0x666a)
    return 0;
    
  if (d2s->golemMagick != 0x666b)
    return 0;

  return 1;
}

int D2SpropertylistToXML(char *buff, D2S_propertylist pl, char * root) {
   char pbuff[512];
   char *cb = buff;
   int i;
   
   cb += sprintf(cb, "<%s>", root);
   for (i=0; i<pl.count-1; ++i) {
      D2SpropertyToXML(pbuff, pl.property[i]);
      cb += sprintf(cb, "%s", pbuff);   
   }
   cb += sprintf(cb, "</%s>", root);
   *cb = 0;
   return cb - buff;
}
int D2SpropertyToXML(char *buff, D2S_property *prop) {
   char *cb = buff;
   cb += sprintf(cb, "<property id=\"%d\">", prop->id);
   if (prop->stat1)
      cb += sprintf(cb, "<stat1>%lu</stat1>", prop->stat1);
   if (prop->stat2)
      cb += sprintf(cb, "<stat2>%lu</stat2>", prop->stat2);
   if (prop->stat3)
      cb += sprintf(cb, "<stat3>%lu</stat3>", prop->stat3);
   if (prop->stat4)
      cb += sprintf(cb, "<stat4>%lu</stat4>", prop->stat4);            
   cb += sprintf(cb, "</property>");
   *cb = 0;
   return cb - buff;
}

int D2SitemToXML(char *buff, D2S_itemformat *item, int key) {
  char *cb = buff;
  char b64buff[1024];
  char propbuff[1024];
  char sockbuff[8192];
  int i;
  
  cb += sprintf(cb, "<item");

  if (key >= 0) cb += sprintf(cb, " key=\"%d\"", key);
  
  cb += sprintf(cb, " quest=\"%d\"", item->f_quest);
  cb += sprintf(cb, " identified=\"%d\"", item->f_identified);
  cb += sprintf(cb, " disabled=\"%d\"", item->f_disabled);
  cb += sprintf(cb, " dup=\"%d\"", item->f_dup);
  cb += sprintf(cb, " socketed=\"%d\"", item->f_socketed);
  cb += sprintf(cb, " illegal=\"%d\"", item->f_illegal);
  cb += sprintf(cb, " ear=\"%d\"", item->f_ear);
  cb += sprintf(cb, " starter=\"%d\"", item->f_starter);
  cb += sprintf(cb, " simple=\"%d\"", item->f_simple);
  cb += sprintf(cb, " ethereal=\"%d\"", item->f_ethereal);
  cb += sprintf(cb, " inscribed=\"%d\"", item->f_inscribed);
  cb += sprintf(cb, " runeword=\"%d\"", item->f_runeword);
  
  cb += sprintf(cb, " type=\"%s\"", item->type);
  cb += sprintf(cb, " width=\"%d\"", item->width);
  cb += sprintf(cb, " height=\"%d\"", item->height);
  cb += sprintf(cb, " nodurability=\"%d\"", item->nodurability);
  cb += sprintf(cb, " stackable=\"%d\"", item->stackable);
  
  cb += sprintf(cb, " location=\"%d\"", item->location);
  cb += sprintf(cb, " bodypos=\"%d\"", item->bodypos);
  cb += sprintf(cb, " row=\"%d\"", item->row);
  cb += sprintf(cb, " col=\"%d\"", item->col);
  cb += sprintf(cb, " storedin=\"%d\"", item->storedin);
            
  cb += sprintf(cb, ">");
  
  D2Sbase64encode(item->bytes, item->blen, b64buff, 1024);
  cb += sprintf(cb, "<d2i>%s</d2i>", b64buff);  
  
  if (item->f_ear) {
     cb += sprintf(cb, "<class>%d</class>", item->ear->ear_class);
     cb += sprintf(cb, "<level>%d</level>", item->ear->ear_level);
     cb += sprintf(cb, "<name>%s</name>", item->ear->ear_name);  
  } else {
     cb += sprintf(cb, "<type>%s</type>", item->info->type);  
     
     if (!item->f_simple) {
        cb += sprintf(cb, "<guid>%08lx</guid>", item->info->GUID);
        cb += sprintf(cb, "<level>%d</level>", item->info->level);
        cb += sprintf(cb, "<qual>%d</qual>", item->info->qual);
        if (item->info->vgfx) cb += sprintf(cb, "<vgfx>%d</vgfx>", item->info->vgfx);
        if (item->info->class) cb += sprintf(cb, "<class>%d</class>", item->info->class);
        
        if (item->info->charm) cb += sprintf(cb, "<charm suffix=\"%d\">%d</charm>", item->info->chSuf, item->info->charm);
        if (item->info->tome) cb += sprintf(cb, "<tome>%d</tome>", item->info->tome);
        if (item->info->monster) cb += sprintf(cb, "<monster>%d</monster>", item->info->monster);
        
        if (item->info->subQual) cb += sprintf(cb, "<subQual>%d</subQual>", item->info->subQual);
        
        if (item->info->prefix) cb += sprintf(cb, "<prefix>%d</prefix>", item->info->prefix);
        if (item->info->suffix) cb += sprintf(cb, "<suffix>%d</suffix>", item->info->suffix);
        if (item->info->setid) cb += sprintf(cb, "<setid>%d</setid>", item->info->setid);
        if (item->info->Rname1) cb += sprintf(cb, "<Rname1>%d</Rname1>", item->info->Rname1);
        if (item->info->Rname2) cb += sprintf(cb, "<Rname2>%d</Rname2>", item->info->Rname2);
        if (item->info->pref1) cb += sprintf(cb, "<pref1>%d</pref1>", item->info->pref1);
        if (item->info->suff1) cb += sprintf(cb, "<suff1>%d</suff1>", item->info->suff1);
        if (item->info->pref2) cb += sprintf(cb, "<pref2>%d</pref2>", item->info->pref2);
        if (item->info->suff2) cb += sprintf(cb, "<suff2>%d</suff2>", item->info->suff2);
        if (item->info->pref3) cb += sprintf(cb, "<pref3>%d</pref3>", item->info->pref3);
        if (item->info->suff3) cb += sprintf(cb, "<suff3>%d</suff3>", item->info->suff3);
        if (item->info->UID) cb += sprintf(cb, "<UID>%d</UID>", item->info->UID);
        if (item->info->runeword) cb += sprintf(cb, "<runeword>%d</runeword>", item->info->runeword);
        if (item->f_inscribed) cb += sprintf(cb, "<iname>%s</iname>", item->info->iname);
     }
     if (item->info->goldqty) {
        cb += sprintf(cb, "<gold>%u</gold>", item->info->goldqty);
     }
     if (item->info->rand1 || item->info->rand2 || item->info->rand3) {
        cb += sprintf(cb, "<rand>%08lx%08lx%08lx</rand>", item->info->rand1, item->info->rand2, item->info->rand3);
     }
     if (!item->f_simple) {
        if (item->info->def) cb += sprintf(cb, "<def>%d</def>", item->info->def);
        if (item->info->maxDur) {
           cb += sprintf(cb, "<maxDur>%d</maxDur>", item->info->maxDur);
           cb += sprintf(cb, "<curDur>%d</curDur>", item->info->curDur);
        } 
        if (item->info->qty) cb += sprintf(cb, "<qty>%d</qty>", item->info->qty);
        if (item->info->sock) cb += sprintf(cb, "<sock>%d</sock>", item->info->sock);
        
        D2SpropertylistToXML(propbuff, item->info->prop1, "properties");
        cb += sprintf(cb, "%s", propbuff);
        
        if (item->info->set1) {
           D2SpropertylistToXML(propbuff, item->info->prop2, "setproperties1");
           cb += sprintf(cb, "%s", propbuff);
        }
        
        if (item->info->set2) {
           D2SpropertylistToXML(propbuff, item->info->prop3, "setproperties2");
           cb += sprintf(cb, "%s", propbuff);
        }

        if (item->info->set3) {
           D2SpropertylistToXML(propbuff, item->info->prop4, "setproperties3");
           cb += sprintf(cb, "%s", propbuff);
        }

        if (item->info->set4) {
           D2SpropertylistToXML(propbuff, item->info->prop5, "setproperties4");
           cb += sprintf(cb, "%s", propbuff);
        }

        if (item->info->set5) {
           D2SpropertylistToXML(propbuff, item->info->prop6, "setproperties5");
           cb += sprintf(cb, "%s", propbuff);
        }
        
        if (item->f_runeword) {
           D2SpropertylistToXML(propbuff, item->info->prop7, "runewordproperties");
           cb += sprintf(cb, "%s", propbuff);        
        }
        
        
        if (item->info->gems) {
           cb += sprintf(cb, "<sockedin>");
           for (i=0; i<item->info->gems; i++) {
              D2SitemToXML(sockbuff, item->info->socketeditems[i], -1);
              cb += sprintf(cb, "%s", sockbuff);           
           }
           cb += sprintf(cb, "</sockedin>");
        }
         
     }
    
  }  
  
  cb += sprintf(cb, "</item>");
  
  *cb = 0;
  return cb - buff;
}

void D2Sfree(D2S *d2s) {
   if (!d2s)
      return;
   free(d2s->header);
   free(d2s->questheader);
   free(d2s->act);
   free(d2s->waypoints);
   free(d2s->npcstate);
   free(d2s->stats);
   free(d2s->skills);
   
   D2Sitemlistfree(d2s->itemlist);
   
   if (d2s->corpses)
      D2Sitemlistfree(d2s->corpseslist);
   
   D2Sitemlistfree(d2s->merc_itemlist);
   D2Sitemfree(d2s->golemItem);
   
   free(d2s);
}

void D2Sitemfree(D2S_itemformat *item) {
   int i;

   if (!item)
      return;
   free(item->ear);
   
   if (item->info) {
      for (i=0; i<item->info->prop1.count; i++)
         free(item->info->prop1.property[i]);
      free(item->info->prop1.property);

      for (i=0; i<item->info->prop2.count; i++)
         free(item->info->prop2.property[i]);
      free(item->info->prop2.property);
         
      for (i=0; i<item->info->prop3.count; i++)
         free(item->info->prop3.property[i]);
      free(item->info->prop3.property);
         
      for (i=0; i<item->info->prop4.count; i++)
         free(item->info->prop4.property[i]);
      free(item->info->prop4.property);
         
      for (i=0; i<item->info->prop5.count; i++)
         free(item->info->prop5.property[i]);
      free(item->info->prop5.property);
         
      for (i=0; i<item->info->prop6.count; i++)
         free(item->info->prop6.property[i]);
      free(item->info->prop6.property);
         
      for (i=0; i<item->info->prop7.count; i++)
         free(item->info->prop7.property[i]);
      free(item->info->prop7.property);

      if (item->info->gems) {
         for (i=0; i<item->info->gems; ++i)
            D2Sitemfree(item->info->socketeditems[i]);
         free(item->info->socketeditems);
      }
      
      free(item->info);
   }
   
   free(item);
}

void D2Sitemlistfree(D2S_itemlist *ilist) {
   int i;
   
   if (!ilist)
      return;
      
   for (i=0; i<ilist->count; i++)
      D2Sitemfree(ilist->item[i]);;
   free(ilist->item);
      
   free(ilist);
}

//copy from wikipedia :)
int D2Sbase64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;
 
   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3) 
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = data[x] << 16;
 
      if((x+1) < dataLength)
         n += data[x+1] << 8;
 
      if((x+2) < dataLength)
         n += data[x+2];
 
      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
 
      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];
 
      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }
 
      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }  
 
   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      } 
   }
   if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   return 1;   /* indicate success */
}



