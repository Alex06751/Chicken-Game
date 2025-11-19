#include "raylib.h"  
#include "drawtile.h"  

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int texture_id) {
	Rectangle source = { static_cast<float>(texture_index_x) * TILE_WIDTH, static_cast<float>(texture_index_y) * TILE_HEIGHT, static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT) };
	Rectangle dest = { static_cast<float>(pos_x), static_cast<float>(pos_y), static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT) };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(textures[texture_id], source, dest, origin, 0.0f, WHITE); // Draw texture with source rectangle  
}
