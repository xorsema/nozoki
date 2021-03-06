/*
    The MIT License (MIT)

    Copyright (c) 2013 Max Rose

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

    This file is part of Nozoki.
*/

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include <cstring>

#include "entity.hpp"
#include "map.hpp"

std::mt19937 gRanNumGen;

Map::Map( size_t w, size_t h, size_t ts )
{
	mWidth	  = w;
	mHeight	  = h;
	mTileSize = ts;

	mMapTexture.create( w * mTileSize, h * mTileSize);
	mMapTexture.clear( sf::Color::Black );

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
	int i, j, ci, cj;

	for( i = x, ci = 0; ci < w; i++, ci++ )
	{
		for( j = y, cj = 0; cj < h; j++, cj++ )
		{
			getTile( i, j ) = type;
		}
	}
}

void Map::makeCenteredSquare( sf::Uint8 type, size_t x, size_t y, size_t w, size_t h )
{
	makeSquare( type, x - ( w / 2 ), y - ( h / 2 ), w, h );
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
sf::Vector2i Map::getTileCoordForPoint( sf::Vector2f point )
{
	size_t x = (size_t)point.x;
	size_t y = (size_t)point.y;

	return sf::Vector2i( x / mTileSize, y / mTileSize );
}

sf::Uint8 Map::getTileForPoint( sf::Vector2f point )
{
	return getTile( ( (size_t)point.x ) / mTileSize, ( (size_t)point.y ) / mTileSize );
}

sf::Vector2f Map::getCoordForTile( size_t x, size_t y )
{
	return sf::Vector2f( x * mTileSize, y * mTileSize );
}

//AABB test to check collision with a given tile
bool Map::collidesWithTile( sf::FloatRect other, size_t x, size_t y )
{
	sf::FloatRect tile( sf::Vector2f( mTileSize * x, mTileSize * y ), 
			    sf::Vector2f( mTileSize, mTileSize ) );

	return tile.intersects( other );
}

//Return true if a given square is empty
bool Map::isSquareEmpty( size_t x, size_t y, size_t w, size_t h )
{
	int i, j, ci, cj;
	
	for( i = x, ci = 0; ci < w; i++, ci++ )
	{
		for( j = y, cj = 0; cj < h; j++, cj++ )
		{
			if( getTile( i, j ) != TILE_NONE )
			{
				return false;
			}
		}
	}

	return true;
}

bool Map::isTouchingTileType( sf::Uint8 type, sf::FloatRect AABB )
{
	if( getTileForPoint( sf::Vector2f( AABB.left, AABB.top ) ) == type ||
	    getTileForPoint( sf::Vector2f( AABB.left + AABB.width, AABB.top ) ) == type ||
	    getTileForPoint( sf::Vector2f( AABB.left, AABB.top + AABB.height ) ) == type ||
	    getTileForPoint( sf::Vector2f( AABB.left + AABB.width, AABB.top + AABB.height ) ) == type )
	{
		return true;
	}

	return false;
}

bool Map::isInsideMap( sf::FloatRect AABB )
{
	return !( AABB.top + AABB.height > mHeight * mTileSize ||
		  AABB.left < 0 ||
		  AABB.left + AABB.width > mWidth * mTileSize ||
		  AABB.top < 0 );
}

DungeonMap::DungeonMap() : Map( 512, 512, 16 )
{
	gRanNumGen.seed( std::chrono::system_clock::now().time_since_epoch().count() );

	if( !mFloorTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 6 * 16, 1 * 16, 16, 16 ) ) )
	{
		std::cout << "Error loading floor texture from res/basictiles.png!" << std::endl;
	}

	if( !mPlayerSpawnTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 1 * 16, 7 * 16, 16, 16 ) ) )
	{
		std::cout << "Error loading floor texture from res/basictiles.png!" << std::endl;
	}

	sf::IntRect spawnRect = makeSpawnRoom( 256, 256, 10, 10 );
	
	generateRooms( spawnRect, 10 );
	generateRooms( spawnRect, 10 );
	generateRooms( spawnRect, 10 );

	drawTiles( mMapTexture, sf::Sprite( mFloorTexture ), TILE_FLOOR );
	drawTiles( mMapTexture, sf::Sprite( mFloorTexture ), TILE_ENEMY_SPAWN );
	drawTiles( mMapTexture, sf::Sprite( mPlayerSpawnTexture ), TILE_PLAYER_SPAWN );
	mMapSprite.setTexture( mMapTexture.getTexture() );
}

sf::Sprite& DungeonMap::getSprite()
{
	return mMapSprite;
}

sf::IntRect DungeonMap::makeSpawnRoom( size_t x, size_t y, size_t w, size_t h )
{
	makeSquare( TILE_FLOOR, x, y, w, h );
	
	getTile( x + ( w / 2 ), y + ( h / 2 ) ) = TILE_PLAYER_SPAWN; 

	return sf::IntRect( x, y, w, h );
}

void DungeonMap::makeHallway( int direction, size_t x, size_t y, size_t length )
{
	int i, j, c;

	switch( direction )
	{
	case DIRECTION_UP:
		for( i = x, j = y, c = 0; c < length; c++, j-- )
		{
			getTile( i, j ) = TILE_FLOOR;
		}
		break;

	case DIRECTION_DOWN:
		for( i = x, j = y, c = 0; c < length; c++, j++ )
		{
			getTile( i, j ) = TILE_FLOOR;
		}
		break;

	case DIRECTION_RIGHT:
		for( i = x, j = y, c = 0; c < length; c++, i++ )
		{
			getTile( i, j ) = TILE_FLOOR;
		}
		break;

	case DIRECTION_LEFT:
		for( i = x, j = y, c = 0; c < length; c++, i-- )
		{
			getTile( i, j ) = TILE_FLOOR;
		}
		break;
	}
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

void DungeonMap::furnishRoom( sf::IntRect room, bool placeExit )
{
	int i;
	std::uniform_int_distribution<int> enemyAmount( 0, 5 );
	std::uniform_int_distribution<int> enemyX( 0, room.width - 1 );
	std::uniform_int_distribution<int> enemyY( 0, room.height - 1 );

	for( i = 0; i < enemyAmount( gRanNumGen ); i++ )
	{
		getTile( room.left + enemyX( gRanNumGen ), room.top + enemyY( gRanNumGen ) ) = TILE_ENEMY_SPAWN;
	}
}

sf::IntRect DungeonMap::generateRooms( sf::IntRect start, size_t depth )
{
	if( depth == 0 )
	{
		return start;
	}

	size_t	roomWidth  = 20;
	size_t	roomHeight = 20;
	size_t	hallWidth  = 2;
	size_t	hallLen	   = 15;

	sf::Vector2i roomStart;
	sf::Vector2i hallStart;

	size_t targetHallWidth;
	size_t targetHallHeight;

	sf::IntRect result;

	std::uniform_int_distribution<int> dirRand( 0, 3 );
	std::uniform_int_distribution<int> subDepthRand( 0, 5 );

	int	direction = dirRand( gRanNumGen );
	int	subDepth  = subDepthRand( gRanNumGen );

	switch( direction )
	{
	case DIRECTION_RIGHT:
		hallStart.x	 = start.left + start.width;
		hallStart.y	 = start.top + ( start.height / 2 );
		targetHallWidth	 = hallLen;
		targetHallHeight = hallWidth;
		roomStart.x	 = hallStart.x + hallLen;
		roomStart.y	 = start.top;
		break;

	case DIRECTION_LEFT:
		hallStart.x	 = start.left - hallLen;
		hallStart.y	 = start.top + ( start.height / 2 );
		targetHallWidth	 = hallLen;
		targetHallHeight = hallWidth;
		roomStart.x	 = hallStart.x - roomWidth;
		roomStart.y	 = start.top;
		break;

	case DIRECTION_UP:
		hallStart.x	 = start.left + ( start.width / 2 );
		hallStart.y	 = start.top - hallLen;
		targetHallWidth	 = hallWidth;
		targetHallHeight = hallLen;		
		roomStart.x	 = start.left;
		roomStart.y	 = hallStart.y - roomHeight;
		break;

	case DIRECTION_DOWN:
		hallStart.x	 = start.left + ( start.width / 2 );
		hallStart.y	 = start.top + start.width;
		targetHallWidth	 = hallWidth;
		targetHallHeight = hallLen;		
		roomStart.x	 = start.left;
		roomStart.y	 = hallStart.y + hallLen;
		break;
	}

	if( !isInsideMap( sf::FloatRect( hallStart.x * mTileSize, hallStart.y * mTileSize, targetHallWidth * mTileSize, targetHallHeight * mTileSize ) ) ||
	    !isInsideMap( sf::FloatRect( roomStart.x, roomStart.y, roomWidth, roomHeight ) ) )	
	{
		return start;
	}

	if( !isSquareEmpty( hallStart.x, hallStart.y, targetHallWidth, targetHallHeight ) ||
	    !isSquareEmpty( roomStart.x, roomStart.y, roomWidth, roomHeight ) )
	{
		return generateRooms( start, --depth );
	}

	makeSquare( TILE_FLOOR, hallStart.x, hallStart.y, targetHallWidth, targetHallHeight );
	makeSquare( TILE_FLOOR, roomStart.x, roomStart.y, roomWidth, roomHeight );

	result = sf::IntRect( roomStart, sf::Vector2i( roomWidth, roomHeight ) );
	furnishRoom( result, false );

	generateRooms( result, subDepth );
	
	return generateRooms( result, --depth );
}
