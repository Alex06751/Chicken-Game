// TutorialGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "raylib.h"
#include "raymath.h"
#include "DrawTile.h"
#include "Entity.h"

void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();

const int screenWidth = 800;
const int screenHeight = 600;

// Add a global timer
float AnimationTimer = 0.0f;
const float FrameDuration = 0.25f; // Duration of each frame in seconds
int waterFrameIndex = 0;
float CharacterTimer = 0.0f; // Timer for character animation

// Set up the textures
#define MAX_TEXTURES 6
enum texture_asset {
	TEXTURE_GRASS = 0,
	TEXTURE_CHARACTER,
	TEXTURE_WATER,
	TEXTURE_HOUSE,
	TEXTURE_CHICKEN,
	TEXTURE_ENVIRONMENT,
};

Texture2D textures[MAX_TEXTURES];

#define MAX_SOUNDS 2
enum sound_asset {
	SOUND_GRASS = 0,
	SOUND_CHICKEN,
};

Sound sounds[MAX_SOUNDS];

#define MAX_MUSIC 1
enum music_asset {
	MUSIC_AMBIENCE = 0,
};

Music music[MAX_MUSIC];

// Set up the world
#define WORLD_WIDTH 30
#define WORLD_HEIGHT 30


// Enum for floor material types and their associated sprites
enum tile_type {
	TILE_WATER = 0,
	TILE_GRASS,
	TILE_GRASS_SIDE,
};

class Tile {
public:
	int x;
	int y;
	int type;
	int subType;
};

Tile world[WORLD_WIDTH][WORLD_HEIGHT];

int score;

// Set up the camera
Camera2D camera = { 0 };

// Set up Entity objects
Entity player;
Entity chicken;
Entity house;

#define MAX_TREES 10
Entity trees[MAX_TREES];

int worldBorder = 7; // Set world border size

// GAME STARTUP FUNCTION
void GameStartup() {
	InitAudioDevice(); // Initialize audio device

	// Load textures
	// 176 x 112 - each tile is 16 x 16 giving us 11 tile across and 7 tiles down
    Image grassImage = LoadImage("assets/Tilesets/Grass.png"); // Load grass image
    textures[TEXTURE_GRASS] = LoadTextureFromImage(grassImage); // Load texture from grass image
    UnloadImage(grassImage); // Unload grass image after loading texture

    Image characterImage = LoadImage("assets/Characters/Character Spritesheet.png"); // Load character image
    textures[TEXTURE_CHARACTER] = LoadTextureFromImage(characterImage); // Load texture from character image
    UnloadImage(characterImage); // Unload character image after loading texture

	Image waterImage = LoadImage("assets/Tilesets/Water.png"); // Load water image
	textures[TEXTURE_WATER] = LoadTextureFromImage(waterImage); // Load texture from water image
	UnloadImage(waterImage); // Unload water image after loading texture

	Image houseImage = LoadImage("assets/Objects/Chicken_House.png"); // Load house image
	textures[TEXTURE_HOUSE] = LoadTextureFromImage(houseImage); // Load texture from house image
	UnloadImage(houseImage); // Unload house image after loading texture

	Image chickenImage = LoadImage("assets/Characters/Chicken Sprites.png"); // Load chicken image
	textures[TEXTURE_CHICKEN] = LoadTextureFromImage(chickenImage); // Load texture from chicken image
	UnloadImage(chickenImage); // Unload chicken image after loading texture

	Image environmentImage = LoadImage("assets/Objects/Grass_things.png"); // Load environment image
	textures[TEXTURE_ENVIRONMENT] = LoadTextureFromImage(environmentImage); // Load texture from environment image
	UnloadImage(environmentImage); // Unload environment image after loading texture


	// Initialize world tiles
	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			if (i <= worldBorder - 1 || j <= worldBorder - 1 || i >= (WORLD_WIDTH - worldBorder) || j >= (WORLD_HEIGHT - worldBorder)) {
				world[i][j] = Tile
				{
					i,
					j,
					TILE_WATER,
				};
			}
			else if (i == worldBorder || j == worldBorder || i == (WORLD_WIDTH - worldBorder - 1) || j == (WORLD_HEIGHT - worldBorder - 1)) {
				world[i][j] = Tile
				{
					i,
					j,
					TILE_GRASS_SIDE,
					0, // Default subtype for grass side
				};
				if (i == worldBorder && j == worldBorder) {
					world[i][j].subType = 4; // Grass corner topleft
				}
				
				else if (i == (WORLD_WIDTH - worldBorder - 1) && j == worldBorder) {
					world[i][j].subType = 5; // Grass corner topright
				}
				else if (i == worldBorder && j == (WORLD_HEIGHT - worldBorder - 1)) {
					world[i][j].subType = 6; // Grass corner bottomleft
				}
				else if (i == (WORLD_WIDTH - worldBorder - 1) && j == (WORLD_HEIGHT - worldBorder - 1)) {
					world[i][j].subType = 7; // Grass corner bottomright
				}
				else if (j == worldBorder) {
					world[i][j].subType = 0; // Grass top
				}
				else if (j == (WORLD_HEIGHT - worldBorder - 1)) {
					world[i][j].subType = 1; // Grass bottom
				}
				else if (i == worldBorder) {
					world[i][j].subType = 2; // Grass left
				}
				else if (i == (WORLD_WIDTH - worldBorder - 1)) {
					world[i][j].subType = 3; // Grass right
				}
			}
			else {
				world[i][j] = Tile
				{
					i,
					j,
					TILE_GRASS,
					GetRandomValue(0, 2), // Random subtype for grass
				};
			}
		}
	}

	// Initialize player
	player.x = 9 * TILE_WIDTH; // Player x position
	player.y = 9 * TILE_HEIGHT; // Player y position
	player.SetIndex(1, 1); // Set player index
	player.isAlive = true; // Set player as alive

	// Initialize house
	house.x = 14 * TILE_WIDTH; // House x position
	house.y = 7 * TILE_HEIGHT; // House y position
	house.isAlive = true; // Set house as alive
	house.isPassable = true; // Set house as passable

	// Initialize chicken
	chicken.x = 9 * TILE_WIDTH; // Player x position
	chicken.y = 12 * TILE_HEIGHT; // Chicken x position
	chicken.isAlive = true; // Set chicken as alive
	chicken.isGrabed = false; // Set chicken as not grabbed
	chicken.isPassable = true; // Set chicken as passable

	// Initialize trees
	int treesPlaced = 0; // Initialize trees placed counter
	for (int i = 0; i < MAX_TREES; i++) {
		trees[i].x = (GetRandomValue((worldBorder + 1) / 2, 10) * 2) * TILE_WIDTH; // Random x position
		trees[i].y = (GetRandomValue((worldBorder + 5) / 2, 10) * 2 ) * TILE_HEIGHT; // Random y position
		trees[i].isAlive = true; // Set tree as alive
		trees[i].isPassable = false; // Set tree as not passable
		// Check if the tree position is not colliding with other trees
		for (int j = 0; j < treesPlaced; j++) {
			while (trees[i].x == trees[j].x && trees[i].y == trees[j].y) {
				// Move tree to a new random position
				trees[i].x = (GetRandomValue((worldBorder + 1) / 2, 10) * 2) * TILE_WIDTH; // Random x position
				trees[i].y = (GetRandomValue((worldBorder + 5) / 2, 10) * 2) * TILE_HEIGHT; // Random y position
				// Check if the new position is not colliding with other trees
				if (trees[i].x != trees[j].x && trees[j].y != trees[j].y) {
					break; // Exit the loop if not colliding
				}
			}
		}
	}

	
	chicken.isAlive = true; // Set chicken as alive

	//Initialize camera
	camera.target = Vector2{ static_cast<float>(player.x), static_cast<float>(player.y) }; // Camera target point (rotation and zoom)
	camera.offset = Vector2{ static_cast<float>(screenWidth) / 2, static_cast<float>(screenHeight) / 2 }; // Camera offset (displacement from target)
	camera.rotation = 0.0f; // Camera rotation in degrees
	camera.zoom = 3.75f; // Camera zoom (scaling)

	// Initialize score
	score = 0;

	// Load sounds
	sounds[SOUND_GRASS] = LoadSound("assets/Sounds/walking_on_leaves.wav"); // Load grass sound
	sounds[SOUND_CHICKEN] = LoadSound("assets/Sounds/chicken_1.wav"); // Load chicken sound

	// Load music
	music[MUSIC_AMBIENCE] = LoadMusicStream("assets/Sounds/Ambient 1.mp3"); // Load music stream

	PlayMusicStream(music[MUSIC_AMBIENCE]); // Play music stream
}

// GAME UPDATE FUNCTION
void GameUpdate() {
	// Update music stream
	UpdateMusicStream(music[MUSIC_AMBIENCE]);
	
	// Update the animation timers
	AnimationTimer += GetFrameTime(); // Increment timer by frame time

	// Update water animation frame
	if (AnimationTimer >= FrameDuration) {
		AnimationTimer = 0.0f; // Reset timer
		waterFrameIndex = (waterFrameIndex + 1) % 4; // Cycle through frames 0, 1, 2, 3
	}

	// Update player position based on input
	float x = player.x;
	float y = player.y;
	float holdSpeed = 0.18f; // Speed for holding down the key
	float pressSpeed = 0.05f; // Speed for pressing the key
	int animationFrames[] = { 7, 10 }; // Animation frames for movement
	static int currentFrame = 0; // Current animation frame index

	if (IsKeyPressed(KEY_LEFT)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= pressSpeed) { // Check if 0.5 seconds have passed
			x -= 1 * TILE_WIDTH; // Move left
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 7); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}
	else if (IsKeyPressed(KEY_RIGHT)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= pressSpeed) { // Check if 0.5 seconds have passed
			x += 1 * TILE_WIDTH; // Move right
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 10); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}
	else if (IsKeyPressed(KEY_UP)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= pressSpeed) { // Check if 0.5 seconds have passed
			y -= 1 * TILE_HEIGHT; // Move up
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 4); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}
	else if (IsKeyPressed(KEY_DOWN)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= pressSpeed) { // Check if 0.5 seconds have passed
			y += 1 * TILE_HEIGHT; // Move down
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 1); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}
	else if (IsKeyDown(KEY_LEFT)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= holdSpeed) { // Check if 0.5 seconds have passed
			x -= 1 * TILE_WIDTH; // Move left
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 7); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= holdSpeed) { // Check if 0.5 seconds have passed
			x += 1 * TILE_WIDTH; // Move right
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 10); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}

	else if (IsKeyDown(KEY_UP)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= holdSpeed) { // Check if 0.5 seconds have passed
			y -= 1 * TILE_HEIGHT; // Move up
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 4); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}
	else if (IsKeyDown(KEY_DOWN)) {
		CharacterTimer += GetFrameTime(); // Increment character timer by frame time
		if (CharacterTimer >= holdSpeed) { // Check if 0.5 seconds have passed
			y += 1 * TILE_HEIGHT; // Move down
			PlaySound(sounds[SOUND_GRASS]); // Play grass sound
			currentFrame = (currentFrame + 1) % 2; // Cycle through animation frames
			player.SetIndex(animationFrames[currentFrame], 1); // Set player sprite facing left
			CharacterTimer = 0.0f; // Reset the timer
		}
	}
	else if (IsKeyReleased(KEY_LEFT)) {
		player.SetIndex(1, 7); // Set player sprite to idle
	}
	else if (IsKeyReleased(KEY_RIGHT)) {
		player.SetIndex(1, 10); // Set player sprite to idle
	}
	else if (IsKeyReleased(KEY_UP)) {
		player.SetIndex(1, 4); // Set player sprite to idle
	}
	else if (IsKeyReleased(KEY_DOWN)) {
		player.SetIndex(1, 1); // Set player sprite to idle
	}

	// Set and update camera zoom
	float wheel = GetMouseWheelMove(); // Get mouse wheel movement
	if (wheel != 0) {
		const float zoomIncrement = 0.125f; // Zoom increment value
		camera.zoom += (wheel * zoomIncrement); // Update camera zoom
		if (camera.zoom < 3.75f) camera.zoom = 3.75f; // Clamp zoom to minimum value
		if (camera.zoom > 8.0f) camera.zoom = 8.0f; // Clamp zoom to maximum value
	}

	// Check if player is in the chicken area and if the chicken is not grabbed
	if (chicken.x == x && chicken.y == y && IsKeyPressed(KEY_E)) {
		chicken.isGrabed = true; // Set chicken as grabbed
	}

	// Check if player is in the house area and if the chicken is grabbed
	if ((player.x == house.x + (1 * TILE_WIDTH)) && player.y == (10 * TILE_HEIGHT) && chicken.isGrabed == true && IsKeyPressed(KEY_E)) {
		chicken.isAlive = false; // Set chicken as dead
		chicken.isGrabed = false; // Set chicken as not grabbed
		PlaySound(sounds[SOUND_CHICKEN]); // Play grass sound
		score += 1; // Increment score
	}

	// Check if player is colliding
	bool isColliding = false; // Initialize collision flag

    // Update player's position if not in house zone
    if ((x >= house.x && x <= house.x + (2 * TILE_WIDTH)) && (y >= house.y + (1 * TILE_HEIGHT) && y <= house.y + (2 * TILE_HEIGHT))) {
		isColliding = true; // Set collision flag
    }

	// Check collision with trees
	for (int i = 0; i < MAX_TREES; i++) {
		if (x == trees[i].x && y == trees[i].y + (1 * TILE_HEIGHT)) {
			isColliding = true; // Set collision flag
			break;
		}
	}

	if (isColliding == false) {
		player.x = x; // Update player x position
		player.y = y; // Update player y position
	}

	// Clamp player position within the border bounds
	player.x = Clamp(player.x, worldBorder * TILE_WIDTH, (WORLD_WIDTH - worldBorder) * TILE_WIDTH - TILE_WIDTH); // Clamp player x position within border bounds  
	player.y = Clamp(player.y, worldBorder * TILE_HEIGHT, (WORLD_HEIGHT - worldBorder) * TILE_HEIGHT - TILE_HEIGHT); // Clamp player y position within border bounds
	
	if (chicken.isGrabed) {
		chicken.x = player.x + 1; // Set chicken x position to player x position
		chicken.y = player.y - 8; // Set chicken y position to player y position
	}

	// Teleport the chicken if collected
	if (chicken.isAlive == false) {
		chicken.x = GetRandomValue(8, 21) * TILE_WIDTH; // Set chicken x position to random value
		chicken.y = GetRandomValue(12, 21) * TILE_HEIGHT; // Set chicken y position to random value

		for (int i = 0; i < MAX_TREES; i++) {
			while (chicken.x == trees[i].x && chicken.y == trees[i].y + (1 * TILE_HEIGHT)) {
				// Move chicken to a new random position
				chicken.x = GetRandomValue(8, 21) * TILE_WIDTH; // Set chicken x position to random value
				chicken.y = GetRandomValue(12, 21) * TILE_HEIGHT; // Set chicken y position to random value
				// Check if the new position is not colliding with trees
				if (chicken.x != trees[i].x && chicken.y != trees[i].y + (1 * TILE_HEIGHT)) {
					break; // Exit the loop if not colliding
				}
			}
		}
		chicken.isAlive = true; // Set chicken as alive
	}

	// Update camera target
	camera.target = Vector2{ static_cast<float>(player.x), static_cast<float>(player.y) };
}

// GAME RENDER FUNTION
void GameRender() {

	// Draw the camera
	BeginMode2D(camera); // Begin 2D mode with custom camera

	// Draw the world
	Tile tile;
	int index_x = 0;
	int index_y = 0;
	int texture_id = TEXTURE_GRASS; // Default texture ID
	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			tile = world[i][j];
			switch (tile.type) {
			case TILE_GRASS:
				texture_id = TEXTURE_GRASS; // Set texture ID to grass
				switch (tile.subType) {
				case 0: // Grass subtype 0
					index_x = 1;
					index_y = 1;
					break;
				case 1: // Grass subtype 1
					index_x = 1;
					index_y = 5;
					break;
				case 2: // Grass subtype 2
					index_x = 1;
					index_y = 6;
					break;
				}
				break;
			case TILE_GRASS_SIDE:
				texture_id = TEXTURE_GRASS; // Set texture ID to grass
				switch (tile.subType) {
				case 0: // Grass top
					index_x = 1;
					index_y = 0;
					break;
				case 1: // Grass bottom
					index_x = 1;
					index_y = 2;
					break;
				case 2: // Grass left
					index_x = 0;
					index_y = 1;
					break;
				case 3: // Grass right
					index_x = 2;
					index_y = 1;
					break;
				case 4: // Grass corner topleft
					index_x = 0;
					index_y = 0;
					break;
				case 5: // Grass corner topright
					index_x = 2;
					index_y = 0;
					break;
				case 6: // Grass corner bottomleft
					index_x = 0;
					index_y = 2;
					break;
				case 7: // Grass corner bottomright
					index_x = 2;
					index_y = 2;
					break;
				}
				DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, waterFrameIndex, 0, TEXTURE_WATER);
				break;
			case TILE_WATER:
				texture_id = TEXTURE_WATER; // Set texture ID to water
				index_x = waterFrameIndex; // Use the current frame index for water animation
				index_y = 0;
				break;
			}
			DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, index_x, index_y, texture_id); // Draw tile
		}
	}
	//Draw the chicken when not grabbed
	if (chicken.isGrabed == false && chicken.isAlive) {
		DrawTile(chicken.x, chicken.y, 0, 0, TEXTURE_CHICKEN); // Draw chicken
	}

	// Draw the house when player is behind it
	if (player.y > (house.y)) {
		Rectangle source = { static_cast<float>(0) * (TILE_WIDTH * 3), static_cast<float>(0) * (TILE_HEIGHT * 3), static_cast<float>(TILE_WIDTH) * 3, static_cast<float>(TILE_HEIGHT) * 3 };
		Rectangle dest = { static_cast<float>(house.x) - 8, static_cast<float>(house.y), static_cast<float>(TILE_WIDTH) * 4, static_cast<float>(TILE_HEIGHT) * 4 };
		Vector2 origin = { 0, 0 };
		DrawTexturePro(textures[TEXTURE_HOUSE], source, dest, origin, 0.0f, WHITE); // Draw texture with source rectangle
	}

	// Draw the trees

	for (int i = 0; i < MAX_TREES; i++) {
		if (player.y > (trees[i].y)) {
			Rectangle source = { static_cast<float>(0) * TILE_WIDTH, static_cast<float>(0) * (TILE_HEIGHT * 2), static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT) * 2 };
			Rectangle dest = { static_cast<float>(trees[i].x), static_cast<float>(trees[i].y) + 4, static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT) * 2 };
			Vector2 origin = { 0, 0 };
			DrawTexturePro(textures[TEXTURE_ENVIRONMENT], source, dest, origin, 0.0f, WHITE); // Draw texture with source rectangle 
		}
	}

	// Draw the player
	DrawTile(camera.target.x, camera.target.y, player.IndexX(), player.IndexY(), TEXTURE_CHARACTER); // Draw player character


	// Draw the chicken when grabbed
	if (chicken.isGrabed == true && chicken.isAlive) {
		DrawTile(chicken.x, chicken.y, 0, 0, TEXTURE_CHICKEN); // Draw chicken
	}

	for (int i = 0; i < MAX_TREES; i++) {
		if (player.y <= (trees[i].y)) {
			Rectangle source = { static_cast<float>(0) * TILE_WIDTH, static_cast<float>(0) * (TILE_HEIGHT * 2), static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT) * 2 };
			Rectangle dest = { static_cast<float>(trees[i].x), static_cast<float>(trees[i].y) + 4, static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT) * 2 };
			Vector2 origin = { 0, 0 };
			DrawTexturePro(textures[TEXTURE_ENVIRONMENT], source, dest, origin, 0.0f, WHITE); // Draw texture with source rectangle 
		}
	}

	// Draw the house when player is in front of it
	if (player.y <= (house.y)) {
		Rectangle source = { static_cast<float>(0) * (TILE_WIDTH * 3), static_cast<float>(0) * (TILE_HEIGHT * 3), static_cast<float>(TILE_WIDTH) * 3, static_cast<float>(TILE_HEIGHT) * 3 };
		Rectangle dest = { static_cast<float>(house.x) - 8, static_cast<float>(house.y), static_cast<float>(TILE_WIDTH) * 4, static_cast<float>(TILE_HEIGHT) * 4 };
		Vector2 origin = { 0, 0 };
		DrawTexturePro(textures[TEXTURE_HOUSE], source, dest, origin, 0.0f, WHITE); // Draw texture with source rectangle
	}

	// End 2D mode
	EndMode2D();

	// Draw UI elements  
	Color borderColor = DARKBLUE; // Set border color to dark blue
	Color textColor = DARKBLUE; // Set text color to white

	// Draw a semi-transparent rectangle with extended width
	DrawRectangle(5, 5, 400, 120, Fade(WHITE, 0.5f)); // Draw a semi-transparent rectangle with extended width  
	DrawRectangleLinesEx({ 5, 5, 400, 120 }, 2, DARKBLUE); // Draw rectangle outline with slightly thinner border

	// Draw game objective text  
	DrawText("Objective:", 15, 15, 20, textColor); // Title text  
	DrawText("Use arrow keys to move.", 15, 35, 18, textColor); // Controls description  
	DrawText("Press 'E' to interact.", 15, 55, 18, textColor); // Interaction description
	DrawText("Grab the chicken and bring it to the house.", 15, 75, 18, textColor); // Objective description
	DrawText("Collect as many chickens as you can!", 15, 95, 18, textColor); // Objective description

	// Draw timer with the same font as the score  
	float gameTime = GetTime(); // Get the elapsed time since the game started  

	// Format the timer as minutes and seconds  
	int minutes = static_cast<int>(gameTime) / 60;
	int seconds = static_cast<int>(gameTime) % 60;

	// Draw timer with white text and thicker black outline  
	DrawText(TextFormat("Time: %02i:%02i", minutes, seconds), screenWidth - 150 + 2, 20, 24, borderColor); // Black outline (right)  
	DrawText(TextFormat("Time: %02i:%02i", minutes, seconds), screenWidth - 150 - 2, 20, 24, borderColor); // Black outline (left)  
	DrawText(TextFormat("Time: %02i:%02i", minutes, seconds), screenWidth - 150, 20 + 2, 24, borderColor); // Black outline (bottom)  
	DrawText(TextFormat("Time: %02i:%02i", minutes, seconds), screenWidth - 150, 20 - 2, 24, borderColor); // Black outline (top)  
	DrawText(TextFormat("Time: %02i:%02i", minutes, seconds), screenWidth - 150 + 1, 20 + 1, 24, borderColor); // Black outline (top-right diagonal)  
	DrawText(TextFormat("Time: %02i:%02i", minutes, seconds), screenWidth - 150 - 1, 20 - 1, 24, borderColor); // Black outline (bottom-left diagonal)  
	DrawText(TextFormat("Time: %02i:%02i", minutes, seconds), screenWidth - 150, 20, 24, WHITE); // White text  

	// Draw score with white text and thicker black outline  
	DrawText(TextFormat("Score: %i", score), screenWidth - 150 + 2, 50, 24, borderColor); // Black outline (right)  
	DrawText(TextFormat("Score: %i", score), screenWidth - 150 - 2, 50, 24, borderColor); // Black outline (left)  
	DrawText(TextFormat("Score: %i", score), screenWidth - 150, 50 + 2, 24, borderColor); // Black outline (bottom)  
	DrawText(TextFormat("Score: %i", score), screenWidth - 150, 50 - 2, 24, borderColor); // Black outline (top)  
	DrawText(TextFormat("Score: %i", score), screenWidth - 150 + 1, 50 + 1, 24, borderColor); // Black outline (top-right diagonal)  
	DrawText(TextFormat("Score: %i", score), screenWidth - 150 - 1, 50 - 1, 24, borderColor); // Black outline (bottom-left diagonal)  
	DrawText(TextFormat("Score: %i", score), screenWidth - 150, 50, 24, WHITE); // White text
}



// GAME SHUTDOWN FUNCTION
void GameShutdown() {

	// Unload textures
	for (int i = 0; i < MAX_TEXTURES; i++) {
		UnloadTexture(textures[i]);
	}

	// Unload sounds
	for (int i = 0; i < MAX_SOUNDS; i++) {
		UnloadSound(sounds[i]);
	}

	// Unload music
	for (int i = 0; i < MAX_MUSIC; i++) {
		StopMusicStream(music[i]); // Stop music stream
		UnloadMusicStream(music[i]); // Unload music stream
	}

	// Unload audio device
	CloseAudioDevice();

}

int main()
{
	InitWindow(screenWidth, screenHeight, "Chicken Game");
	SetTargetFPS(60); // Set our game to run at 60 frames-per-second

	GameStartup();

	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		GameUpdate();

		BeginDrawing();
		ClearBackground(GRAY);

		GameRender();

		EndDrawing();
	}

	GameShutdown();

	CloseWindow(); // Close window and OpenGL context
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
