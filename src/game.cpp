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
#include <vector>
#include <iostream>

#include "entity.hpp"
#include "map.hpp"
#include "game.hpp"

Game::Game() : mNozState( this )
{
	mWindowWidth  = 800;
	mWindowHeight = 600;
}

void Game::openWindow()
{
	mWindow = new sf::RenderWindow( sf::VideoMode( mWindowWidth, mWindowHeight ), "nozoki" );
	mWindow->setVerticalSyncEnabled(true);
}

int GameState::getDelta()
{
	return mParent->getDelta();
}

//Main loop
void Game::doLoop()
{
	openWindow();

	setState( &mNozState );

	mWindow->setKeyRepeatEnabled( false );

	mFrameTime = mDeltaClock.restart().asMilliseconds();

	while( mWindow->isOpen() )
	{
		mState->handleInput();

		mWindow->clear( sf::Color::Black );

		mState->doFrame();

		mWindow->display();

		mFrameTime = mDeltaClock.restart().asMilliseconds();
	}
}

void Game::setState( GameState *state )
{
	state->initState();
	mState = state;
}

GameState::GameState( Game *parent )
{
	mParent = parent;
}

NozokiState::NozokiState( Game *parent ) : GameState( parent )
{
}

void NozokiState::initState()
{
	mPlayer.loadResources();
	mPlayer.setPosition( mMap.getPlayerSpawn() );
	spawnEnemies();
	mView.reset( sf::FloatRect( 0, 0, 800, 600 ) );
	mView.zoom( 1.0f );
}

//Called by the game object every frame
void NozokiState::doFrame()
{
	//Set our view
	mParent->mWindow->setView( mView );
	
	//Set the map's position
	mMap.getSprite().setPosition( sf::Vector2f( 0.0f, mMap.getTileSize() ) );
	
	//Draw it
	mParent->mWindow->draw( mMap.getSprite() );

	//Update and draw all the other entities
	for( auto it = mEntities.begin(); it != mEntities.end(); it++ )
	{
		( *it )->update( this );
		mParent->mWindow->draw( ( *it )->getSprite() );
	}

	//Then the player
	mPlayer.update( this );

	//Center the camera on the player
	mView.setCenter( mPlayer.getPosition() );

	//Re-set the view of our window to the updated one
	mParent->mWindow->setView( mView );

	//And draw the player to it
	mParent->mWindow->draw( mPlayer.getSprite() );
}

void NozokiState::handleInput()
{
	sf::Event event;

	while( mParent->mWindow->pollEvent( event ) )
	{
		if( event.type == sf::Event::Closed ) 
		{
			mParent->mWindow->close();
		}
	
		mPlayer.handleEvent( event );
		for( auto it = mEntities.begin(); it != mEntities.end(); it++ )
		{
			( *it )->handleEvent( event );
		}
	}
}

void NozokiState::spawnEnemies()
{
	int i, j;

	for( i = 0; i < mMap.getWidth(); i++ )
	{
		for( j = 0; j < mMap.getHeight(); j++ )
		{
			if( mMap.getTile( i, j ) == TILE_ENEMY_SPAWN )
			{
				mEntities.push_back( new Slime( mMap.getCoordForTile( i, j ) ) );
			}
		}
	}
}
