#pragma once

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"
#include "Objects.h"
#include "Attributes.h"

class TileMap: public sf::Drawable
{
public:
	TileMap();
	~TileMap();

	bool load(const std::string& tmx_file_path);
	std::vector<std::vector<size_t>>* getCollisionLayer();
	std::vector<Object>               getObjectsByName(const std::string& name);
	std::vector<Object>               getObjectsByType(const std::string& type);
	std::vector<Object>*              getAllObjects();           

private:
	bool loadTilesets(tinyxml2::XMLElement* map);
	bool loadLayers(tinyxml2::XMLElement* map);
	void loadObjects(tinyxml2::XMLElement* map);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	std::vector<TilesetData>         tilesets;
	std::vector<TileLayer>           tile_layers;
	std::vector<std::vector<size_t>> collision_layer;	
	std::vector<Object>              objects;
};



