#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <assert.h>
#include <stdint.h>

#define TABLE_SIZE 64

typedef struct Tag_
{
   int num;
   void* data;
   struct Tag_* next;
} Tag;

typedef struct TableDescr_ TableDescr;

typedef struct TagTable_
{
   Tag* tags[TABLE_SIZE];
   TableDescr* descr;
} TagTable;

typedef struct TagDescr_
{
   int num;
} TagDescr;

typedef struct TableDescr_
{
   TagDescr tag_descrs[247];
} TableDescr;

TableDescr* makeDescr();
Tag* makeTag(int num);
TagTable* makeTable(TableDescr* descr);

void tag_set(TagTable* table, Tag* tag)
{
   int idx = tag->num % TABLE_SIZE;
   tag->next = table->tags[idx];
   table->tags[idx] = tag;
}

Tag* tag_get(TagTable* table, int num)
{
   int idx = num % TABLE_SIZE;
   Tag* tag = table->tags[idx];
   int cnt = 0;
   while(tag)
   {
      if (tag->num == num)
      {
         return tag;
      }
      tag = tag->next;
   }
   return NULL;
}

int main()
{
   TableDescr* descr = makeDescr();
   TagTable* table = makeTable(descr);
   tag_set(table, makeTag(8));
   tag_set(table, makeTag(9));
   tag_set(table, makeTag(49));
   tag_set(table, makeTag(56));
   tag_set(table, makeTag(34));
   tag_set(table, makeTag(57));
   tag_set(table, makeTag(52));
   tag_set(table, makeTag(17));
   tag_set(table, makeTag(11));
   tag_set(table, makeTag(37));
   tag_set(table, makeTag(150));
   tag_set(table, makeTag(39));
   tag_set(table, makeTag(1));
   tag_set(table, makeTag(55));
   tag_set(table, makeTag(54));
   tag_set(table, makeTag(38));
   tag_set(table, makeTag(44));
   tag_set(table, makeTag(59));
   tag_set(table, makeTag(32));
   tag_set(table, makeTag(31));
   tag_set(table, makeTag(151));
   tag_set(table, makeTag(14));
   tag_set(table, makeTag(6));
   tag_set(table, makeTag(21));
   tag_set(table, makeTag(58));

   struct timespec start, stop;

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   for(int i = 0; i < 100000; ++i)
   {
      for(int j = 0; j < sizeof(descr->tag_descrs) / sizeof(TagDescr) ; ++j)
      {
         TagDescr* tdescr = &descr->tag_descrs[j];
         Tag* tag = tag_get(table, tdescr->num);
      }
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

   printf("%ld\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000);
   return 0;
}

Tag* makeTag(int num)
{
   Tag* tag = сalloc(1, sizeof(Tag));
   tag->num = num;
   return tag;
}

TagTable* makeTable(TableDescr* descr)
{
   TagTable* table = calloc(1, sizeof(TagTable));
   table->descr = descr;
   return table;
}

TableDescr* makeDescr()
{
   TableDescr* tdescr = malloc(sizeof(TableDescr));
   tdescr->tag_descrs[0].num = 8;
   tdescr->tag_descrs[1].num = 9;
   tdescr->tag_descrs[2].num = 35;
   tdescr->tag_descrs[3].num = 49;
   tdescr->tag_descrs[4].num = 56;
   tdescr->tag_descrs[5].num = 115;
   tdescr->tag_descrs[6].num = 128;
   tdescr->tag_descrs[7].num = 90;
   tdescr->tag_descrs[8].num = 91;
   tdescr->tag_descrs[9].num = 34;
   tdescr->tag_descrs[10].num = 50;
   tdescr->tag_descrs[11].num = 142;
   tdescr->tag_descrs[12].num = 57;
   tdescr->tag_descrs[13].num = 143;
   tdescr->tag_descrs[14].num = 116;
   tdescr->tag_descrs[15].num = 144;
   tdescr->tag_descrs[16].num = 129;
   tdescr->tag_descrs[17].num = 145;
   tdescr->tag_descrs[18].num = 43;
   tdescr->tag_descrs[19].num = 97;
   tdescr->tag_descrs[20].num = 52;
   tdescr->tag_descrs[21].num = 122;
   tdescr->tag_descrs[22].num = 212;
   tdescr->tag_descrs[23].num = 213;
   tdescr->tag_descrs[24].num = 347;
   tdescr->tag_descrs[25].num = 369;
   tdescr->tag_descrs[26].num = 627;
   tdescr->tag_descrs[27].num = 37;
   tdescr->tag_descrs[28].num = 198;
   tdescr->tag_descrs[29].num = 526;
   tdescr->tag_descrs[30].num = 527;
   tdescr->tag_descrs[31].num = 11;
   tdescr->tag_descrs[32].num = 41;
   tdescr->tag_descrs[33].num = 583;
   tdescr->tag_descrs[34].num = 693;
   tdescr->tag_descrs[35].num = 790;
   tdescr->tag_descrs[36].num = 584;
   tdescr->tag_descrs[37].num = 911;
   tdescr->tag_descrs[38].num = 912;
   tdescr->tag_descrs[39].num = 453;
   tdescr->tag_descrs[40].num = 229;
   tdescr->tag_descrs[41].num = 382;
   tdescr->tag_descrs[42].num = 66;
   tdescr->tag_descrs[43].num = 548;
   tdescr->tag_descrs[44].num = 551;
   tdescr->tag_descrs[45].num = 549;
   tdescr->tag_descrs[46].num = 17;
   tdescr->tag_descrs[47].num = 19;
   tdescr->tag_descrs[48].num = 150;
   tdescr->tag_descrs[49].num = 39;
   tdescr->tag_descrs[50].num = 636;
   tdescr->tag_descrs[51].num = 103;
   tdescr->tag_descrs[52].num = 378;
   tdescr->tag_descrs[53].num = 1;
   tdescr->tag_descrs[54].num = 660;
   tdescr->tag_descrs[55].num = 581;
   tdescr->tag_descrs[56].num = 589;
   tdescr->tag_descrs[57].num = 590;
   tdescr->tag_descrs[58].num = 591;
   tdescr->tag_descrs[59].num = 63;
   tdescr->tag_descrs[60].num = 64;
   tdescr->tag_descrs[61].num = 544;
   tdescr->tag_descrs[62].num = 635;
   tdescr->tag_descrs[63].num = 55;
   tdescr->tag_descrs[64].num = 65;
   tdescr->tag_descrs[65].num = 48;
   tdescr->tag_descrs[66].num = 22;
   tdescr->tag_descrs[67].num = 454;
   tdescr->tag_descrs[68].num = 460;
   tdescr->tag_descrs[69].num = 461;
   tdescr->tag_descrs[70].num = 167;
   tdescr->tag_descrs[71].num = 762;
   tdescr->tag_descrs[72].num = 200;
   tdescr->tag_descrs[73].num = 541;
   tdescr->tag_descrs[74].num = 224;
   tdescr->tag_descrs[75].num = 225;
   tdescr->tag_descrs[76].num = 239;
   tdescr->tag_descrs[77].num = 226;
   tdescr->tag_descrs[78].num = 227;
   tdescr->tag_descrs[79].num = 228;
   tdescr->tag_descrs[80].num = 255;
   tdescr->tag_descrs[81].num = 543;
   tdescr->tag_descrs[82].num = 470;
   tdescr->tag_descrs[83].num = 471;
   tdescr->tag_descrs[84].num = 472;
   tdescr->tag_descrs[85].num = 240;
   tdescr->tag_descrs[86].num = 202;
   tdescr->tag_descrs[87].num = 947;
   tdescr->tag_descrs[88].num = 206;
   tdescr->tag_descrs[89].num = 231;
   tdescr->tag_descrs[90].num = 223;
   tdescr->tag_descrs[91].num = 207;
   tdescr->tag_descrs[92].num = 106;
   tdescr->tag_descrs[93].num = 348;
   tdescr->tag_descrs[94].num = 349;
   tdescr->tag_descrs[95].num = 107;
   tdescr->tag_descrs[96].num = 350;
   tdescr->tag_descrs[97].num = 351;
   tdescr->tag_descrs[98].num = 691;
   tdescr->tag_descrs[99].num = 667;
   tdescr->tag_descrs[100].num = 875;
   tdescr->tag_descrs[101].num = 876;
   tdescr->tag_descrs[102].num = 864;
   tdescr->tag_descrs[103].num = 873;
   tdescr->tag_descrs[104].num = 874;
   tdescr->tag_descrs[105].num = 913;
   tdescr->tag_descrs[106].num = 914;
   tdescr->tag_descrs[107].num = 915;
   tdescr->tag_descrs[108].num = 918;
   tdescr->tag_descrs[109].num = 788;
   tdescr->tag_descrs[110].num = 916;
   tdescr->tag_descrs[111].num = 917;
   tdescr->tag_descrs[112].num = 919;
   tdescr->tag_descrs[113].num = 898;
   tdescr->tag_descrs[114].num = 711;
   tdescr->tag_descrs[115].num = 54;
   tdescr->tag_descrs[116].num = 232;
   tdescr->tag_descrs[117].num = 854;
   tdescr->tag_descrs[118].num = 38;
   tdescr->tag_descrs[119].num = 152;
   tdescr->tag_descrs[120].num = 516;
   tdescr->tag_descrs[121].num = 468;
   tdescr->tag_descrs[122].num = 469;
   tdescr->tag_descrs[123].num = 40;
   tdescr->tag_descrs[124].num = 423;
   tdescr->tag_descrs[125].num = 44;
   tdescr->tag_descrs[126].num = 99;
   tdescr->tag_descrs[127].num = 211;
   tdescr->tag_descrs[128].num = 835;
   tdescr->tag_descrs[129].num = 836;
   tdescr->tag_descrs[130].num = 837;
   tdescr->tag_descrs[131].num = 838;
   tdescr->tag_descrs[132].num = 840;
   tdescr->tag_descrs[133].num = 388;
   tdescr->tag_descrs[134].num = 389;
   tdescr->tag_descrs[135].num = 841;
   tdescr->tag_descrs[136].num = 842;
   tdescr->tag_descrs[137].num = 843;
   tdescr->tag_descrs[138].num = 844;
   tdescr->tag_descrs[139].num = 846;
   tdescr->tag_descrs[140].num = 839;
   tdescr->tag_descrs[141].num = 845;
   tdescr->tag_descrs[142].num = 847;
   tdescr->tag_descrs[143].num = 848;
   tdescr->tag_descrs[144].num = 849;
   tdescr->tag_descrs[145].num = 850;
   tdescr->tag_descrs[146].num = 15;
   tdescr->tag_descrs[147].num = 376;
   tdescr->tag_descrs[148].num = 377;
   tdescr->tag_descrs[149].num = 59;
   tdescr->tag_descrs[150].num = 168;
   tdescr->tag_descrs[151].num = 432;
   tdescr->tag_descrs[152].num = 126;
   tdescr->tag_descrs[153].num = 18;
   tdescr->tag_descrs[154].num = 528;
   tdescr->tag_descrs[155].num = 529;
   tdescr->tag_descrs[156].num = 582;
   tdescr->tag_descrs[157].num = 32;
   tdescr->tag_descrs[158].num = 652;
   tdescr->tag_descrs[159].num = 31;
   tdescr->tag_descrs[160].num = 651;
   tdescr->tag_descrs[161].num = 669;
   tdescr->tag_descrs[162].num = 194;
   tdescr->tag_descrs[163].num = 195;
   tdescr->tag_descrs[164].num = 30;
   tdescr->tag_descrs[165].num = 336;
   tdescr->tag_descrs[166].num = 625;
   tdescr->tag_descrs[167].num = 943;
   tdescr->tag_descrs[168].num = 29;
   tdescr->tag_descrs[169].num = 151;
   tdescr->tag_descrs[170].num = 14;
   tdescr->tag_descrs[171].num = 6;
   tdescr->tag_descrs[172].num = 424;
   tdescr->tag_descrs[173].num = 425;
   tdescr->tag_descrs[174].num = 426;
   tdescr->tag_descrs[175].num = 427;
   tdescr->tag_descrs[176].num = 75;
   tdescr->tag_descrs[177].num = 60;
   tdescr->tag_descrs[178].num = 113;
   tdescr->tag_descrs[179].num = 12;
   tdescr->tag_descrs[180].num = 13;
   tdescr->tag_descrs[181].num = 479;
   tdescr->tag_descrs[182].num = 497;
   tdescr->tag_descrs[183].num = 218;
   tdescr->tag_descrs[184].num = 220;
   tdescr->tag_descrs[185].num = 221;
   tdescr->tag_descrs[186].num = 222;
   tdescr->tag_descrs[187].num = 662;
   tdescr->tag_descrs[188].num = 663;
   tdescr->tag_descrs[189].num = 699;
   tdescr->tag_descrs[190].num = 761;
   tdescr->tag_descrs[191].num = 235;
   tdescr->tag_descrs[192].num = 236;
   tdescr->tag_descrs[193].num = 701;
   tdescr->tag_descrs[194].num = 696;
   tdescr->tag_descrs[195].num = 697;
   tdescr->tag_descrs[196].num = 698;
   tdescr->tag_descrs[197].num = 381;
   tdescr->tag_descrs[198].num = 157;
   tdescr->tag_descrs[199].num = 230;
   tdescr->tag_descrs[200].num = 158;
   tdescr->tag_descrs[201].num = 159;
   tdescr->tag_descrs[202].num = 738;
   tdescr->tag_descrs[203].num = 920;
   tdescr->tag_descrs[204].num = 921;
   tdescr->tag_descrs[205].num = 922;
   tdescr->tag_descrs[206].num = 258;
   tdescr->tag_descrs[207].num = 259;
   tdescr->tag_descrs[208].num = 260;
   tdescr->tag_descrs[209].num = 238;
   tdescr->tag_descrs[210].num = 237;
   tdescr->tag_descrs[211].num = 118;
   tdescr->tag_descrs[212].num = 119;
   tdescr->tag_descrs[213].num = 120;
   tdescr->tag_descrs[214].num = 155;
   tdescr->tag_descrs[215].num = 156;
   tdescr->tag_descrs[216].num = 21;
   tdescr->tag_descrs[217].num = 110;
   tdescr->tag_descrs[218].num = 111;
   tdescr->tag_descrs[219].num = 77;
   tdescr->tag_descrs[220].num = 210;
   tdescr->tag_descrs[221].num = 775;
   tdescr->tag_descrs[222].num = 58;
   tdescr->tag_descrs[223].num = 354;
   tdescr->tag_descrs[224].num = 355;
   tdescr->tag_descrs[225].num = 193;
   tdescr->tag_descrs[226].num = 192;
   tdescr->tag_descrs[227].num = 641;
   tdescr->tag_descrs[228].num = 442;
   tdescr->tag_descrs[229].num = 480;
   tdescr->tag_descrs[230].num = 481;
   tdescr->tag_descrs[231].num = 513;
   tdescr->tag_descrs[232].num = 494;
   tdescr->tag_descrs[233].num = 483;
   tdescr->tag_descrs[234].num = 515;
   tdescr->tag_descrs[235].num = 484;
   tdescr->tag_descrs[236].num = 485;
   tdescr->tag_descrs[237].num = 638;
   tdescr->tag_descrs[238].num = 639;
   tdescr->tag_descrs[239].num = 851;
   tdescr->tag_descrs[240].num = 518;
   tdescr->tag_descrs[241].num = 555;
   tdescr->tag_descrs[242].num = 797;
   tdescr->tag_descrs[243].num = 136;
   tdescr->tag_descrs[244].num = 93;
   tdescr->tag_descrs[245].num = 89;
   tdescr->tag_descrs[246].num = 10;
   return tdescr;
}