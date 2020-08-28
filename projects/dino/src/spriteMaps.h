#pragma once
#include <raylib.h>

const int numCactusSprites = 6;
const Rectangle cactusSpriteMap[] = {
	{ 446, 2, 34, 70 },
	{ 480, 2, 34, 70 },
	{ 514, 2, 34, 70 },
	{ 548, 2, 34, 70 },
	{ 582, 2, 34, 70 },
	{ 616, 2, 34, 70 }
};

const int numBigCactusSprites = 4;
const Rectangle bigCactusSpriteMap[] = {
	{ 652, 2, 50, 100 },
	{ 702, 2, 50, 100 },
	{ 752, 2, 50, 100 },
	{ 802, 2, 50, 100 }
};

const int numXlCactusSprites = 1;
const Rectangle xlCactusSpriteMap[] = {
	{ 850, 2, 102, 100 },
};

enum CactusType {
	Normal = 0,
	Big = 1,
	ExtraLarge = 2
};

const int cactusMapsSizes[] = {
	numCactusSprites,
	numBigCactusSprites,
	numXlCactusSprites
};

const Rectangle* cactusMaps[] = {
	cactusSpriteMap,
	bigCactusSpriteMap,
	xlCactusSpriteMap
};

struct frame {
	Rectangle rect;
	float duration;
};

// 1678, 2 - 88 x 94
const int numPlayerRunFrames = 2;
const frame playerRunFrames[] = {
	{{1854, 2, 88, 94}, 0.10f},
	{{1942, 2, 88, 94}, 0.10f},
};

const int numPlayerDuckFrames = 2;
const frame playerDuckFrames[] = {
	{{2203, 2, 118, 95}, 0.1f},
	{{2321, 2, 118, 95}, 0.1f},
};

const int numPlayerJumpFrames = 1;
const frame playerJumpFrames[] = {
	{{1678, 2, 88, 94}, 0.10f},
};

enum PlayerAnimationType {
	Running = 0,
	Ducking = 1,
	Jumping = 2
};

const int playerAnimationFrameNumbers[] = {
	numPlayerRunFrames,
	numPlayerDuckFrames,
	numPlayerJumpFrames
};

const frame* playerAnimations[] = {
	playerRunFrames,
	playerDuckFrames,
	playerJumpFrames
};

enum BirdAnimationType {
	Flapping
};

const int numBirdFlapFrames = 2;
const frame birdFlapFrames[] = {
	{{260, 0, 93, 82}, 0.1f},
	{{353, 0, 93, 82}, 0.1f},
};

const int birdAnimationFrameNumbers[] = {
	numBirdFlapFrames
};

const frame* birdAnimations[] = {
	birdFlapFrames
};