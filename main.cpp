#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "imgui.h"
#include "imgui-SFML.h"



struct Object
{
	sf::Color color;

	float res = 10000;
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
float screenWidth = 800.0f;
float screenHeight = 600.0f;

sf::Color bgColor = sf::Color::Black;

bool isPaused = false;


// Objects
std::vector<Object> initialObjects =
{
	Object(100.0f, std::vector<float>{ screenWidth / 2.0f - 200.0f, 200.0f }, std::vector<float>{ 2.0f, 0.0f }, 100.0f, sf::Color::Red),
	Object(50.0f, std::vector<float>{ screenWidth / 2.0f + 150.0f, 200.0f }, std::vector<float>{ -2.0f, 0.0f }, 50.0f, sf::Color::Green)
};

Object circle1 = initialObjects[0];
Object circle2 = initialObjects[1];

std::vector<Object*> objects = { &circle1, &circle2 };


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

		if (!isPaused)
		{
			circle1.updatePos();
			circle2.updatePos();

			handleCollisions(circle1, circle2);

		}
		circle1.draw(window);
		circle2.draw(window);


		// Wall Collision
		// 1
		float collisionSlowdown = -1.0f;
		{
			if (circle1.position[0] - circle1.radius < 0 || circle1.position[0] + circle1.radius > screenWidth)
				circle1.velocity[0] *= collisionSlowdown;

			if (circle1.position[1] - circle1.radius < 0 || circle1.position[1] + circle1.radius > screenHeight)
				circle1.velocity[1] *= collisionSlowdown;
		}
		// 2
		{
			if (circle2.position[0] - circle2.radius < 0 || circle2.position[0] + circle2.radius > screenWidth)
				circle2.velocity[0] *= collisionSlowdown;

			if (circle2.position[1] - circle2.radius < 0 || circle2.position[1] + circle2.radius > screenHeight)
				circle2.velocity[1] *= collisionSlowdown;
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
						auto object = objects[i];
						ImGui::PushID(i);

						ImGui::SeparatorText("Object");

						// Color
						float color[3] = { object->color.r / 255.0f, object->color.g / 255.0f, object->color.b / 255.0f };
						ImGui::Text("Color");
						ImGui::ColorEdit3("##color", color);
						object->color.r = static_cast<int>(color[0] * 255.0f);
						object->color.g = static_cast<int>(color[1] * 255.0f);
						object->color.b = static_cast<int>(color[2] * 255.0f);

						ImGui::Separator();

						// Object Parameters
						ImGui::Text("Object Parameters");
						ImGui::DragFloat("Resolution", &object->res, 10.0f, 100.0f, 100000.0f, "%.0f");
						ImGui::DragFloat("Radius", &object->radius, 1.0f, 1.0f, 200.0f);
						ImGui::DragFloat("Mass", &object->mass, 0.1f, 1.0f, 1000.0f);

						ImGui::Separator();

						// Coordinates
						ImGui::Text("Coordinates");
						ImGui::DragFloat2("Position", object->position.data());
						ImGui::DragFloat2("Velocity", object->velocity.data());
						ImGui::Spacing();

						ImGui::PopID();
					}

					// Buttons
					if (ImGui::Button("Reset"))
					{
						for (int i = 0; i < objects.size(); i++)
						{
							objects[i]->mass = initialObjects[i].mass;
							objects[i]->position = initialObjects[i].position;
							objects[i]->velocity = initialObjects[i].velocity;
							objects[i]->radius = initialObjects[i].radius;
							objects[i]->color = initialObjects[i].color;
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Pause"))
					{
						isPaused = !isPaused;
					}

					ImGui::EndTabItem();
				}

				// Background Tab
				if (ImGui::BeginTabItem("Background"))
				{
					ImGui::SeparatorText("BackGround");
					float color[3] = { bgColor.r / 255.0f, bgColor.g / 255.0f, bgColor.b / 255.0f };
					ImGui::ColorEdit3("##color", color);
					bgColor.r = static_cast<int>(color[0] * 255.0f);
					bgColor.g = static_cast<int>(color[1] * 255.0f);
					bgColor.b = static_cast<int>(color[2] * 255.0f);

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

