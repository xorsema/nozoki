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

#ifndef GAME_HPP
#define GAME_HPP

class Game;

//Base game state class
class GameState
{
public:
	GameState( Game * );

	int getDelta();
	virtual void handleInput() {}
	virtual void initState() {}
	virtual void doFrame() {}

protected:
	Game *mParent;
};

//State used for our actual game
class NozokiState : public GameState
{
public:
	NozokiState( Game * );
	virtual void handleInput();
	virtual void initState();
	virtual void doFrame();
	DungeonMap& getMap() { return mMap; }
	void spawnEnemies();

private:
	std::vector<Entity*>	mEntities;
	Player			mPlayer;
	sf::View		mView;
	DungeonMap		mMap;

};

//The game itself
class Game
{
public:
	Game();
	void doLoop();
	void setState( GameState *);
	int getDelta() { return mFrameTime; }

	sf::RenderWindow	*mWindow;

private:
	int		 mWindowWidth;
	int		 mWindowHeight;
	sf::Clock	 mDeltaClock;
	int		 mFrameTime;
	GameState	*mState;
	NozokiState	 mNozState;

	void openWindow();	
};

#endif
