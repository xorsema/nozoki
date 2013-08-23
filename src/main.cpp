#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "entity.hpp"
#include "map.hpp"
#include "game.hpp"

Game game;

int main( int argc, char **argv )
{
	game.doLoop();
	return 0;
}
