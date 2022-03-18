#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES	1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<string>
#include<string.h>
#include <bangtal.h>
using namespace std;

SceneID scene;
ObjectID startbutton, restart, endbutton;
time_t time_start, time_end;
int time_d;
string time_s;
const char* time_f;

struct POS {
	int x, y;
};
const POS positions[16] = {
	{300, 470}, {450, 470}, {600, 470}, {750, 470},
	{300, 320}, {450, 320}, {600, 320}, {750, 320},
	{300, 170}, {450, 170}, {600, 170}, {750, 170},
	{300, 20}, {450, 20}, {600, 20}, {750, 20},
};

ObjectID initObjects[16];
ObjectID gameObjects[16];
int blank = 15;

TimerID timerMixing;
const int mixCount = 200;
const Second animationTime = 0.05f;
int mixing = 0;

bool game = false;

void game_init()
{
	scene = createScene("KOBE", "Images/bg.png");

	char image[20];
	for (int i = 0; i < 16; i++) {
		sprintf(image, "Images/%d.png", i + 1);
		initObjects[i] = createObject(image);
		gameObjects[i] = initObjects[i];

		locateObject(gameObjects[i], scene, positions[i].x, positions[i].y);
		showObject(gameObjects[i]);
	}

	startbutton = createObject("Images/start.png");
	locateObject(startbutton, scene, 540, 50);
	scaleObject(startbutton, 2);
	showObject(startbutton);

	restart = createObject("Images/restart.png");
	locateObject(restart, scene, 540, 50);
	scaleObject(restart, 2);

	endbutton = createObject("Images/end.png");
	locateObject(endbutton, scene, 595, 10);
	scaleObject(endbutton,1);

	blank = 15;
	game = false;

	srand((unsigned int)time(NULL));
	timerMixing = createTimer();
}

int game_index(ObjectID oid) {
	for (int i = 0; i < 16; i++) {
		if (gameObjects[i] == oid) return i;
	}

	return -1;
}

bool game_move(int index)
{
	if (index < 0) return false;
	if (index > 15) return false;

	if (index == (blank - 4) || index == (blank + 4) ||
		(index == (blank - 1) && (blank % 4) != 0) ||
		(index == (blank + 1) && (blank % 4) != 3)) {

		locateObject(gameObjects[index], scene, positions[blank].x, positions[blank].y);
		locateObject(gameObjects[blank], scene, positions[index].x, positions[index].y);

		ObjectID o = gameObjects[blank];
		gameObjects[blank] = gameObjects[index];
		gameObjects[index] = o;

		blank = index;

		return true;
	}

	return false;
}

void game_random_move()
{
	bool possible = false;
	int next = -1;
	while (!possible) {
		switch (rand() % 4) {
			case 0:		// UP
				next = blank - 4;
				break;
			case 1:		// DOWN
				next = blank + 4;
				break;
			case 2:		// LEFT
				next = (blank % 4 != 0) ? (blank - 1) : -1;
				break;
			case 3:		// RIGHT
				next = (blank % 4 != 3) ? (blank + 1) : -1;
				break;
		}

		possible = game_move(next);
	}
}

void game_prepare()
{
	hideObject(gameObjects[blank]);

	mixing = mixCount;

	// start to mix
	setTimer(timerMixing, animationTime);
	startTimer(timerMixing);
}

bool game_check()
{
	for (int i = 0; i < 16; i++) {
		if (gameObjects[i] != initObjects[i]) return false;
	}

	return true;
}

void game_end()
{
	game = false;

	showObject(gameObjects[blank]);
	showObject(restart);
	showObject(endbutton);
	//시간측정끝, 시간 보여주기
	time_end = time(NULL);

	int time_d = (time_end - time_start);

	time_s = std::to_string(time_d);
	time_f = time_s.c_str();
	showMessage(time_f);
	
	
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (game) {
		game_move(game_index(object));
		

		if (game_check()) {
			game_end();
		}
	}
	else if (mixing == 0 && object == startbutton) {
		game_prepare();
		//시간측정시작점
		time_start = time(NULL);
	}
	else if (object == restart) {
		hideObject(restart);
		hideObject(endbutton);
		showObject(startbutton);
	}
	else if (object == endbutton) {
		endGame();
	}
}

void timerCallback(TimerID timer)
{
	game_random_move();

	if (mixing > 0) {
		--mixing;

		setTimer(timerMixing, animationTime);
		startTimer(timerMixing);
	}
	else {
		hideObject(startbutton);
		game = true;
	}
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);

	game_init();

	startGame(scene);

	

	
}
