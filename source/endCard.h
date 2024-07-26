
//{{BLOCK(endCard)

//======================================================================
//
//	endCard, 256x256@4, 
//	+ palette 4 entries, not compressed
//	+ 40 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 8 + 1280 + 2048 = 3336
//
//	Time-stamp: 2024-07-24, 00:45:34
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_ENDCARD_H
#define GRIT_ENDCARD_H

#define endCardTilesLen 1280
extern const unsigned short endCardTiles[640];

#define endCardMapLen 2048
extern const unsigned short endCardMap[1024];

#define endCardPalLen 8
extern const unsigned short endCardPal[4];

#endif // GRIT_ENDCARD_H

//}}BLOCK(endCard)
