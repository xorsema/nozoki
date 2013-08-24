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
#include <cstring>

#include "entity.hpp"
#include "map.hpp"

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

DungeonMap::DungeonMap() : Map( 512, 512, 16 )
{
	if( !mFloorTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 6 * 16, 1 * 16, 16, 16 ) ) )
	{
		std::cout << "Error loading floor texture from res/basictiles.png!" << std::endl;
	}

	if( !mPlayerSpawnTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 1 * 16, 7 * 16, 16, 16 ) ) )
	{
		std::cout << "Error loading floor texture from res/basictiles.png!" << std::endl;
	}

	makeSpawnRoom( 256, 256, 40, 20 );

	drawTiles( mMapTexture, sf::Sprite( mFloorTexture ), TILE_FLOOR );
	drawTiles( mMapTexture, sf::Sprite( mPlayerSpawnTexture ), TILE_PLAYER_SPAWN );
	mMapSprite.setTexture( mMapTexture.getTexture() );
}

sf::Sprite& DungeonMap::getSprite()
{
	return mMapSprite;
}

void DungeonMap::makeSpawnRoom( size_t x, size_t y, size_t w, size_t h )
{
	makeSquare( TILE_FLOOR, x, y, w, h );
	
	getTile( x + ( w / 2 ), y + ( h / 2 ) ) = TILE_PLAYER_SPAWN; 
}

void DungeonMap::makeHallway( int direction, size_t x, size_t y, size_t length )
{
	int i, j, c;

	switch( direction )
	{
	case DIRECTION_UP:
		for( i = x, j = y, c = 0; c < length; c++, j++ )
		{
			getTile( i, j ) = TILE_FLOOR;
		}
		break;

	case DIRECTION_DOWN:
		for( i = x, j = y, c = 0; c < length; c++, j-- )
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
