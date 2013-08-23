#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cstring>

#include "entity.hpp"
#include "map.hpp"

Map::Map( size_t w, size_t h, size_t ts )
{
	mWidth	  = w;
	mHeight	  = h;
	mTileSize = ts;

	mMapTexture.create( w * mTileSize, h * mTileSize);

	mMapData = new sf::Uint8[mWidth * mHeight];
	std::memset( mMapData, 0, mWidth * mHeight );
}

Map::~Map()
{
	delete mMapData;
}

sf::Uint8& Map::getTile( size_t x, size_t y ) const
{
	return mMapData[( x * mWidth ) + y];
}

//Make a square of tiles in our map
void Map::makeSquare( sf::Uint8 type, size_t x, size_t y, size_t w, size_t h )
{
	int i, j;

	for( i = x; i < w; i++ )
	{
		for( j = y; j < h; j++ )
		{
			getTile( i, j ) = type;
		}
	}
}

//Render the tiles to a RenderTexture
void Map::drawTiles( sf::RenderTexture& dst, sf::Sprite src, sf::Uint8 type )
{
	int i, j;

	for( i = 0; i < mWidth; i++ )
	{
		for( j = 0; j < mHeight; j++ )
		{
			if( getTile( i, j ) == type )
			{
				//Note that the Y coord is "reversed" to fit with SFML's coordinate system
				src.setPosition( sf::Vector2f( i * mTileSize, ( mHeight - j ) * mTileSize ) );
				dst.draw( src );
			}
		}
	}
}

//Get the tile enclosing the given coordinate
sf::Vector2f Map::getTileCoordForPoint( sf::Vector2f point )
{
	size_t x = (size_t)point.x;
	size_t y = (size_t)point.y;

	return sf::Vector2f( x / mTileSize, y / mTileSize );
}

//AABB test to check collision with a given tile
bool Map::collidesWithTile( sf::FloatRect other, size_t x, size_t y )
{
	sf::FloatRect tile( sf::Vector2f( mTileSize * x, mTileSize * y ), 
		   sf::Vector2f( mTileSize, mTileSize ) );

	return tile.intersects( other );
}

DungeonMap::DungeonMap() : Map( 64, 64, 16 )
{
	if( !mFloorTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 6 * 16, 1 * 16, 16, 16 ) ) )
	{
		std::cout << "Error loading floor texture from res/basictiles.png!" << std::endl;
	}

	if( !mPlayerSpawnTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 1 * 16, 7 * 16, 16, 16 ) ) )
	{
		std::cout << "Error loading floor texture from res/basictiles.png!" << std::endl;
	}

	makeSquare( TILE_FLOOR, 5, 5, 10, 10 );
	getTile( 6, 6 ) = TILE_PLAYER_SPAWN;
	
	drawTiles( mMapTexture, sf::Sprite( mFloorTexture ), TILE_FLOOR );
	drawTiles( mMapTexture, sf::Sprite( mPlayerSpawnTexture ), TILE_PLAYER_SPAWN );
	mMapSprite.setTexture( mMapTexture.getTexture() );
}

sf::Sprite& DungeonMap::getSprite()
{
	return mMapSprite;
}

//Get the first player spawn in the map from bottom to top
sf::Vector2f DungeonMap::getPlayerSpawn()
{
	int i, j;

	for( i = 0; i < mWidth; i++ )
	{
		for( j = 0; j < mHeight; j++ )
		{
			if( getTile( i, j ) == TILE_PLAYER_SPAWN )
			{
				return sf::Vector2f( i * mTileSize, j * mTileSize );
			}
		}
	}
}
