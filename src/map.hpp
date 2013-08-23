#ifndef MAP_HPP
#define MAP_HPP

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
	virtual sf::Sprite& getSprite() = 0;
	void		drawTiles( sf::RenderTexture&, sf::Sprite, sf::Uint8 );
	sf::Vector2f	getTileCoordForPoint( sf::Vector2f );
	bool		collidesWithTile( sf::FloatRect, size_t, size_t );
	size_t getWidth() { return mWidth; }
	size_t getHeight() { return mHeight; }
	size_t getTileSize() { return mTileSize; }
	sf::FloatRect getAABB() { return sf::FloatRect( sf::Vector2f( 0, 0 ), sf::Vector2f( mWidth * mTileSize, mHeight * mTileSize ) ); }

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
	sf::Texture	mFloorTexture;
	sf::Texture	mPlayerSpawnTexture;
};

#endif
