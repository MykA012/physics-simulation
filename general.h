#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

#include "Object.h"


// settings
float screenWidth = 800.0f;
float screenHeight = 600.0f;


bool isPaused = false;


// bg Color
sf::Color bgColor = sf::Color::Black;


// Objects
std::vector<Object> initialObjects = 
{
	Object(100.0f, std::vector<float>{ screenWidth / 2.0f - 200.0f, 200.0f }, std::vector<float>{ 2.0f, 0.0f }, 100.0f, sf::Color::Red),
	Object(50.0f, std::vector<float>{ screenWidth / 2.0f + 150.0f, 200.0f }, std::vector<float>{ -2.0f, 0.0f }, 50.0f, sf::Color::Green)
};

Object circle1 = initialObjects[0];
Object circle2 = initialObjects[1];

std::vector<Object*> objects = { &circle1, &circle2 };