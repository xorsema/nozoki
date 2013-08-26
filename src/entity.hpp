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

#ifndef ENTITY_HPP
#define ENTITY_HPP

class GameState;
class NozokiState;

enum {
	PLAYER_IDLE,
	PLAYER_WALKING
};

enum {
	ENEMY_IDLE,
	ENEMY_WALKING
};

enum {
	DIRECTION_RIGHT = 0,
	DIRECTION_LEFT	= 1,
	DIRECTION_UP	= 2,
	DIRECTION_DOWN	= 3
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
	virtual sf::FloatRect getAABB() { return sf::FloatRect( mPosition,  mScale ); }
	virtual sf::Vector2f getScale() { return mScale; }
	virtual sf::Vector2f getVelocity() { return mVelocity; }

protected:
	int		mState;
	sf::Vector2f	mPosition;
	int		mDirection;
	sf::Vector2f	mScale;
	sf::Vector2f	mVelocity;
};

//Our player
class Player : public Entity
{
public:
	Player();
	void		update( GameState * );
	sf::Sprite& getSprite();
	void loadResources();
	void setState( int state ) { mState = state; }
	void setVelocity( sf::Vector2f vel ) { mVelocity = vel; }
	sf::Vector2f getVelocity() { return mVelocity; }
	int mWalkSpeed;
 
private:
	sf::Texture	mTexture;
	sf::Sprite	mIdleSprites[4];
	Animation	mWalkingAnims[4];
};

class Enemy : public Entity
{
public:
	Enemy( sf::Vector2f );
	virtual sf::Sprite& getSprite();
	virtual void loadResources();
	virtual void update( GameState * );

private:
	static sf::Texture	mTexture;
	static bool		mTextureInit;
	Animation		mWalkAnim;
	sf::Sprite		mIdleSprite;
};

#endif
