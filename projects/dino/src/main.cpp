#include <iostream>
#include <entt.hpp>
#include <raylib.h>
#include <map>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>

#include "spriteMaps.h"
#include "components.h"

const int WIDTH = 800;
const int HEIGHT = 450;



bool intersect(const boundingBox& a, const boundingBox& b) {
	return (a.minx <= b.maxx && a.maxx >= b.minx) && (a.miny <= b.maxy && a.maxy >= b.miny);
}

void updatePosition(float dt, entt::registry &registry) {
	auto view = registry.view<velocity, position>();

	for (auto entity : view) {
		auto &vel = view.get<velocity>(entity);
		auto &pos = view.get<position>(entity);

		pos.x += vel.dx * dt;
		pos.y += vel.dy * dt;
	}
}

void updateGravity(float dt, float gravity, entt::registry &registry) {
	auto view = registry.view<velocity>();

	for (auto entity : view) {
		auto &vel = view.get<velocity>(entity);
		vel.dy += gravity * dt;
	}
}

void drawRectView(entt::registry &registry) {
	auto view = registry.view<rectView, position>();

	for (auto entity : view) {
		auto &rView = view.get<rectView>(entity);
		auto &pos = view.get<position>(entity);

		DrawRectangle(pos.x, pos.y, rView.width, rView.height, rView.color);
	}
}

void updateInput(float dt, entt::registry &registry) {
	auto view = registry.view<velocity, playerController>();
	for (auto entity : view) {
		auto &input = view.get<playerController>(entity);
		auto &vel = view.get<velocity>(entity);

		if (IsKeyDown(input.space)) {
			// jump
			if (input.grounded) {
				vel.dy -= input.playerSpeed * dt;
				input.grounded = false;
			}
		}

		input.crouching = IsKeyDown(input.crouch);
	}
}

void updatePlayerState(float dt, entt::registry& registry) {
	auto view = registry.view<animationController, playerController>();
	for (auto entity : view) {
		auto &ac = view.get<animationController>(entity);
		auto &pc = view.get<playerController>(entity);
		if (pc.grounded) {
			ac.currentAnimationType = (pc.crouching) ? PlayerAnimationType::Ducking : PlayerAnimationType::Running;
		} else {
			ac.currentAnimationType = PlayerAnimationType::Jumping;
		}
	}
}

void updateAnimation(float dt, entt::registry &registry) {
	// TODO maybe keep a previous state to see if its been changed. If so we need to reset the duration to keep it from overruning into the next frame

	auto view = registry.view<animationController>();
	for (auto entity : view) {
		auto &ac = view.get<animationController>(entity);
		ac.currentAnimationTime -= dt;
		if (ac.currentAnimationTime <= 0) {
			std::cout << "Swapping frame: " << ac.currentAnimationType << "\t" << ac.currentFrame << std::endl;
			ac.currentFrame = (ac.currentFrame + 1) % ac.numFramesFor[ac.currentAnimationType];
			ac.currentAnimationTime = ac.animations[ac.currentAnimationType][ac.currentFrame].duration;
		}
	}
}

void drawSprite(Texture2D& texture, entt::registry &registry) {
	auto view = registry.view<spriteView, position>();
	for (auto entity : view) {
		auto &sprite = view.get<spriteView>(entity);
		auto &pos = view.get<position>(entity);

		DrawTextureRec(texture, sprite.rect, { pos.x, pos.y }, WHITE);
	}
}

void drawAnimatedSprite(Texture2D& texture, entt::registry &registry) {
	auto view = registry.view<position, animationController>();

	for (auto entity : view) {
		auto &pos = view.get<position>(entity);
		auto &ac = view.get<animationController>(entity);

		DrawTextureRec(texture, ac.animations[ac.currentAnimationType][ac.currentFrame].rect, { pos.x, pos.y }, WHITE);
	}

}

entt::entity createPlayer(entt::registry& registry, position pos) {
	auto entity = registry.create();
	registry.emplace<position>(entity, pos);
	registry.emplace<velocity>(entity, 0.f, 0.f);
	registry.emplace<rectView>(entity, 10.f, 10.f, RED);
	registry.emplace<playerController>(entity, KEY_SPACE, KEY_S, 800.f, false, true);
	registry.emplace<animationController>(entity, PlayerAnimationType::Running, playerAnimationFrameNumbers, playerAnimations, 0, 0.f);
	return entity;
}

entt::entity createCactus(entt::registry& registry, position pos, CactusType type) {
	auto entity = registry.create();
	registry.emplace<position>(entity, pos);
	registry.emplace<velocity>(entity, 0.0f, 0.0f);
	registry.emplace<rectView>(entity, 10.f, 50.f, GREEN);
	registry.emplace<spriteView>(entity, cactusMaps[type][rand() % (cactusMapsSizes[type] - 1)]);

	return entity;
}

entt::entity createBird(entt::registry& registry, position pos) {
	auto entity = registry.create();
	registry.emplace<position>(entity, pos);
	registry.emplace<velocity>(entity, -100.f, 0.f);
	registry.emplace<animationController>(entity, BirdAnimationType::Flapping, birdAnimationFrameNumbers, birdAnimations, 0, 0.f);
	registry.emplace<BirdLane>(entity, BirdLane::Top);

	return entity;
}

int main() {
	srand(time(NULL));
	InitWindow(WIDTH, HEIGHT, "Dino Run");
	SetTargetFPS(60);
	entt::registry registry;

	Texture2D spriteSheet = LoadTexture("assets/sprite-sheet.png");

	auto player = createPlayer(registry, {100.f, 100.f});
	auto bird = createBird(registry, { 200.f, 100.f });

	for (int i = 0; i < 100; i++) {
		if (i % 2 == 0) {
			auto temp = createCactus(registry, { i * 50.f, 200.f }, CactusType::Big);
		} else {
			auto temp = createCactus(registry, { i * 50.f, 200.f }, CactusType::Normal);
		}
	}

	while (!WindowShouldClose()) {
		float deltaTime = GetFrameTime();

		updatePosition(deltaTime, registry);
		updateInput(deltaTime, registry);
		updatePlayerState(deltaTime, registry);
		// updateGravity(deltaTime, 400, registry);
		updateAnimation(deltaTime, registry);

		BeginDrawing();
		{
			ClearBackground(RAYWHITE);
			drawSprite(spriteSheet, registry);
			drawAnimatedSprite(spriteSheet, registry);
		}
		EndDrawing();
	}

	UnloadTexture(spriteSheet);

	CloseWindow();

	return 0;
}