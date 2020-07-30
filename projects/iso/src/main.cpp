#include <SFML/Graphics.hpp>
#include <FastNoise.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>

const int WIDTH = 1920;
const int HEIGHT = 1080;

int mapWidth = 40;
int mapHeight = 40;

enum Ops {
	Add,
	Subtract,
	Multiply,
	Divide,
	Average
};


struct NoiseSettings {
	Ops op;
	float xOffset;
	float yOffset;
	FastNoise noiseGen;
	float amplitude;
};

float getNoise(const NoiseSettings& settings, float prevHeight, float x, float y) {
	float height = std::abs(settings.noiseGen.GetNoise(x + settings.xOffset, y + settings.yOffset)) * settings.amplitude;
	switch (settings.op) {
		case Add: return prevHeight + height;
		case Subtract: return prevHeight - height;
		case Multiply: return prevHeight * height;
		case Divide: return prevHeight / height;
		case Average: return (prevHeight + height) / 2.f;
	}
}


struct ColorBand {
	int r;
	int g;
	int b;
	int min;
	int max;
};

void setupDockSpace() {
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", (bool*)0, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}

sf::Vector2f toIso(const sf::Vector2f& point, sf::Vector2f ) {
	return sf::Vector2f(point.x + point.y, point.y - point.x / 2.0);
}

sf::Vector2f isoToScreen(const sf::Vector3f& coord, const sf::Vector2f& worldOrigin) {
	float y = coord.y * std::sqrt(1.25);
	float z = 0.5 * (coord.x + coord.z) - coord.y;
	float x = coord.x - coord.z;
	return sf::Vector2f(x + worldOrigin.x, z + worldOrigin.y);
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Noise");
	ImGui::SFML::Init(window);
	ImGui::GetIO().ConfigFlags |= ImGuiWindowFlags_MenuBar | ImGuiConfigFlags_DockingEnable;

	FastNoise noiseGen(FastNoise::Perlin);

	sf::Texture block;
	if (!block.loadFromFile("assets/blocks_27.png")) {
		std::cout << "Error loading block texture" << std::endl;
	}

	sf::Sprite blockSprite;
	blockSprite.setTexture(block);
	blockSprite.scale(0.5, 0.5);

	sf::RenderTexture renderTexture;
	renderTexture.create(WIDTH, HEIGHT);

	const char* noiseTypes[] = {
		"Value", "ValueFractal", "Perlin", "PerlinFractal", "Simplex", "SimplexFractal", "Cellular", "WhiteNoise", "Cubic", "CubicFractal"
	};

	const char* opTypes[] = {
		"Add",
		"Subtract",
		"Multiply",
		"Divide",
		"Average"
	};

	float offset_speed[] = { 0, 0 };
	float offset[] = { 0, 0 };
	bool scrolling[] = { false, false };
	float origin[] = { 64, 16 };
	float amplitude = 1;
	int currentNoiseType = 0;
	float** map = new float*[mapWidth];
	std::vector<ColorBand> colorBands;
	std::vector<int> colorBandsToErase;
	std::vector<NoiseSettings> noiseSettings;
	std::vector<int> noiseSettingsToErase;


	for (int i = 0; i < mapWidth; i++) {
		map[i] = new float[mapHeight];
	}

	int drawCalls = 0;


	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Time restart = clock.restart();

		ImGui::SFML::Update(window, restart);

		float dt = restart.asSeconds();
		float fps = 1.f / dt;

		setupDockSpace();

		ImGui::Begin("Controls");
		{
			ImGui::Checkbox("Scroll X", &scrolling[0]); ImGui::SameLine();
			ImGui::Checkbox("Scroll Y", &scrolling[1]);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::DragFloat2("Scroll Speed", offset_speed, 1.0f, -200.0f, 200.0f);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::InputFloat2("Scroll", offset);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::DragFloat("Amplitude", &amplitude, 1.0f, 1.0f, 500.0f);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::DragFloat2("Origin", origin);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Combo("Noise Type", &currentNoiseType, noiseTypes, IM_ARRAYSIZE(noiseTypes));

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::InputInt("Map Width", &mapWidth);

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::InputInt("Map Height", &mapHeight);

		}
		ImGui::End();

		ImGui::Begin("Info");
		{
			ImGui::Text(("Seed: " + std::to_string(noiseGen.GetSeed())).c_str());
			ImGui::Text(("FPS: " + std::to_string(fps)).c_str());
			ImGui::Text(("FPS: " + std::to_string(drawCalls)).c_str());
		}
		ImGui::End();

		ImGui::Begin("Noise");
		{
			noiseSettingsToErase.clear();
			for (int i = 0; i < noiseSettings.size(); i++) {
				int tempNoiseType = (int)noiseSettings[i].noiseGen.GetNoiseType();
				int tempOpType = (int)noiseSettings[i].op;
				ImGui::Combo(("Noise Type " + std::to_string(i)).c_str(), &tempNoiseType, noiseTypes, IM_ARRAYSIZE(noiseTypes));
				ImGui::Combo(("Op Type " + std::to_string(i)).c_str(), &tempOpType, opTypes, IM_ARRAYSIZE(opTypes));
				ImGui::InputFloat2(("X/Y Offset " + std::to_string(i)).c_str(), &noiseSettings[i].xOffset);
				ImGui::DragFloat(("Amplitude " + std::to_string(i)).c_str(), &noiseSettings[i].amplitude);
				if (ImGui::Button(("Remove " + std::to_string(i)).c_str())) {
					noiseSettingsToErase.push_back(i);
				}
				noiseSettings[i].noiseGen.SetNoiseType((FastNoise::NoiseType)tempNoiseType);
				noiseSettings[i].op = (Ops)tempOpType;
				ImGui::Separator();
			}

			for (int i = 0; i < noiseSettingsToErase.size(); i++) {
				noiseSettings.erase(noiseSettings.begin() + noiseSettingsToErase[i]);
			}

			if (ImGui::Button("Add Noise")) {
				noiseSettings.push_back(NoiseSettings{ Ops::Add, 0, 0, FastNoise(FastNoise::Value), 0});
			}
		}
		ImGui::End();

		ImGui::Begin("Color");
		{
			colorBandsToErase.clear();
			for (int i = 0; i < colorBands.size(); i++) {
				ImGui::InputInt3(("RGB " + std::to_string(i)).c_str(), &colorBands[i].r);
				ImGui::InputInt2(("Min/Max " + std::to_string(i)).c_str(), &colorBands[i].min);
				if (ImGui::Button(("Remove " + std::to_string(i)).c_str())) {
					colorBandsToErase.push_back(i);
				}
				ImGui::Separator();
			}
			for (int i = 0; i < colorBandsToErase.size(); i++) {
				colorBands.erase(colorBands.begin() + colorBandsToErase[i]);
			}

			if (ImGui::Button("Add Color")) {
				colorBands.push_back(ColorBand{ 0,0,0,0,0 });
			}
		}
		ImGui::End();

		ImGui::Begin("View");
		{
			sf::Texture::bind(&renderTexture.getTexture());
			int renderTextureID;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &renderTextureID);
			ImGui::GetWindowDrawList()->AddImage(
				renderTextureID, 
				ImVec2(ImGui::GetCursorScreenPos()),
				ImVec2(ImGui::GetCursorScreenPos().x + window.getSize().x * 0.75, ImGui::GetCursorScreenPos().y + window.getSize().y * 0.75), 
				ImVec2(0, 1), 
				ImVec2(1, 0));
		}
		ImGui::End();

		renderTexture.clear(sf::Color(120, 255, 237));
		noiseGen.SetNoiseType((FastNoise::NoiseType)currentNoiseType);
		drawCalls = 0;

		for (int y = 0; y < mapHeight; y++) {
			for (int x = 0; x < mapWidth; x++) {
				float height = std::abs(noiseGen.GetNoise(x + offset[0], y + offset[1])) * amplitude;
				for (int i = 0; i < noiseSettings.size(); i++) {
					height = getNoise(noiseSettings[i], height, x + offset[0], y + offset[1]);
				}
				int zStart = 0;
				if (x > 0 && x < mapWidth - 1) {
					if (y > 0 && y < mapHeight - 1) {
						float yFrontHeight = std::abs(noiseGen.GetNoise(x + offset[0], y + 1 + offset[1])) * amplitude;
						float xFrontHeight = std::abs(noiseGen.GetNoise(x + 1 + offset[0], y + offset[1])) * amplitude;
						for (int i = 0; i < noiseSettings.size(); i++) {
							yFrontHeight = getNoise(noiseSettings[i], yFrontHeight, x + offset[0], y + 1 + offset[1]);
							xFrontHeight = getNoise(noiseSettings[i], xFrontHeight, x + 1 + offset[0], y + offset[1]);
						}
						zStart = std::min(yFrontHeight, xFrontHeight);
					}
				}
				for (; zStart < height; zStart++) {
					sf::Vector2f screenCoord = isoToScreen(sf::Vector3f(x, zStart, y), sf::Vector2f(origin[0], origin[1]));
					float screenX = screenCoord.x * (blockSprite.getGlobalBounds().width / 2);
					float screenY = screenCoord.y * (blockSprite.getGlobalBounds().height / 2);
					blockSprite.setPosition(screenX, screenY);

					blockSprite.setColor(sf::Color::White);
					for (int i = 0; i < colorBands.size(); i++) {
						if (zStart > colorBands[i].min && zStart < colorBands[i].max) {
							blockSprite.setColor(sf::Color(colorBands[i].r, colorBands[i].g, colorBands[i].b));
						}
					}

					renderTexture.draw(blockSprite);
					drawCalls++;
				}
			}
		}

		for (int i = 0; i < 2; i++) {
			if (scrolling[i]) {
				offset[i] += offset_speed[i] * dt;
			}
		}

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}