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

#ifndef MAP_HPP
#define MAP_HPP

extern std::mt19937 gRanNumGen;


enum {
	TILE_NONE = 0,
	TILE_FLOOR,
	TILE_PLAYER_SPAWN,
	TILE_ENEMY_SPAWN
};

//Base map class
class Map
{
public:
	Map( size_t, size_t, size_t );
	~Map();
	sf::Uint8& getTile( size_t, size_t ) const;
	void		makeSquare( sf::Uint8, size_t, size_t, size_t, size_t );
	void		makeCenteredSquare( sf::Uint8, size_t, size_t, size_t, size_t );
	virtual sf::Sprite& getSprite() = 0;
	void		drawTiles( sf::RenderTexture&, sf::Sprite, sf::Uint8 );
	sf::Vector2f	getTileCoordForPoint( sf::Vector2f );
	bool		collidesWithTile( sf::FloatRect, size_t, size_t );
	size_t getWidth() { return mWidth; }
	size_t getHeight() { return mHeight; }
	size_t getTileSize() { return mTileSize; }
	sf::FloatRect getAABB() { return sf::FloatRect( sf::Vector2f( 0, 0 ), sf::Vector2f( mWidth * mTileSize, mHeight * mTileSize ) ); }
	bool isSquareEmpty( size_t, size_t, size_t, size_t );
	bool isTouchingTileType( sf::Uint8, sf::FloatRect );
	sf::Vector2f getCoordForTile( size_t, size_t );
	bool isInsideMap( sf::FloatRect );

protected:
	sf::Uint8		*mMapData;
	size_t			 mWidth;
	size_t			 mHeight;
	size_t			 mTileSize;
	sf::RenderTexture	 mMapTexture;
	sf::Sprite		 mMapSprite;
};

//Map subclass used for the main game
class DungeonMap : public Map
{
public:
	DungeonMap();
	sf::Sprite& getSprite();
	sf::Vector2f getPlayerSpawn();

private:
	sf::IntRect generateRooms( sf::IntRect, size_t );
	sf::IntRect makeSpawnRoom( size_t, size_t, size_t, size_t );
	void makeHallway( int, size_t, size_t, size_t );

	sf::Texture	mFloorTexture;
	sf::Texture	mPlayerSpawnTexture;
};

#endif
