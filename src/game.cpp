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
	mWindow->setVerticalSyncEnabled(false);
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
	mView.reset( sf::FloatRect( 0, 0, 800, 600 ) );
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
	for( std::vector<Entity*>::iterator it = mEntities.begin(); it != mEntities.end(); it++ )
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
		for( std::vector<Entity*>::iterator it = mEntities.begin(); it != mEntities.end(); it++ )
		{
			( *it )->handleEvent( event );
		}
	}
}
