#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>


struct Object
{
	sf::Color color;

	float res = 1000;
	float radius;

	std::vector<float> position;

	std::vector<float> velocity;
	float mass;



	Object(std::vector<float> position, std::vector<float> velocity, float mass, float radius, sf::Color color)
	{
		this->position = position;
		this->velocity = velocity;
		this->mass = mass;
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
		// Вектор нормали
		float nx = dx / distance;
		float ny = dy / distance;

		// Относительная скорость
		float vx_rel = obj2.velocity[0] - obj1.velocity[0];
		float vy_rel = obj2.velocity[1] - obj1.velocity[1];

		// Проекция относительной скорости на нормаль
		float dot_product = vx_rel * nx + vy_rel * ny;

		// Расчёт новых скоростей
		float impulse = 2 * dot_product / (obj1.mass + obj2.mass);

		obj1.velocity[0] += impulse * obj2.mass * nx;
		obj1.velocity[1] += impulse * obj2.mass * ny;

		obj2.velocity[0] -= impulse * obj1.mass * nx;
		obj2.velocity[1] -= impulse * obj1.mass * ny;
	}
}

float screenWidth = 1920.0f;
float screenHeight = 1080.0f;


int main()
{
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(screenWidth, screenHeight)), "Simulation OpenGL3");

	// Тела
	Object circle1(std::vector<float>{ 100.0f, 200.0f }, std::vector<float>{ 1.0f, 0.0f }, 25.0f, 100.0f, sf::Color::Red);
	Object circle2(std::vector<float>{ 700.0f, 200.0f }, std::vector<float>{ -1.0f, 0.0f }, 50.0f, 50.0f, sf::Color::Green);


	while (window.isOpen())
	{
		window.clear(sf::Color::White);


		circle1.updatePos();
		circle2.updatePos();


		handleCollisions(circle1, circle2);


		circle1.draw(window);
		circle2.draw(window);


		// Колизии стен
		// X
		{
			if (circle1.position[0] - circle1.radius < 0 || circle1.position[0] + circle1.radius > screenWidth)
				circle1.velocity[0] *= -1;

			if (circle1.position[1] - circle1.radius < 0 || circle1.position[1] + circle1.radius > screenHeight)
				circle1.velocity[1] *= -1;
		}
		// Y
		{
			if (circle2.position[0] - circle2.radius < 0 || circle2.position[0] + circle2.radius > screenWidth)
				circle2.velocity[0] *= -1;

			if (circle2.position[1] - circle2.radius < 0 || circle2.position[1] + circle2.radius > screenHeight)
				circle2.velocity[1] *= -1;
		}


		window.display();

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}
	}
}

