#include "TileMap.h"
#include "Mario.h"


int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 510), "TileMap Test");
	window.setFramerateLimit(60u);

	sf::View view_port(sf::FloatRect(0, 0, 800, 510));

	//  Create background
	sf::Texture sky_texture;
	sky_texture.loadFromFile("textures/Sky.png");
	sky_texture.setRepeated(true);
	sf::Sprite sky(sky_texture, sf::IntRect(0, 0, 230 * 32, 16 * 32));

	TileMap map;
	map.load("tmx_files/World_1-1.tmx");

	sf::Texture mario_texture;
	mario_texture.loadFromFile("textures/Mario.png");
	Mario mario(mario_texture, map);

	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			mario.dx = -3.0f;
		else
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			mario.dx = 3.0f;
		else
			mario.dx = 0.0f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			if (mario.on_ground)
			{
				mario.dy = -9.0f;
				mario.on_ground = false;
			}

		mario.update();

		sf::Vector2f viewport_center(mario.x, 255.f);

		if (viewport_center.x < 400.f) 
			viewport_center.x = 400.f;

		view_port.setCenter(viewport_center);
		window.setView(view_port);

		window.clear();
		window.draw(sky);
		window.draw(map);
		window.draw(mario);
		window.display();
	}
	return 0;
}