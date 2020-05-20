#pragma once

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"
#include <string>
#include <vector>

class TilesetData
{
public:
	TilesetData()
	{			
		first_tile_id = tile_width = tile_height = tile_count = columns = 0;
	}

	~TilesetData()
	{
	}

	struct Quad
	{
		sf::Vertex leftTop;
		sf::Vertex rightTop;
		sf::Vertex rightBottom;
		sf::Vertex leftBottom;
	};

	sf::Texture tileset;
	size_t      first_tile_id, tile_width, tile_height, tile_count, columns;
	static std::vector<Quad> texture_rects;
};

class TileLayer: public sf::Drawable
{
public:
	TileLayer(const TilesetData& data,
		      const sf::Vector2u& tile_layer_size,
		      const std::vector<size_t>& csv_vector);

	~TileLayer()
	{
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const sf::Texture* texture;
	sf::VertexArray    vertices;
};