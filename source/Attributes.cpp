#include "Attributes.h"

std::vector<TilesetData::Quad>TilesetData::texture_rects;

TileLayer::TileLayer(const TilesetData& data,
	                 const sf::Vector2u& tile_layer_size,
	                 const std::vector<size_t>& csv_vector)
{
	texture = &data.tileset;
	vertices.setPrimitiveType(sf::Quads);

	size_t first_tile = data.first_tile_id;
	size_t last_tile = first_tile + data.tile_count;

	for (size_t y = 0u, index = 0u; y < tile_layer_size.y; ++y)
	{
		for (size_t x = 0u; x < tile_layer_size.x; ++x, ++index)
		{
			size_t tile_num = csv_vector[index];

			if (tile_num >= first_tile && tile_num < last_tile)
			{
				sf::Vertex leftTop;
				sf::Vertex rightTop;
				sf::Vertex rightBottom;
				sf::Vertex leftBottom;

				leftTop.position     = sf::Vector2f((float)x * data.tile_width, (float)y * data.tile_height);
				rightTop.position    = sf::Vector2f(((float)x + 1) * data.tile_width, (float)y * data.tile_height);
				rightBottom.position = sf::Vector2f(((float)x + 1) * data.tile_width, (float)(y + 1) * data.tile_height);
				leftBottom.position  = sf::Vector2f((float)x * data.tile_width, (float)(y + 1) * data.tile_height);

				leftTop.texCoords     = TilesetData::texture_rects[tile_num - 1].leftTop.texCoords;
				rightTop.texCoords    = TilesetData::texture_rects[tile_num - 1].rightTop.texCoords;
				rightBottom.texCoords = TilesetData::texture_rects[tile_num - 1].rightBottom.texCoords;
				leftBottom.texCoords  = TilesetData::texture_rects[tile_num - 1].leftBottom.texCoords;

				vertices.append(leftTop);
				vertices.append(rightTop);
				vertices.append(rightBottom);
				vertices.append(leftBottom);
			}
		}
	}
}

void TileLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(vertices, texture);
}

