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
	if( !state->getMap().isTouchingTileType( TILE_NONE, sf::FloatRect( mPosition + ( mVelocity * ( state->getDelta() / 1000.0f ) ), mScale ) ) )
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

Enemy::Enemy( sf::Vector2f pos )
{
	mPosition   = pos;
	mVelocity.x = 30.0f;
	mVelocity.y = 0.0f;
	mScale.x    = 16.0f;
	mScale.y    = 16.0f;
	mDirection  = DIRECTION_RIGHT;
	mState	    = ENEMY_WALKING;
	loadResources();
}

void Enemy::loadResources()
{
	if( !mTexture.loadFromFile( "res/basictiles.png", sf::IntRect( 0, 12 * 16, 3 * 16, 16 ) ) )
	{
		std::cout << "Error loading textures from res/basictiles.png!" << std::endl;
	}	

	mWalkAnim = Animation( 300,
			       sf::Sprite( mTexture, sf::IntRect( 0, 0, 16, 16 ) ),
			       sf::Sprite( mTexture, sf::IntRect( 16, 0, 16, 16 ) ) );
	mIdleSprite = sf::Sprite( mTexture, sf::IntRect( 0, 0, 16, 16 ) );
}

sf::Sprite& Enemy::getSprite()
{
	switch( mState )
	{
	case ENEMY_IDLE:
		switch( mDirection )
		{
		case DIRECTION_RIGHT:
			mIdleSprite.setScale( -1.0f, 1.0f );
			mIdleSprite.move( mScale.x, 0 );
			return mIdleSprite;
			
		case DIRECTION_LEFT:
			return mIdleSprite;

		case DIRECTION_UP:
			return mIdleSprite;

		case DIRECTION_DOWN:
			return mIdleSprite;
		}
		break;

	case ENEMY_WALKING:
		switch( mDirection )
		{
		case DIRECTION_RIGHT:
			mWalkAnim.getCurrentFrame().setScale( -1.0f, 1.0f );
			mWalkAnim.getCurrentFrame().move( mScale.x, 0 );
			return mWalkAnim.getCurrentFrame();
			
		case DIRECTION_LEFT:
			mWalkAnim.getCurrentFrame().setScale( 1.0f, 1.0f );
			return mWalkAnim.getCurrentFrame();
			
		case DIRECTION_UP:
			return mWalkAnim.getCurrentFrame();
			
		case DIRECTION_DOWN:
			return mWalkAnim.getCurrentFrame();

		}
		break;
	}
}

void Enemy::update( GameState *gs )
{
	NozokiState *state = (NozokiState*)gs;
	
	if( state->getMap().isTouchingTileType( TILE_NONE, sf::FloatRect( mPosition + ( mVelocity * ( state->getDelta() / 1000.0f ) ), mScale ) ) )
	{
		mDirection = ( mDirection == DIRECTION_RIGHT ) ? DIRECTION_LEFT : DIRECTION_RIGHT;
		mVelocity.x *= -1;
		mPosition += ( mVelocity * ( state->getDelta() / 1000.0f ) );
	} 
	else
	{
		mPosition += ( mVelocity * ( state->getDelta() / 1000.0f ) );
	}

	getSprite().setPosition( mPosition );
}
