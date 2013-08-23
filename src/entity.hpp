#ifndef ENTITY_HPP
#define ENTITY_HPP

class GameState;
class NozokiState;

enum {
	PLAYER_IDLE,
	PLAYER_WALKING
};

enum {
	DIRECTION_RIGHT = 0,
	DIRECTION_LEFT,
	DIRECTION_UP,
	DIRECTION_DOWN
};

//Base animation class, takes a sequence of sprites and will return the appropriate one
class Animation
{
public:
	Animation( int = 1000 );
	Animation( int, sf::Sprite, sf::Sprite );
	sf::Sprite&	getCurrentFrame();
	void		addFrame( sf::Sprite );
	void		setDelay( int );
	void		reset();

private:
	std::vector<sf::Sprite>	mFrames;
	int			mCurrentFrame;
	bool			mLoop;
	int			mDelay;
	sf::Clock		mClock;
};

//Base entity class
class Entity
{
public:
	virtual void update( GameState * ) {}
	virtual sf::Sprite& getSprite()	= 0;
	virtual void loadResources()	= 0;
	virtual void setDirection( int direction ) { mDirection	= direction; }
	virtual void setPosition( sf::Vector2f position ) { mPosition = position; }
	virtual sf::Vector2f getPosition() { return mPosition; }
	virtual void move( sf::Vector2f offset ) { mPosition += offset; }
	virtual void handleEvent( sf::Event ) {}
	virtual sf::FloatRect getAABB() = 0;
	virtual sf::Vector2f getScale() { return mScale; }

protected:
	int		mState;
	sf::Vector2f	mPosition;
	int		mDirection;
	sf::Vector2f	mScale;
};

//Our player
class Player : public Entity
{
public:
	Player();
	void		update( GameState * );
	sf::Sprite& getSprite();
	void handleEvent( sf::Event );
	void loadResources();
	void setState( int state ) { mState = state; }
	void setVelocity( sf::Vector2f vel ) { mVelocity = vel; }
	sf::Vector2f getVelocity() { return mVelocity; }
	sf::FloatRect getAABB() { return sf::FloatRect( mPosition,  mScale ); }
	int mWalkSpeed;
 
private:
	bool checkCollisions( NozokiState* );

	sf::Texture	mTexture;
	sf::Sprite	mIdleSprites[4];
	Animation	mWalkingAnims[4];
	sf::Vector2f	mVelocity;
};

#endif
