#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct Property
{
	Property() {}
	Property(const std::string& n, const std::string& t, const std::string& v) :
		name(n), type(t), value(v)
	{
	}

	std::string name, type, value;
};

class Object : public sf::FloatRect
{
public:
	Object() : sf::FloatRect() {}
	Object(float x, float y, float w, float h) : sf::FloatRect(x, y, w, h) {}
	~Object() {}

	std::string           name, type;
	std::vector<Property> properties;
};