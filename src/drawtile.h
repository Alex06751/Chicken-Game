#ifndef DRAWTILE_H  
#define DRAWTILE_H  

#include "raylib.h"  

#define TILE_WIDTH 16  
#define TILE_HEIGHT 16  

extern Texture2D textures[];

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int texture_id);

#endif // DRAWTILE_H  
