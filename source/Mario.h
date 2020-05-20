#pragma once

#include "TileMap.h"

enum Tile: size_t // static tiles for collision detection
{
	NONE = 0,
	GROUND = 1,
	STONE = 4,
	INVISIBLE_WALL = 25u // ...etc
};

class Mario : public sf::Sprite
{
public:
	Mario(sf::Texture& texture, TileMap& tm)
	{
		setTexture(texture);
		setTextureRect(sf::IntRect(0, 96, tile_size, tile_size)); // small Mario texture rect, for example

		map = tm.getCollisionLayer();

		on_ground = false;
		x = 200.0f; // start position 
		y = 400.0f;
	}

	~Mario()
	{
	}

	void update()
	{
		x += dx;
		handleCollisionHorizontally();

		if (!on_ground)		
			dy += 0.30f; // imitation of gravity

		y += dy;
		on_ground = false;
		handleCollisionVertically();
		
		setPosition(x, y); 
	}

	void handleCollisionHorizontally()
	{
		// find Mario`s bounds in the tilemap and check its 4 corners
		for (size_t i = y / tile_size; i < (y + tile_size) / tile_size; i++)
			for (size_t j = x / tile_size; j < (x + tile_size) / tile_size; j++)
			{
				if ((*map)[i][j] == Tile::INVISIBLE_WALL || 
					(*map)[i][j] == Tile::GROUND         ||
					(*map)[i][j] == Tile::STONE)
				{
					if (dx > 0) // move right
						x = j * tile_size - tile_size;

					if (dx < 0) // move left
						x = j * tile_size + tile_size;
				}
			}
	}

	void handleCollisionVertically()
	{
		// find Mario`s bounds in the tilemap and define its 4 corners
		for (size_t i = y / tile_size; i < (y + tile_size) / tile_size; i++)
			for (size_t j = x / tile_size; j < (x + tile_size) / tile_size; j++)
			{
				if ((*map)[i][j] == Tile::GROUND ||
					(*map)[i][j] == Tile::STONE)
				{
					if (dy > 0) // the fall
					{
						y = i * tile_size - tile_size;
						dy = 0;
						on_ground = true;
					}
				}
			}
	}

	bool on_ground;
	float x, y, dx, dy;
	const size_t tile_size = 32u;
	std::vector<std::vector<size_t>>* map;
};