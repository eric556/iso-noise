#include <SFML/Graphics.hpp>
#include <FastNoise.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>

const int WIDTH = 200;
const int HEIGHT = 200;
const int MARGIN = 30;
const sf::Color AIR_COLOR =  sf::Color(120, 255, 237);
const sf::Color SAND_COLOR = sf::Color(242, 232, 121);
const sf::Color WATER_COLOR = sf::Color(19, 78, 194);
const sf::Color DIRT_COLOR = sf::Color(120, 74, 18);

struct Air {
	byte type : 4;
};

struct Sand {
	byte type : 4;
};

struct Water {
	byte type : 4;
	byte denisty : 8;
};

struct Particle {
	enum ParticleType {
		AIR,
		SAND,
		WATER,
		DIRT
	} type;

	union
	{
		Air airValue;
		Sand sandValue;
		Water waterValue;
	} value;
};

typedef void(*UpdateFunc)(Particle**, int, int);
typedef std::unordered_map<Particle::ParticleType, UpdateFunc> UpdateFunctionMap;

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
	ImGui::Begin("DOCKING BWAAAAA", (bool*)0, window_flags);
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

void clearImage(sf::Image& image, sf::Color clearColor) {
	for (int y = 0; y < image.getSize().y; y++) {
		for (int x = 0; x < image.getSize().x; x++) {
			image.setPixel(x, y, clearColor);
		}
	}
}

sf::Vector2f randomPointInCircle(double radius) {
	double a = rand() * 2 * 3.14159;
	double r = radius * sqrt(rand());
	double x = r * cos(a);
	double y = r * sin(a);

	return sf::Vector2f(x, y);
}

sf::Vector2i windowToMapSpace(const sf::Vector2f& coord, const sf::FloatRect& renderWindow) {
	return sf::Vector2i((int)(coord.x * (WIDTH / renderWindow.width)), (int)(coord.y * (HEIGHT / renderWindow.height)));
}

sf::Color getColor(Particle::ParticleType type) {
	switch (type)
	{
	case Particle::AIR:  return AIR_COLOR;
	case Particle::SAND: return SAND_COLOR;
	case Particle::WATER: return WATER_COLOR;
	case Particle::DIRT: return DIRT_COLOR;
	default: return sf::Color(238, 0, 255);
	}
}

void updateSand(Particle** map, int x, int y) {
	if (map[x][y + 1].type == Particle::AIR) {
		map[x][y + 1] = map[x][y];
		map[x][y] = { {Particle::AIR} };
	}
	else {
		if (x < WIDTH - 1 && x > 0 && map[x + 1][y + 1].type == Particle::AIR && map[x - 1][y + 1].type == Particle::AIR) {
			int temp = std::rand();
			if (temp == 0) {
				map[x - 1][y + 1] = map[x][y];

			}
			else {
				map[x + 1][y + 1] = map[x][y];
			}
			map[x][y] = { {Particle::AIR} };
		}
		else if (x < WIDTH - 1 && map[x + 1][y + 1].type == Particle::AIR) {
			map[x + 1][y + 1] = map[x][y];
			map[x][y] = { {Particle::AIR} };
		}
		else if (x > 0 && map[x - 1][y + 1].type == Particle::AIR) {
			map[x - 1][y + 1] = map[x][y];
			map[x][y] = { {Particle::AIR} };
		}
		else {
			// dont do anything?
		}
	}
}

void updateWater(Particle** map, int x, int y) {
	if (map[x][y + 1].type == Particle::AIR) {
		map[x][y + 1] = map[x][y];
		map[x][y] = { {Particle::AIR} };
	}
	else {
		if (x < WIDTH - 1 && x > 0 && map[x + 1][y + 1].type == Particle::AIR && map[x - 1][y + 1].type == Particle::AIR) {
			int temp = std::rand();
			if (temp == 0) {
				map[x - 1][y + 1] = map[x][y];

			}
			else {
				map[x + 1][y + 1] = map[x][y];
			}
			map[x][y] = { {Particle::AIR} };
		} else if (x < WIDTH - 1 && map[x + 1][y + 1].type == Particle::AIR) {
			map[x + 1][y + 1] = map[x][y];
			map[x][y] = { {Particle::AIR} };
		} else if (x > 0 && map[x - 1][y + 1].type == Particle::AIR) {
			map[x - 1][y + 1] = map[x][y];
			map[x][y] = { {Particle::AIR} };
		} else if (x < WIDTH - 1 && x > 0 && map[x + 1][y].type == Particle::AIR && map[x - 1][y].type == Particle::AIR) {
			int temp = std::rand();
			if (temp == 0) {
				map[x - 1][y] = map[x][y];
			}
			else {
				map[x + 1][y] = map[x][y];
			}
			map[x][y] = { {Particle::AIR} };
		} else if (x < WIDTH - 1 && map[x + 1][y].type == Particle::AIR) {
			map[x + 1][y] = map[x][y];
			map[x][y] = { {Particle::AIR} };
		} else if (x > 0 && map[x - 1][y].type == Particle::AIR) {
			map[x - 1][y] = map[x][y];
			map[x][y] = { {Particle::AIR} };
		} else {
			// dont do anything?
		}
	}
}

void updateDirt(Particle** map, int x, int y) {
	if (map[x][y - 1].type == Particle::DIRT && map[x - 1][y].type == Particle::DIRT) {
		// all good	
	} else if (map[x][y - 1].type == Particle::DIRT && map[x + 1][y].type == Particle::DIRT) {
		// all good
	} else if (map[x][y + 1].type == Particle::DIRT) {
		// all good
	} else {
		// Update like the filthy sand we are
		updateSand(map, x, y);
	}
}


void updateAir(Particle** map, int x, int y) {}

int main()
{
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Iso Noise");
	ImGui::SFML::Init(window);
	ImGui::GetIO().ConfigFlags |= ImGuiWindowFlags_MenuBar | ImGuiConfigFlags_DockingEnable;

	sf::Image image;
	image.create(WIDTH, HEIGHT);

	sf::Texture texture;
	texture.loadFromImage(image);

	int drawCalls = 0; 

	Particle** map = new Particle*[WIDTH];

	for (int i = 0; i < WIDTH; i++) {
		map[i] = new Particle[HEIGHT];
		for (int y = 0; y < HEIGHT; y++) {
			map[i][y] = { { Particle::AIR } };
		}
	}

	UpdateFunctionMap funcMap;
	funcMap.emplace(Particle::SAND, updateSand);
	funcMap.emplace(Particle::AIR, updateAir);
	funcMap.emplace(Particle::DIRT, updateDirt);
	funcMap.emplace(Particle::WATER, updateWater);

	
	Particle::ParticleType currentParticleType = Particle::SAND;

	bool pause = false;

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();
		}

		sf::Time restart = clock.restart();

		ImGui::SFML::Update(window, restart);

		float dt = restart.asSeconds();
		float fps = 1.f / dt;

		setupDockSpace();

		sf::FloatRect renderWindow;

		ImGui::Begin("View");
		{
			texture.loadFromImage(image);
			sf::Texture::bind(&texture);
			int renderTextureID;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &renderTextureID);
			renderWindow.left = ImGui::GetCursorScreenPos().x;
			renderWindow.top = ImGui::GetCursorScreenPos().y + 22;
			renderWindow.width = ImGui::GetWindowSize().x - MARGIN;
			renderWindow.height = ImGui::GetWindowSize().y - MARGIN;
			ImGui::GetWindowDrawList()->AddImage(
				renderTextureID,
				ImVec2(ImGui::GetCursorScreenPos()),
				ImVec2(ImGui::GetCursorScreenPos().x + renderWindow.width, ImGui::GetCursorScreenPos().y + renderWindow.height),
				ImVec2(0, 1),
				ImVec2(1, 0));
		}
		ImGui::End();

		sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition().x - renderWindow.left, sf::Mouse::getPosition().y - renderWindow.top);

		ImGui::Begin("Stats");
		{
			ImGui::Text(("FPS: " + std::to_string(fps)).c_str());
			ImGui::Text(("X/Y: " + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y)).c_str());
		}
		ImGui::End();

		ImGui::Begin("Particle Picker");
		{
			ImGui::PushItemWidth(100);
			if (ImGui::Button("Air")) {
				currentParticleType = Particle::AIR;
			}
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(100);
			if (ImGui::Button("Sand")) {
				currentParticleType = Particle::SAND;
			}
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(100);
			if (ImGui::Button("Water")) {
				currentParticleType = Particle::WATER;
			}
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(100);
			if (ImGui::Button("Dirt")) {
				currentParticleType = Particle::DIRT;
			}
			ImGui::PopItemWidth();
		}
		ImGui::End();

		ImGui::Begin("Controls");
		{
			std::string buttonText = "";
			if (pause) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)sf::Color::Red);
				buttonText = "Run";
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)sf::Color::Green);
				buttonText = "Pause";
			}
			if (ImGui::Button(buttonText.c_str())) {
				pause = !pause;
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();

		if (!pause) {
			sf::Vector2i mouseMapCoord = windowToMapSpace(mousePos, renderWindow);

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				for (int i = 0; i < 20; i++) {
					sf::Vector2f offset = randomPointInCircle(1);
					mouseMapCoord = sf::Vector2i(offset.x, offset.y) + mouseMapCoord;
					if (mouseMapCoord.x > 0 && mouseMapCoord.x < WIDTH && mouseMapCoord.y > 0 && mouseMapCoord.y < HEIGHT) {
						map[mouseMapCoord.x][mouseMapCoord.y] = { {currentParticleType} };
					}
				}

				
			}

			for (int y = HEIGHT - 1; y >= 0; y--) {
				for (int x = 0; x < WIDTH; x++) {
					if (map[x][y].type == 0) continue;
					if (y < HEIGHT - 1) {
						funcMap[map[x][y].type](map, x, y);
					}
				}
			}

			for (int y = 0; y < HEIGHT; y++) {
				for (int x = 0; x < WIDTH; x++) {
					image.setPixel(x, HEIGHT - y - 1, getColor(map[x][y].type));
				}
			}
		}

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}