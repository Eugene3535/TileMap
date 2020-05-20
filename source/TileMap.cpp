#include "TileMap.h"
#include <iostream>
#include <cctype>
#include <algorithm>

TileMap::TileMap()
{	
}

TileMap::~TileMap()
{
}

bool TileMap::load(const std::string& tmx_file_path)
{
	tinyxml2::XMLDocument document;

	if (document.LoadFile(tmx_file_path.c_str()) != tinyxml2::XML_SUCCESS)
	{
		std::cerr << "Loading file " << tmx_file_path << " failed..." << std::endl;
		return false;
	}
	
	tinyxml2::XMLElement* map = document.FirstChildElement("map");
	
	bool success = loadTilesets(map) && loadLayers(map);
	loadObjects(map);

	return success;	
}

std::vector<std::vector<size_t>>* TileMap::getCollisionLayer()  
{
	return &collision_layer;
}

std::vector<Object> TileMap::getObjectsByName(const std::string& name)
{
	std::vector<Object> objects_by_name;

	for (auto& object : objects)
		if (object.type == name)
			objects_by_name.push_back(object);

	return objects_by_name;
}

std::vector<Object> TileMap::getObjectsByType(const std::string& type)
{
	std::vector<Object> objects_by_type;

	for (auto& object : objects)	
		if (object.type == type)	
			objects_by_type.push_back(object);
		
	return objects_by_type;
}

std::vector<Object>* TileMap::getAllObjects()
{
	return &objects;
}

bool TileMap::loadTilesets(tinyxml2::XMLElement* map)
{
	for (auto tileset = map->FirstChildElement("tileset");
		      tileset != nullptr;
		      tileset = tileset->NextSiblingElement("tileset"))
	{
		TilesetData data;
		data.first_tile_id = std::stoul(tileset->Attribute("firstgid"));
		data.tile_width    = std::stoul(tileset->Attribute("tilewidth"));
		data.tile_height   = std::stoul(tileset->Attribute("tileheight"));
		data.tile_count    = std::stoul(tileset->Attribute("tilecount"));
		data.columns       = std::stoul(tileset->Attribute("columns"));

		std::vector<sf::Vector2f> texture_grid;
		texture_grid.reserve(data.tile_count);

		size_t rows = data.tile_count / data.columns;
		size_t columns = data.columns;

		for (size_t y = 0u; y < rows; ++y)
			for (size_t x = 0u; x < columns; ++x)
				texture_grid.emplace_back(sf::Vector2f((float)x * data.tile_width, (float)y * data.tile_height));

		for (size_t i = 0u; i < data.tile_count; ++i)
		{
			TilesetData::Quad quad;

			quad.leftTop.texCoords     = sf::Vector2f(texture_grid[i].x,                   texture_grid[i].y);
			quad.rightTop.texCoords    = sf::Vector2f(texture_grid[i].x + data.tile_width, texture_grid[i].y);
			quad.rightBottom.texCoords = sf::Vector2f(texture_grid[i].x + data.tile_width, texture_grid[i].y + data.tile_height);
			quad.leftBottom.texCoords  = sf::Vector2f(texture_grid[i].x,                   texture_grid[i].y + data.tile_height);

			TilesetData::texture_rects.push_back(quad);
		}

		auto source = tileset->FirstChildElement("image");
		std::string path = source->Attribute("source");

		while (!isalpha(path.front())) // If path begins like this: "../textures/Tiles.png", for example
			path.erase(0, 1);

		if (!data.tileset.loadFromFile(path))
			return false;
		
		tilesets.push_back(std::move(data));
	}
	return true;
}

bool TileMap::loadLayers(tinyxml2::XMLElement* map)
{
	static bool first_layer_is_loaded = false;

	for (auto layer = map->FirstChildElement("layer");
		      layer != nullptr;
		      layer = layer->NextSiblingElement("layer"))
	{
		const size_t width  = std::stoul(layer->Attribute("width"));
		const size_t height = std::stoul(layer->Attribute("height"));

		tinyxml2::XMLElement* data = layer->FirstChildElement("data");
		std::string dirty_string = data->GetText(), buffer;

		std::vector<size_t> csv_array;
		csv_array.reserve(dirty_string.size());

		for (auto& character : dirty_string)
		{
			if (isdigit(character))
				buffer += character;
			else
				if (!buffer.empty())
				{
					csv_array.push_back(std::stoi(buffer));
					buffer.clear();
				}
		}
		csv_array.shrink_to_fit();

		if (!first_layer_is_loaded)
		{
			collision_layer.reserve(height);

			for (size_t i = 0u; i < height; ++i)
			{
				collision_layer.push_back(std::vector<size_t>());
				collision_layer[i].reserve(width);
			}		

			for (size_t i = 0u, index = 0u; i < height; ++i)
				for (size_t j = 0u; j < width; ++j, ++index)
					collision_layer[i].push_back(csv_array[index]);

			first_layer_is_loaded = true;
		}

		for (size_t i = 0; i < tilesets.size(); ++i)
		{
			size_t first_tile = tilesets[i].first_tile_id;
			size_t last_tile  = first_tile + tilesets[i].tile_count;

			auto match = std::find_if(csv_array.begin(), csv_array.end(), [&](size_t tile_num)
				{					
					return tile_num >= first_tile && tile_num < last_tile;
				});

			if (match != csv_array.end())
				tile_layers.push_back(TileLayer(tilesets[i], sf::Vector2u(width, height), csv_array));
		}	 
	}
	return true;
}

void TileMap::loadObjects(tinyxml2::XMLElement* map)
{
	for (auto object_group = map->FirstChildElement("objectgroup");
		      object_group != nullptr; 
		      object_group = object_group->NextSiblingElement("objectgroup"))
	{
		for (auto object = object_group->FirstChildElement("object");
			      object != nullptr;
			      object = object->NextSiblingElement("object"))
		{
			std::string object_name;
			if (object->Attribute("name"))
				object_name = object->Attribute("name");

			std::string object_type;
			if (object->Attribute("type"))			
				object_type = object->Attribute("type");
						
			float x = std::stof(object->Attribute("x"));
			float y = std::stof(object->Attribute("y"));

			float width{}, height{};

			if (object->Attribute("width") && object->Attribute("height"))
			{
				width  = std::stof(object->Attribute("width"));
				height = std::stof(object->Attribute("height"));
			}			

			Object new_object(x, y, width, height);
			new_object.name = object_name;
			new_object.type = object_type;

			auto properties = object->FirstChildElement("properties");

			if (properties)
			{
				for (auto property = properties->FirstChildElement("property");
					      property != nullptr;
					      property = property->NextSiblingElement("property"))
				{
					std::string name, type, value;

					if (property->Attribute("name"))
						name = property->Attribute("name");

					if (property->Attribute("type"))
						type = property->Attribute("type");

					if (property->Attribute("value"))
						value = property->Attribute("value");

					new_object.properties.emplace_back(Property(name, type, value));
				}
			}			
			objects.emplace_back(std::move(new_object));
		}
	}
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto& layer : tile_layers)
		target.draw(layer);
}
