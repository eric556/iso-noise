#pragma once
#include <raylib.h>
typedef frame** animationMap;

struct position {
	float x;
	float y;
};

struct velocity {
	float dx;
	float dy;
};

struct rectView {
	float width;
	float height;
	Color color;
};

struct acceleration {
	float dvx;
	float dvy;
};

struct forceAccumulator {
	float mass;
	float fx;
	float fy;
};

struct playerController {
	int space;
	int crouch;
	float playerSpeed;
	bool crouching;
	bool grounded;
};

struct animationController {
	int currentAnimationType;
	const int* numFramesFor;
	const frame** animations;
	int currentFrame;
	float currentAnimationTime;
};

enum BirdLane {
	Top = 0,
	Middle = 1,
	Bottom = 2
};

struct groundAnimationController {

};

struct boundingBox {
	float minx;
	float maxx;
	float miny;
	float maxy;
};

struct spriteView {
	Rectangle rect;
};