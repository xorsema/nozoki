#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>

#include "map.hpp"
#include "entity.hpp"
#include "game.hpp"

Animation::Animation( int delay )
{
	setDelay( delay );
	mCurrentFrame = 0;
}

Animation::Animation( int delay, sf::Sprite a, sf::Sprite b )
{
	setDelay( delay );
	mFrames.push_back( a );
	mFrames.push_back( b );
	mCurrentFrame = 0;
}

void Animation::addFrame( sf::Sprite sprite )
{
	mFrames.push_back( sprite );
}

//Figure out the correct sprite to return this frame of the animation
sf::Sprite& Animation::getCurrentFrame()
{
	if( mClock.getElapsedTime().asMilliseconds() >= mDelay )
	{
		mClock.restart();
		mCurrentFrame++;

		if( mCurrentFrame == mFrames.size() )
		{
			mCurrentFrame = 0;
		}
	}

	return mFrames[mCurrentFrame];
}

void Animation::setDelay( int delay )
{
	mClock.restart();
	mDelay = delay;
}

void Animation::reset()
{
	mClock.restart();
	mCurrentFrame = 0;
}

Player::Player()
{
	mState = PLAYER_IDLE;
	mWalkSpeed = 75;
	mScale.x = 16.0f;
	mScale.y = 16.0f;
}

//Return the right sprite depending on state
sf::Sprite& Player::getSprite()
{
	switch( mState )
	{
	case PLAYER_IDLE:
		return mIdleSprites[mDirection];

	case PLAYER_WALKING:
		return mWalkingAnims[mDirection].getCurrentFrame();
	}
}

void Player::handleEvent( sf::Event e )
{
}

//Make sure the player does not go out of bounds, if we're in the bounds, return true
bool Player::checkCollisions( NozokiState *state )
{
	int i, j;

	for( i = 0; i < state->getMap().getWidth(); i++ )
	{
		for( j = 0; j < state->getMap().getHeight(); j++ )
		{
			if( state->getMap().getTile( i, j ) == TILE_NONE && 
			    state->getMap().collidesWithTile( sf::FloatRect( mPosition + ( mVelocity * ( state->getDelta() / 1000.0f ) ), mScale ), i, j ) )
			{
				return false;
			}
		}
	}

	return true;
}

void Player::update( GameState *gs )
{
	NozokiState *state = (NozokiState*)gs;

	//Do movement
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) &&
	    !sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) )
	{
		setState( PLAYER_WALKING );
		mDirection = DIRECTION_RIGHT;
		mVelocity = sf::Vector2f( mWalkSpeed, 0 );
	}

	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) &&
	    !sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) )
	{
		setState( PLAYER_WALKING );
		mDirection = DIRECTION_LEFT;
		mVelocity = sf::Vector2f( -mWalkSpeed, 0 );
	}

	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) &&
	    !sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) )
	{
		setState( PLAYER_WALKING );
		mDirection = DIRECTION_UP;
		mVelocity = sf::Vector2f( 0, -mWalkSpeed );
	}

	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) &&
	    !sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) )
	{
		setState( PLAYER_WALKING );
		mDirection = DIRECTION_DOWN;
		mVelocity = sf::Vector2f( 0, mWalkSpeed );
	}

	if( !sf::Keyboard::isKeyPressed( sf::Keyboard::Up) &&
	    !sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) &&
	    !sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) &&
	    !sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) )
	{
		mVelocity = sf::Vector2f( 0, 0 );
		setState( PLAYER_IDLE );
		
	}

	//Move only if we won't hit something we shouldn't
	if( checkCollisions( state ) ) 
	{
		mPosition += ( mVelocity * ( state->getDelta() / 1000.0f ) );
	}
	
	//Update the actual sprite
	getSprite().setPosition( mPosition );
}

void Player::loadResources()
{
	if( !mTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 0, 9 * 16, 8 * 16, 16 ) ) )
	{
		std::cout << "Error loading textures from res/basictiles.png!" << std::endl;
	}

	mIdleSprites[DIRECTION_RIGHT] = sf::Sprite( mTexture, sf::IntRect( 2 * 16, 0, 16, 16 ) );
	mIdleSprites[DIRECTION_LEFT] = sf::Sprite( mTexture, sf::IntRect( 6 * 16, 0, 16, 16 ) );
	mIdleSprites[DIRECTION_UP] = sf::Sprite( mTexture, sf::IntRect( 4 * 16, 0, 16, 16 ) );
	mIdleSprites[DIRECTION_DOWN] = sf::Sprite( mTexture, sf::IntRect( 1 * 16, 0, 16, 16 ) );

	mWalkingAnims[DIRECTION_RIGHT] = Animation( 300, 
						    sf::Sprite( mTexture, sf::IntRect( 3 * 16, 0, 16, 16 ) ),
						    sf::Sprite( mTexture, sf::IntRect( 2 * 16, 0, 16, 16 ) ) );

	mWalkingAnims[DIRECTION_LEFT] = Animation( 300, 
						    sf::Sprite( mTexture, sf::IntRect( 6 * 16, 0, 16, 16 ) ),
						    sf::Sprite( mTexture, sf::IntRect( 7 * 16, 0, 16, 16 ) ) );

	mWalkingAnims[DIRECTION_UP] = Animation( 300, 
						    sf::Sprite( mTexture, sf::IntRect( 5 * 16, 0, 16, 16 ) ),
						    sf::Sprite( mTexture, sf::IntRect( 4 * 16, 0, 16, 16 ) ) );

	mWalkingAnims[DIRECTION_DOWN] = Animation( 300, 
						    sf::Sprite( mTexture, sf::IntRect( 0 * 16, 0, 16, 16 ) ),
						    sf::Sprite( mTexture, sf::IntRect( 1 * 16, 0, 16, 16 ) ) );
}
