
//{{BLOCK(titleCard)

//======================================================================
//
//	titleCard, 256x256@4, 
//	+ palette 7 entries, not compressed
//	+ 70 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 14 + 2240 + 2048 = 4302
//
//	Time-stamp: 2024-07-25, 15:55:15
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TITLECARD_H
#define GRIT_TITLECARD_H

#define titleCardTilesLen 2240
extern const unsigned short titleCardTiles[1120];

#define titleCardMapLen 2048
extern const unsigned short titleCardMap[1024];

#define titleCardPalLen 14
extern const unsigned short titleCardPal[8];

#endif // GRIT_TITLECARD_H

//}}BLOCK(titleCard)
