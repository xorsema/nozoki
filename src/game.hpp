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
