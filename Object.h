#pragma once
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
