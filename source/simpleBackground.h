
//{{BLOCK(simpleBackground)

//======================================================================
//
//	simpleBackground, 256x256@4, 
//	+ palette 7 entries, not compressed
//	+ 76 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 14 + 2432 + 2048 = 4494
//
//	Time-stamp: 2024-07-17, 01:03:09
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SIMPLEBACKGROUND_H
#define GRIT_SIMPLEBACKGROUND_H

#define simpleBackgroundTilesLen 2432
extern const unsigned short simpleBackgroundTiles[1216];

#define simpleBackgroundMapLen 2048
extern const unsigned short simpleBackgroundMap[1024];

#define simpleBackgroundPalLen 14
extern const unsigned short simpleBackgroundPal[8];

#endif // GRIT_SIMPLEBACKGROUND_H

//}}BLOCK(simpleBackground)
