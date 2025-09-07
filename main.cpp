#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"



struct Object
{
	sf::Color color;

	float res = 1000;
	float radius;

	std::vector<float> position;
	std::vector<float> velocity;
	float mass;


	Object(float mass, std::vector<float> position, std::vector<float> velocity, float radius, sf::Color color)
	{
		this->mass = mass;
		this->position = position;
		this->velocity = velocity;
		this->radius = radius;
		this->color = color;
	}

	bool operator==(const Object& obj2) const
	{
		return this == &obj2;
	}

	bool operator!=(const Object& obj2) const
	{
		return this != &obj2;
	}

	void accelerate(float x, float y)
	{
		velocity[0] += x;
		velocity[1] += y;
	}

	void updatePos()
	{
		position[0] += velocity[0];
		position[1] += velocity[1];
	}

	void draw(sf::RenderWindow& window)
	{
		sf::CircleShape shape(radius, res);
		shape.setFillColor(color);
		shape.setPosition({ position[0], position[1] });
		shape.setOrigin({ radius, radius });

		window.draw(shape);
	}
};

void handleCollisions(Object& obj1, Object& obj2)
{
	// Расстояние двух точек на плоскости
	float dx = obj2.position[0] - obj1.position[0];
	float dy = obj2.position[1] - obj1.position[1];
	float distance = std::sqrt(dx * dx + dy * dy);

	if (distance < obj1.radius + obj2.radius)
	{
		float nx = dx / distance;
		float ny = dy / distance;

		float vx_rel = obj2.velocity[0] - obj1.velocity[0];
		float vy_rel = obj2.velocity[1] - obj1.velocity[1];

		float dot_product = vx_rel * nx + vy_rel * ny;

		float impulse = 2 * dot_product / (obj1.mass + obj2.mass);

		obj1.velocity[0] += impulse * obj2.mass * nx;
		obj1.velocity[1] += impulse * obj2.mass * ny;

		obj2.velocity[0] -= impulse * obj1.mass * nx;
		obj2.velocity[1] -= impulse * obj1.mass * ny;
	}
}


// settings
float screenWidth = 1920.0f;
float screenHeight = 1080.0f;

sf::Color bgColor = sf::Color::Black;
bool isPaused = false;

float collisionSlowdown = -1.0f;


// objects
std::vector<Object> objects{};

// Factory
namespace newObject
{
	float mass = 100.0f;
	std::vector<float> position{ screenWidth / 2.0f, screenHeight / 2.0f };
	std::vector<float> velocity{ 0.0f, 0.0f };
	float radius = 100.0f;
	sf::Color color = sf::Color::Red;
}

void createNewObject(Object& newObject)
{
	objects.push_back(newObject);
}



// --- MAIN ---
int main()
{
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(screenWidth, screenHeight)), "Simulation");
	if (!ImGui::SFML::Init(window)) return -1;


	sf::Clock deltaClock;
	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}
		ImGui::SFML::Update(window, deltaClock.restart());


		window.clear(bgColor);


		for (auto& object : objects)
		{
			if (!isPaused)
			{
				object.updatePos();

				for (auto& compareObject : objects)
				{
					if (object != compareObject)
					{
						handleCollisions(object, compareObject);
					}
				}
			}
			// Wall Collision
			{
				if (object.position[0] - object.radius < 0 || object.position[0] + object.radius > screenWidth)
					object.velocity[0] *= collisionSlowdown;

				if (object.position[1] - object.radius < 0 || object.position[1] + object.radius > screenHeight)
					object.velocity[1] *= collisionSlowdown;
			}

			object.draw(window);
		}

		// ImGui Window
		if (ImGui::Begin("Menu"))
		{
			if (ImGui::BeginTabBar("##"))
			{
				// Objects Tab
				if (ImGui::BeginTabItem("Objects"))
				{
					// Objects
					for (int i = 0; i < objects.size(); i++)
					{
						auto& object = objects[i];
						ImGui::PushID(i);

						ImGui::SeparatorText("Object");

						// Color
						float color[3] = { object.color.r / 255.0f, object.color.g / 255.0f, object.color.b / 255.0f };
						ImGui::Text("Color");
						ImGui::ColorEdit3("##color", color);
						object.color.r = static_cast<int>(color[0] * 255.0f);
						object.color.g = static_cast<int>(color[1] * 255.0f);
						object.color.b = static_cast<int>(color[2] * 255.0f);

						ImGui::Separator();

						// Object Parameters
						ImGui::Text("Object Parameters");
						ImGui::DragFloat("Resolution", &object.res, 10.0f, 100.0f, 100000.0f, "%.0f");
						ImGui::DragFloat("Radius", &object.radius, 1.0f, 1.0f, 200.0f);
						ImGui::DragFloat("Mass", &object.mass, 0.1f, 1.0f, 1000.0f);

						ImGui::Separator();

						// Coordinates
						ImGui::Text("Coordinates");
						ImGui::DragFloat2("Position", object.position.data());
						ImGui::DragFloat2("Velocity", object.velocity.data());

						// Buttons
						if (ImGui::Button("Delete"))
						{
							objects.erase(std::find(objects.begin(), objects.end(), object));
						}

						ImGui::PopID();
					}

					// Buttons
					if (ImGui::Button("Pause"))
					{
						isPaused = !isPaused;
					}

					ImGui::EndTabItem();
				}

				// Global Tab
				if (ImGui::BeginTabItem("Global"))
				{
					// Background
					ImGui::SeparatorText("BackGround");
					float color[3] = { bgColor.r / 255.0f, bgColor.g / 255.0f, bgColor.b / 255.0f };
					ImGui::ColorEdit3("##color", color);
					bgColor.r = static_cast<int>(color[0] * 255.0f);
					bgColor.g = static_cast<int>(color[1] * 255.0f);
					bgColor.b = static_cast<int>(color[2] * 255.0f);

					// Objects
					ImGui::SeparatorText("Objects");
					if (ImGui::Button("Remove all"))
					{
						objects.clear();
					}

					ImGui::EndTabItem();
				}

				// new Object
				if (ImGui::BeginTabItem("create object"))
				{
					using namespace newObject;

					// Color
					float colorArray[3] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f };
					ImGui::Text("Color");
					ImGui::ColorEdit3("##color", colorArray);
					color.r = static_cast<int>(colorArray[0] * 255.0f);
					color.g = static_cast<int>(colorArray[1] * 255.0f);
					color.b = static_cast<int>(colorArray[2] * 255.0f);

					// Object Parameters
					ImGui::DragFloat("Radius", &radius, 1.0f, 1.0f, 200.0f);
					ImGui::DragFloat("Mass", &mass, 0.1f, 1.0f, 1000.0f);

					ImGui::Separator();

					// Coordinates
					ImGui::Text("Coordinates");
					ImGui::DragFloat2("Position", position.data());
					ImGui::DragFloat2("Velocity", velocity.data());
					ImGui::Spacing();

					if (ImGui::Button("Create new object"))
					{
						Object obj(mass, position, velocity, radius, color);

						createNewObject(obj);
					}
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}


		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}

