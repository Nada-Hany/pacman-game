#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<sstream>
#include "functions.h"
#include <map>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace sf;

//map
#define ll long long
#define NUMBERCOLUMNS 20
#define NUMBERROW 22
#define TILESIZE 40
#define offset_x 580
#define offset_y 100
//pacman
#define player_width 38
#define player_height 38
const int diff = ((TILESIZE - player_width) / 2);
#define baseSpeed 2
//ghost 
#define speedInPowerUp 1
#define ghostSpeed 4


enum class direction {
	up, right, left, down
};
//map
enum class tile_type
{
	wall, score, powerup, pacman, ghosts, none, cherry
	//Wall=0 ,Point=1 ,Space=2 ,powerup=3 ,Ghosts=4 ,Pacmam=5 ,cherry=6
};

// our small data base
string username;
map<string, int> Users;
void SaveHighScores();
void SaveNewScore(int score);
void LoadHighScores();
void UsernameWindow(RenderWindow& window);
template <size_t ROW, size_t COL>
void LoadEasyMap(int (&map)[ROW][COL]);

int changing_map[NUMBERROW][NUMBERCOLUMNS];

struct tile
{
	Sprite cherry;
	CircleShape cipoint;
	CircleShape cpowerup;
	RectangleShape recwall;
	tile_type type;
	int columns;
	int rows;

};
tile map_[NUMBERROW][NUMBERCOLUMNS];

//pacman
struct PACMAN {

	Sprite sprite;
	Texture deadPac_texture;
	Texture alivePac_texture;
	direction direction;
	int moving_direction = -1;
	int keyPressed = -1;
	int animation_alive = 0;
	int animetion_dead = 0;
	int score = 0;
	float delay = 0.2f; // deathPAC
	bool isAlive = true;
	bool deathSound = false;
	bool scoreSound = false;
	bool powerBallSound = false;
	bool cherrySound = false;
	bool ateGhostSound = false;
	bool powerBallBool = false;

}pacman;
void enumDirectionPACMAN(PACMAN& pacman) {
	switch (pacman.moving_direction)
	{
	case 0:
		pacman.direction = direction::right;
	case 1:
		pacman.direction = direction::up;
	case 2:
		pacman.direction = direction::left;
	case 3:
		pacman.direction = direction::down;
	default:
		break;
	}
}

//ghost
struct Ghosts
{
	int moving_direction = -1;
	direction direction;
	// 0 right , 1 up , 2 left , 3 down
	int frames;
	int animation;
	// 0 normal , 1 power up , 2 face , 3 end time
	Sprite sprite;
}ghosts[4];
// 0 red , 1 pink , 2 orange , 3 blue 

Texture red_ghost_texture;
Texture pink_ghost_texture;
Texture orange_ghost_texture;
Texture blue_ghost_texture;
Texture powerup_ghost_texture;
Texture face_ghost_texture;
Texture endtime_ghost_texture;
//cherry
Texture cherryTexture;

void enumDirectionGHOST(Ghosts& ghost) {
	switch (ghost.moving_direction)
	{
	case 0:
		ghost.direction = direction::right;
	case 1:
		ghost.direction = direction::up;
	case 2:
		ghost.direction = direction::left;
	case 3:
		ghost.direction = direction::down;
	default:
		break;
	}
}

int selected(Text mainmenu[3], RenderWindow& window);
void selected2(Text mainmenu[3], RenderWindow& window);

void mainmenu(RenderWindow& window);
void play(RenderWindow& window);
void mainmenu2(RenderWindow& window);
void Easy(RenderWindow& window);
void originalwindow(RenderWindow& window);
void pause(RenderWindow& window);
void introduction_window(RenderWindow& window);

//funcs
void get_tile_cor(float x, float y, int& row, int& col) {
	x = x - offset_x, y = y - offset_y;
	col = (int)x / TILESIZE;
	row = (int)y / TILESIZE;
}

bool same_tile_horz(Sprite& sprite) {
	bool condition_1 = false;
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	int row_1, row_2, col_1, col_2;
	get_tile_cor(x + (player_width / 2), y - (player_height / 2), row_1, col_1);
	get_tile_cor(x + (player_width / 2), y + (player_height / 2), row_2, col_2);

	if (row_1 == row_2 && col_1 == col_2)
		condition_1 = true;

	return condition_1;
}
bool same_tile_vert(Sprite& sprite) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false;
	int row_1, row_2, col_1, col_2;
	get_tile_cor(x - (player_width / 2), y - (player_height / 2), row_1, col_1);
	get_tile_cor(x + (player_width / 2), y - (player_height / 2), row_2, col_2);
	if (row_1 == row_2 && col_1 == col_2)
		condition_1 = true;

	return condition_1;
}

void move_right(Sprite& sprite, int& lastKeyPressed) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_horz(sprite);

	int row, col;

	get_tile_cor(x + (player_width / 2) + baseSpeed + 0.0001, y, row, col);
	if (map_[row][col].type == tile_type::wall) {
		if ((col * TILESIZE + offset_x) - (x + (player_width / 2) + diff) > 0) {

			if (condition_1 && condition_2)
			{
				sprite.move((col * TILESIZE + offset_x) - (x - (player_width / 2)), 0);
				lastKeyPressed = 0;
			}
		}
		else {
			condition_2 = false;
		}
	}
	else {
		if (condition_1 && condition_2)
		{
			sprite.move(baseSpeed, 0);
			lastKeyPressed = 0;
		}
	}
}
void move_left(Sprite& sprite, int& moving_direction) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_horz(sprite);

	int row, col;


	get_tile_cor(x - ((player_width / 2)) - baseSpeed - 0.001, y, row, col);

	if (map_[row][col].type == tile_type::wall)
	{
		if ((x - (player_width / 2) - diff) - ((col * TILESIZE) + TILESIZE + offset_x) > 0)
		{
			sprite.move(-((x - (player_width / 2)) - ((col * TILESIZE) + TILESIZE + offset_x)), 0);
			moving_direction = 2;
		}
		else
		{
			condition_2 = false;
		}
	}

	else {
		if (condition_1 && condition_2) {
			sprite.move(-baseSpeed, 0);
			moving_direction = 2;
		}
	}
}
void move_up(Sprite& sprite, int& moving_direction) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_vert(sprite);

	int row, col;

	get_tile_cor(x, y - baseSpeed - (player_height / 2) - 0.001, row, col);

	if (map_[row][col].type == tile_type::wall) {
		if (x, (y - (player_height / 2) - diff) - (row * TILESIZE + TILESIZE + offset_y) > 0) {
			sprite.move(0, -((y - (player_height / 2) - diff) - (row * TILESIZE + TILESIZE + offset_y)));
			moving_direction = 1;
		}
		else
			condition_2 = false;
	}
	else
		if (condition_1 && condition_2)
			sprite.move(0, -baseSpeed), moving_direction = 1;
}
void move_down(Sprite& sprite, int& moving_direction) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_vert(sprite);

	int row, col;

	get_tile_cor(x, y + baseSpeed + (player_height / 2) + 0.001, row, col);
	if (map_[row][col].type == tile_type::wall) {

		if ((row * TILESIZE + offset_y) - (y + (player_height / 2) + diff) > 0)
		{
			sprite.move(0, (row * TILESIZE + offset_y) - (y + (player_height / 2) + diff));
			moving_direction = 3;
		}
		else
			condition_2 = false;
	}
	else
		if (condition_1 && condition_2) sprite.move(0, baseSpeed), moving_direction = 3;
}
void change_direction(Sprite& sprite, int& keyPressed, int& moving_direction, int row, int col) {

	//right
	if (keyPressed == 0) {
		if (same_tile_horz(sprite))
		{
			if (map_[row][col + 1].type != tile_type::wall)
				moving_direction = keyPressed;
		}
	}
	//left
	if (keyPressed == 2) {
		if (same_tile_horz(sprite))
		{
			if (map_[row][col - 1].type != tile_type::wall)
				moving_direction = keyPressed;
		}
	}
	//up
	if (keyPressed == 1) {
		if (same_tile_vert(sprite))
		{
			if (map_[row - 1][col].type != tile_type::wall)
				moving_direction = keyPressed;
		}
	}
	//down
	if (keyPressed == 3) {
		if (same_tile_vert(sprite))
		{
			if (map_[row + 1][col].type != tile_type::wall)
				moving_direction = keyPressed;
		}
	}
}

int num = 0, num2 = 0;
bool isPaused = false;

//small ghosts of mainmenu
Texture redghost[4];

int main() {

	RenderWindow window(VideoMode(1920, 1080), "Main Menu", Style::Fullscreen);

	//pacman
	pacman.alivePac_texture.loadFromFile("pngs/alive pacman2-20.png");
	pacman.deadPac_texture.loadFromFile("pngs/dead pacman.png");
	pacman.sprite.setTexture(pacman.alivePac_texture);
	pacman.sprite.setOrigin((player_width / 2), (player_height / 2));
	pacman.sprite.setPosition(9 * TILESIZE + TILESIZE / 2 + offset_x, 15 * TILESIZE + TILESIZE / 2 + offset_y);
	pacman.sprite.setTextureRect(IntRect(0, 0, player_width, player_height)); //x y w h

	//ghosts
	red_ghost_texture.loadFromFile("pngs/red ghost sprite.png");
	pink_ghost_texture.loadFromFile("pngs/pink ghost sprite.png");
	orange_ghost_texture.loadFromFile("pngs/orange ghost sprite.png");
	blue_ghost_texture.loadFromFile("pngs/cyan ghost sprite.png");
	powerup_ghost_texture.loadFromFile("pngs/ghosts in power up mode.png");
	face_ghost_texture.loadFromFile("pngs/Ghost eyes.png");
	endtime_ghost_texture.loadFromFile("sounds/blue and grey ghosts.png");
	//cherry 
	cherryTexture.loadFromFile("pngs/cherry.png");

	Image icon;
	icon.loadFromFile("pngs/cherry.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	window.setFramerateLimit(50);
	while (window.isOpen()) {
		if (num == 0) {
			introduction_window(window);
		}
		if (num == 1) {
			play(window);
		}
	}
	return 0;
}

//mainmenu -->>> play settings exit
void mainmenu(RenderWindow& window) {

	//prepare the sound
	//select sound
	SoundBuffer select;
	select.loadFromFile("sounds/select sound.wav");
	Sound soundselect;
	soundselect.setBuffer(select);

	//click sound
	SoundBuffer click;
	click.loadFromFile("sounds/enter sound.wav");
	Sound soundclick;
	soundclick.setBuffer(click);

	//background
	Texture backtextmain;
	backtextmain.loadFromFile("pngs/background.jpg");
	Sprite spriteback;
	spriteback.setTexture(backtextmain);
	FloatRect backrect = spriteback.getLocalBounds();
	spriteback.setOrigin(backrect.left + backrect.width / 2.0f, backrect.top + backrect.height / 2.0f);
	spriteback.setPosition(Vector2f(1920 / 2, 1080 / 2));

	//small ghsots
	redghost[3].loadFromFile("pngs/red ghost.png");
	Sprite spriteredghost3;
	spriteredghost3.setTexture(redghost[3]);
	FloatRect redghostrect3 = spriteredghost3.getLocalBounds();
	spriteredghost3.setOrigin(redghostrect3.left + redghostrect3.width / 2.0f, redghostrect3.top + redghostrect3.height / 2.0f);
	spriteredghost3.setPosition(Vector2f(830, 840));

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");

	//creating an array to put (play,settings,exit)
	Text mainmenu[3];

	mainmenu[0].setFont(font);
	mainmenu[1].setFont(font);
	mainmenu[2].setFont(font);

	mainmenu[0].setFillColor(Color::White);
	mainmenu[1].setFillColor(Color::White);
	mainmenu[2].setFillColor(Color::White);

	mainmenu[0].setString("play");
	mainmenu[1].setString("high score");
	mainmenu[2].setString("exit");

	mainmenu[0].setCharacterSize(50);
	mainmenu[1].setCharacterSize(50);
	mainmenu[2].setCharacterSize(50);

	FloatRect textrect = mainmenu[0].getLocalBounds();
	FloatRect textrect1 = mainmenu[1].getLocalBounds();
	FloatRect textrect2 = mainmenu[2].getLocalBounds();

	mainmenu[0].setOrigin(textrect.left + textrect.width / 2.0f, textrect.top + textrect.height / 2.0f);
	mainmenu[1].setOrigin(textrect1.left + textrect1.width / 2.0f, textrect1.top + textrect1.height / 2.0f);
	mainmenu[2].setOrigin(textrect2.left + textrect2.width / 2.0f, textrect2.top + textrect2.height / 2.0f);

	mainmenu[0].setPosition(Vector2f(1920 / 2, 680));
	mainmenu[1].setPosition(Vector2f(1920 / 2, 760));
	mainmenu[2].setPosition(Vector2f(1920 / 2, 840));

	bool sound = 0;
	bool sound2 = 0;
	bool sound4 = 0;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				break;
			}
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Escape) {
					soundclick.play();
					window.close();
					return;
				}
			}
			//to make the sound of button
			else  if (event.type == Event::MouseMoved) {
				// Check if the mouse is over the button
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (mainmenu[2].getGlobalBounds().contains(mousePos)) {
					if (!sound4) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound4 = true;
				}
				else {
					sound4 = false;
				}
				if (mainmenu[0].getGlobalBounds().contains(mousePos)) {
					if (!sound) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound = true;
				}
				else {
					sound = false;
				}
				if (mainmenu[1].getGlobalBounds().contains(mousePos)) {
					if (!sound2) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound2 = true;
				}
				else {
					sound2 = false;
				}
			}
			else if (event.type == Event::MouseButtonPressed) {
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (mainmenu[0].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					//open mainmenu
					mainmenu2(window);
				}
				if (mainmenu[1].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					//open high score menu
					mainmenu2(window);
				}
				if (mainmenu[2].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					window.close();
				}
			}
		}
		//to know the number of the function that we will use in main
		num = selected(mainmenu, window);

		if (num != 0) {
			return;
		}
		window.clear();
		window.draw(spriteback);
		for (int i = 0; i < 3; i++) {
			window.draw(mainmenu[i]);
		}
		selected2(mainmenu, window);
		//when pressing exit
		Mouse mouse;
		//to make small ghost near the word
		bool smallghost = 0;
		bool sound = 0;
		if (mainmenu[2].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {
			sound = 1;
			smallghost = 1;
			mainmenu[2].setFillColor(Color::Red);
			window.draw(spriteredghost3);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				mainmenu[2].setFillColor(Color::White);
				window.close();
				soundclick.play();
				sound = 0;

			}
		}
		else {
			mainmenu[2].setFillColor(Color::White);
		}
		window.display();

	}
}

//2nd mainmenu includes -->>> easy medium hard
void mainmenu2(RenderWindow& window) {

	//select sound
	SoundBuffer select;
	select.loadFromFile("sounds/select sound.wav");
	Sound soundselect;
	soundselect.setBuffer(select);

	//click sound
	SoundBuffer click;
	click.loadFromFile("sounds/enter sound.wav");
	Sound soundclick;
	soundclick.setBuffer(click);

	//background
	Texture backtextmain;
	backtextmain.loadFromFile("pngs/background.jpg");
	Sprite spriteback;
	spriteback.setTexture(backtextmain);
	FloatRect backrect = spriteback.getLocalBounds();
	spriteback.setOrigin(backrect.left + backrect.width / 2.0f, backrect.top + backrect.height / 2.0f);
	spriteback.setPosition(Vector2f(1920 / 2, 1080 / 2));

	RectangleShape rectangle(Vector2f(400, 450));
	rectangle.setOrigin(Vector2f(200, 225));
	rectangle.setPosition(Vector2f(1920 / 2, 1080 / 2));

	rectangle.setFillColor(Color::Blue);
	rectangle.setOutlineThickness(30);
	rectangle.setOutlineColor(Color::White);

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");

	Text mainmenu2[3];

	mainmenu2[0].setFont(font);
	mainmenu2[1].setFont(font);
	mainmenu2[2].setFont(font);


	mainmenu2[0].setFillColor(Color::White);
	mainmenu2[1].setFillColor(Color::White);
	mainmenu2[2].setFillColor(Color::White);

	mainmenu2[0].setString("easy");
	mainmenu2[1].setString("Medium");
	mainmenu2[2].setString("Hard");

	mainmenu2[0].setCharacterSize(50);
	mainmenu2[1].setCharacterSize(50);
	mainmenu2[2].setCharacterSize(50);

	FloatRect textrect = mainmenu2[0].getLocalBounds();
	FloatRect textrect1 = mainmenu2[1].getLocalBounds();
	FloatRect textrect2 = mainmenu2[2].getLocalBounds();

	mainmenu2[0].setOrigin(textrect.left + textrect.width / 2.0f, textrect.top + textrect.height / 2.0f);
	mainmenu2[1].setOrigin(textrect1.left + textrect1.width / 2.0f, textrect1.top + textrect1.height / 2.0f);
	mainmenu2[2].setOrigin(textrect2.left + textrect2.width / 2.0f, textrect2.top + textrect2.height / 2.0f);


	mainmenu2[0].setPosition(Vector2f(1920 / 2, 700));
	mainmenu2[1].setPosition(Vector2f(1920 / 2, 800));
	mainmenu2[2].setPosition(Vector2f(1920 / 2, 900));

	bool sound = 0, sound2 = 0, sound3 = 0;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				break;
			}
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Escape) {
					soundclick.play();
					mainmenu(window);
				}
			}
			else if (event.type == Event::MouseButtonPressed) {
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (mainmenu2[0].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					LoadEasyMap(changing_map);
					originalwindow(window);
				}
			}
			else if (event.type == Event::MouseMoved) {
				// Check if the mouse is over the button
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				if (mainmenu2[0].getGlobalBounds().contains(mousePos)) {
					if (!sound) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound = true;
				}
				else {
					sound = false;
				}

				if (mainmenu2[1].getGlobalBounds().contains(mousePos)) {
					if (!sound2) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound2 = true;
				}
				else {
					sound2 = false;
				}


				if (mainmenu2[2].getGlobalBounds().contains(mousePos)) {
					if (!sound3) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound3 = true;
				}
				else {
					sound3 = false;
				}
			}
		}
		//to know the number of the function that we will use in main
		num2 = selected(mainmenu2, window);

		if (num2 != 0) {
			return;
		}
		window.clear();
		window.draw(spriteback);
		for (int i = 0; i < 3; i++) {
			window.draw(mainmenu2[i]);
		}

		//	put window of score  when pressing easy ONLY
		Mouse mouse;
		if (mainmenu2[0].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			mainmenu2[0].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				mainmenu2[0].setFillColor(Color::White);
				soundclick.play();
				Easy(window);
			}
		}
		else {
			mainmenu2[0].setFillColor(Color::White);
		}

		//	put window of score  when pressing medium ONLY
		if (mainmenu2[1].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			mainmenu2[1].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				mainmenu2[1].setFillColor(Color::White);
				//function of medium and call inside it originalwindow function
			}
		}
		else {
			mainmenu2[1].setFillColor(Color::White);
		}
		//	put window of score  when pressing hard ONLY
		; if (mainmenu2[2].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			mainmenu2[2].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				mainmenu2[2].setFillColor(Color::White);
				//function of hard and call inside it originalwindow function

			}
		}
		else {
			mainmenu2[2].setFillColor(Color::White);
		}
		window.display();
	}
}

//change color of button when selected
int selected(Text arr[3], RenderWindow& window) {

	Mouse mouse;
	for (int i = 0; i < 3; i++) {
		if (arr[i].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			arr[i].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				arr[i].setFillColor(Color::White);
				return i + 1;
			}
		}

		else {
			arr[i].setFillColor(Color::White);
		}
	}
	return 0;

}

//window of (play) then EASY MEDIUM HARD
void play(RenderWindow& window) {

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				break;
			}
		}
		window.clear();
		mainmenu2(window);
		window.display();
	}

}

//easy button then original window of the game
void Easy(RenderWindow& window) {

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				break;
			}
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Escape) {
					num = 0;
					return;
				}
			}
		}
		window.clear();
		originalwindow(window);
		window.display();
	}
}

//original window of the game includes string/map.....rest of the game
void originalwindow(RenderWindow& window) {

	//prepare the sound
	//select sound
	SoundBuffer select;
	select.loadFromFile("sounds/select sound.wav");
	Sound soundselect;
	soundselect.setBuffer(select);

	//click sound
	SoundBuffer click;
	click.loadFromFile("sounds/enter sound.wav");
	Sound soundclick;
	soundclick.setBuffer(click);

	//game sound
	SoundBuffer gamesound;
	gamesound.loadFromFile("sounds/Pacman_Introduction_Music-KP-826387403(1).wav");
	Sound gameS;
	gameS.setBuffer(gamesound);

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");
	Text s;
	Text timer;

	//circle of pause
	CircleShape circle(50, 50);
	circle.setOrigin(Vector2f(50, 50));
	circle.setPosition(Vector2f(1800, 70));
	circle.setFillColor(Color::Black);
	circle.setOutlineThickness(5);
	circle.setOutlineColor(Color::White);

	//two lines oooof pause
	RectangleShape line1(Vector2f(10, 50));
	line1.setOrigin(Vector2f(50, 50));
	line1.setPosition(Vector2f(1855, 95));
	line1.setFillColor(Color::White);

	RectangleShape line2(Vector2f(10, 50));
	line2.setOrigin(Vector2f(50, 50));
	line2.setPosition(Vector2f(1835, 95));
	line2.setFillColor(Color::White);

	//score text
	s.setFont(font);
	s.setOrigin(Vector2f(50, 50));
	s.setCharacterSize(90);
	s.setFillColor(Color::White);
	s.setPosition(55, 70);

	//time 
	timer.setFont(font);
	timer.setCharacterSize(70);
	timer.setFillColor(Color::White);
	timer.setPosition(50, 120);

	//cherry
	Sprite cherrySprite;
	cherrySprite.setTexture(cherryTexture);
	cherrySprite.setOrigin(player_width / 2, player_height / 2);
	cherrySprite.setPosition(9 * TILESIZE + TILESIZE / 2 + offset_x, 3 * TILESIZE + TILESIZE / 2 + offset_y);
	bool cherry = false;

	bool hundredshow = false;
	Text hundred;
	hundred.setFont(font);
	hundred.setString("100");
	hundred.setCharacterSize(8);
	hundred.setFillColor(Color::Yellow);
	hundred.setPosition(130, 130);
	hundred.setOrigin((player_width / 2), (player_height / 2));

	//hole
	RectangleShape rect_right;
	rect_right.setSize(Vector2f(50, 50));
	rect_right.setFillColor(Color::Black);
	rect_right.setPosition(offset_x + TILESIZE * NUMBERCOLUMNS - TILESIZE, offset_y + 11 * TILESIZE);
	RectangleShape rect_left;
	rect_left.setSize(Vector2f(50, 50));
	rect_left.setFillColor(Color::Black);
	rect_left.setPosition(offset_x - rect_left.getSize().x, offset_y + 11 * TILESIZE);

	//pacman
	bool sound = 0, sound2 = 0;
	bool gamess = 1;
	int timer_sec = 0, timer_min = 0;

	gameS.play();
	Time resettime = seconds(4.0f);

	while (window.isOpen()) {

		for (int i = 0; i < NUMBERROW; i++)
		{
			for (int j = 0; j < NUMBERCOLUMNS; j++)
			{

				if (changing_map[i][j] == 2)
				{
					map_[i][j].type = tile_type::none;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setPosition(j * TILESIZE + offset_x, i * TILESIZE + offset_y);
					map_[i][j].recwall.setFillColor(Color::Black);
				}
				if (changing_map[i][j] == 1)
				{
					map_[i][j].type = tile_type::score; ;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setPosition(j * TILESIZE + offset_x, i * TILESIZE + offset_y);
					map_[i][j].recwall.setFillColor(Color::Black);

					map_[i][j].cipoint.setRadius(3.0f);
					map_[i][j].cipoint.setOrigin(3, 3);
					map_[i][j].cipoint.setPosition((j * TILESIZE + TILESIZE / 2) + offset_x, (i * TILESIZE + TILESIZE / 2) + offset_y);
					map_[i][j].cipoint.setFillColor(Color{ 255, 250, 165 });
				}
				if (changing_map[i][j] == 0)
				{
					map_[i][j].type = tile_type::wall;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setPosition(j * TILESIZE + offset_x, i * TILESIZE + offset_y);
					map_[i][j].recwall.setFillColor(Color{ 36, 31, 201 });
				}
				if (changing_map[i][j] == 3)
				{
					map_[i][j].type = tile_type::powerup;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setPosition(j * TILESIZE + offset_x, i * TILESIZE + offset_y);
					map_[i][j].recwall.setFillColor(Color::Black);

					map_[i][j].cpowerup.setRadius(10.0f);
					map_[i][j].cpowerup.setOrigin(10, 10);
					map_[i][j].cpowerup.setPosition((j * TILESIZE + TILESIZE / 2) + offset_x, (i * TILESIZE + TILESIZE / 2) + offset_y);
					map_[i][j].cpowerup.setFillColor(Color{ 223, 217, 130 });

				}
				map_[i][j].rows = i;
				map_[i][j].columns = j;
			}

		}

		float elapsedTime_cherry = 0;

		SoundBuffer eat;
		eat.loadFromFile("sounds/eatDots.wav");
		Sound eatsound(eat);

		SoundBuffer eatCherry;
		eatCherry.loadFromFile("sounds/pacman_eatfruit.wav");
		Sound eatcherrysound(eatCherry);

		SoundBuffer death;
		death.loadFromFile("sounds/pacman death.wav");
		Sound deathSound(death);

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				break;
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				pacman.keyPressed = 0;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Left)) {
				pacman.keyPressed = 2;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Up)) {
				pacman.keyPressed = 1;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Down)) {
				pacman.keyPressed = 3;
			}
			if (event.type == Event::KeyReleased) {

				if (event.key.code == Keyboard::Escape) {
					soundclick.play();
					isPaused = true;
					gameS.stop();
					pause(window);
					return;
				}
			}

			//to make the sound of button
			else  if (event.type == Event::MouseMoved) {
				// Check if the mouse is over the button
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (line1.getGlobalBounds().contains(mousePos)) {
					if (!sound) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}

					sound = true;
				}
				else
				{
					sound = false;
				}


				if (line2.getGlobalBounds().contains(mousePos))
				{
					if (!sound2)
					{
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound2 = true;
				}
				else
				{
					sound2 = false;
				}
			}

		}

		if (!isPaused)
		{
			//moving
			float x_pac = pacman.sprite.getPosition().x, y_pac = pacman.sprite.getPosition().y;
			int row_pac, col_pac;
			get_tile_cor(x_pac, y_pac, row_pac, col_pac);
			change_direction(pacman.sprite, pacman.keyPressed, pacman.moving_direction, row_pac, col_pac);

			if (pacman.moving_direction == 0 && pacman.isAlive) {

				move_right(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(1, 1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, 0, player_width, player_height));
				pacman.animation_alive %= 9;
			}
			else if (pacman.moving_direction == 2 && pacman.isAlive) {
				move_left(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(-1, 1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, 0, player_width, player_height));
				pacman.animation_alive %= 9;


			}
			else if (pacman.moving_direction == 1 && pacman.isAlive) {
				move_up(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(1, 1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, player_height, player_width, player_height));
				pacman.animation_alive %= 9;

			}
			else if (pacman.moving_direction == 3 && pacman.isAlive) {
				move_down(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(1, -1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, player_height, player_width, player_height));
				pacman.animation_alive %= 9;

			}

			//eat dots
			if (map_[row_pac][col_pac].type == tile_type::score) {
				if (map_[row_pac][col_pac].cipoint.getGlobalBounds().contains(pacman.sprite.getPosition().x, pacman.sprite.getPosition().y))
				{
					changing_map[row_pac][col_pac] = 2;
					pacman.score++;
					//eatsound.play();
				}
			}
			//eat powerBall
			if (map_[row_pac][col_pac].type == tile_type::powerup) {
				if (map_[row_pac][col_pac].cpowerup.getGlobalBounds().contains(pacman.sprite.getPosition().x, pacman.sprite.getPosition().y)) {
					changing_map[row_pac][col_pac] = 2;
					pacman.powerBallBool = true; // for mai ghost
					for (int i = 0; i < 4; i++)
						ghosts[i].animation = 1;
				}
			}

			if (pacman.powerBallBool)
			{
				// check if pacman ate a ghost    // check timer ??

				for (int i = 0; i < 4; i++)
				{
					if (ghosts[i].animation == 1)
					{
						if (pacman.sprite.getGlobalBounds().intersects(ghosts[i].sprite.getGlobalBounds()))
							ghosts[i].animation = 2;
					}
				}
			}
			//eat cherry

			if (pacman.sprite.getGlobalBounds().intersects(cherrySprite.getGlobalBounds()) && cherry == true) {
				cherry = false;
				eatcherrysound.play();
				pacman.score += 100;
				hundredshow = true;
			}

			Clock clock;

			//cherry
			elapsedTime_cherry += clock.restart().asSeconds();
			if (elapsedTime_cherry > 10 && hundredshow == false && pacman.isAlive) {
				cherry = true;
			}
			if (elapsedTime_cherry > 20) {
				cherry = false;
				hundredshow = false;
			}
			//eat cherry
			if (pacman.sprite.getGlobalBounds().intersects(cherrySprite.getGlobalBounds()) && cherry) {
				cherry = false;
				eatcherrysound.play();
				pacman.score += 100;
				hundredshow = true;
			}

			//update score text
			stringstream score_manip;
			score_manip << "score:" << pacman.score;
			s.setString(score_manip.str());

			Clock play_clock;
			char x = ':';

			if (play_clock.getElapsedTime().asSeconds() >= 1) {
				timer_sec++;
				play_clock.restart();
				if (timer_sec % 60 == 0) {
					timer_sec = 0;
					timer_min++;
				}
			}
			stringstream time_manip;
			time_manip << "time " << timer_min << x << timer_sec;
			timer.setString(time_manip.str());

			if (pacman.isAlive == false && pacman.deathSound == false) {
				deathSound.play();
				pacman.deathSound = true;
			}

			//hole 
			int left_hole = offset_x, right_hole = offset_x + (NUMBERCOLUMNS * TILESIZE) - TILESIZE;

			if (x_pac + TILESIZE / 2 <= left_hole - 1 && pacman.moving_direction == 2) {
				pacman.sprite.setPosition(right_hole + TILESIZE / 2, y_pac);
			}
			if (x_pac - player_width / 2 >= right_hole && pacman.moving_direction == 0) {
				pacman.sprite.setPosition(left_hole - TILESIZE / 2, y_pac);
			}


			window.clear();

			for (int i = 0; i < NUMBERROW; i++)
			{
				for (int j = 0; j < NUMBERCOLUMNS; j++)
				{
					window.draw(map_[i][j].recwall);

					if (map_[i][j].type == tile_type::score)
						window.draw(map_[i][j].cipoint);
					if (map_[i][j].type == tile_type::powerup)
						window.draw(map_[i][j].cpowerup);
				}
			}

			window.draw(s);
			window.draw(timer);
			window.draw(circle);
			window.draw(line1);
			window.draw(line2);

			Mouse mouse;

			if (circle.getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

				line1.setFillColor(Color::Red);
				line2.setFillColor(Color::Red);

				if (Mouse::isButtonPressed(Mouse::Left)) {
					gameS.stop();
					line1.setFillColor(Color::White);
					line2.setFillColor(Color::White);

					soundclick.play();
					pause(window);
				}
			}
			else {
				line1.setFillColor(Color::White);
				line2.setFillColor(Color::White);
			}

			//pacman 
			if (cherry)
				window.draw(cherrySprite);

			window.draw(pacman.sprite);

			if (hundredshow)
				window.draw(hundred);

			if (!pacman.isAlive)
				sf::sleep(sf::seconds(pacman.delay));

			window.draw(rect_right);
			window.draw(rect_left);
			window.display();
		}
	}
}

//to pause in the middle of the game
void pause(RenderWindow& window) {

	//prepare the sound
	//select sound
	SoundBuffer select;
	select.loadFromFile("select sound.wav");
	Sound soundselect;
	soundselect.setBuffer(select);

	//click sound
	SoundBuffer click;
	click.loadFromFile("enter sound.wav");
	Sound soundclick;
	soundclick.setBuffer(click);

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");

	Text menupause[2];

	menupause[0].setFont(font);
	menupause[1].setFont(font);

	menupause[0].setFillColor(Color::White);
	menupause[1].setFillColor(Color::White);

	menupause[0].setString("Continue");
	menupause[1].setString("exit");

	menupause[0].setCharacterSize(90);
	menupause[1].setCharacterSize(90);

	FloatRect textrect = menupause[0].getLocalBounds();
	FloatRect textrect1 = menupause[1].getLocalBounds();

	menupause[0].setOrigin(textrect.left + textrect.width / 2.0f, textrect.top + textrect.height / 2.0f);
	menupause[1].setOrigin(textrect1.left + textrect1.width / 2.0f, textrect1.top + textrect1.height / 2.0f);

	menupause[0].setPosition(Vector2f(1920 / 2, 455));
	menupause[1].setPosition(Vector2f(1920 / 2, 625));

	bool sound = 0, sound2 = 0;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {

			if (event.type == Event::Closed) {
				window.close();
				break;
			}
			//exit
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Escape) {
					soundclick.play();
					bool isPaused = false;
					//NOT HERE BUT WE PUT HERE PAUSE WINDOW.
					mainmenu(window);
					return;
				}
			}
			//to make the sound of button
			else  if (event.type == Event::MouseMoved)
			{
				// Check if the mouse is over the button
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (menupause[0].getGlobalBounds().contains(mousePos)) {
					if (!sound) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound = true;
				}
				else
				{
					sound = false;
				}

				if (menupause[1].getGlobalBounds().contains(mousePos)) {
					if (!sound2) {
						// Play the sound if the mouse just entered the button
						bool isPaused = false;
						soundselect.play();
					}
					sound2 = true;
				}
				else {
					sound2 = false;
				}

			}
		}
		window.clear();
		for (int i = 0; i < 2; i++) {
			window.draw(menupause[i]);
		}
		Mouse mouse;
		if (menupause[0].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {
			menupause[0].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				menupause[0].setFillColor(Color::White);
				soundclick.play();
				originalwindow(window);
			}
		}
		else {
			menupause[0].setFillColor(Color::White);
		}

		if (menupause[1].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			menupause[1].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				menupause[1].setFillColor(Color::White);
				soundclick.play();
				mainmenu(window);
			}
		}
		else {
			menupause[1].setFillColor(Color::White);
		}

		window.display();

	}
}

//to put small ghosts
void selected2(Text arr2[3], RenderWindow& window) {

	//small ghosts
	redghost[0].loadFromFile("pngs/yellow ghost.png");
	Sprite spriteredghost;
	spriteredghost.setTexture(redghost[0]);
	FloatRect redghostrect = spriteredghost.getLocalBounds();
	spriteredghost.setOrigin(redghostrect.left + redghostrect.width / 2.0f, redghostrect.top + redghostrect.height / 2.0f);
	spriteredghost.setPosition(Vector2f(830, 680));

	redghost[2].loadFromFile("pngs/blue ghost.png");
	Sprite spriteredghost2;
	spriteredghost2.setTexture(redghost[2]);
	FloatRect redghostrect2 = spriteredghost2.getLocalBounds();
	spriteredghost2.setOrigin(redghostrect2.left + redghostrect2.width / 2.0f, redghostrect2.top + redghostrect2.height / 2.0f);
	spriteredghost2.setPosition(Vector2f(720, 760));

	Mouse mouse;
	bool smallghost = 0;
	for (int i = 0; i < 2; i++) {
		if (arr2[i].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			arr2[i].setFillColor(Color::Red);
			smallghost = 1;
			if (i == 0) {
				window.draw(spriteredghost);
			}
			if (i == 1) {
				window.draw(spriteredghost2);
			}
			if (Mouse::isButtonPressed(Mouse::Left)) {
				arr2[i].setFillColor(Color::White);
			}
			else {
				arr2[i].setFillColor(Color::White);
			}
		}
	}
}

// The transition in the beginning
void introduction_window(RenderWindow& window)
{
	Texture texturePAC_MAN;
	texturePAC_MAN.loadFromFile("graphics/PAC-MAN.png");
	Sprite spritePAC_MAN;
	spritePAC_MAN.setTexture(texturePAC_MAN);

	Texture texturePA_MAN;
	texturePA_MAN.loadFromFile("graphics/PA_-MAN.png");
	Sprite spritePA_MAN;
	spritePA_MAN.setTexture(texturePA_MAN);

	Texture textureC;
	textureC.loadFromFile("graphics/__C-___.png");
	Sprite spriteC;
	spriteC.setTexture(textureC);
	float C_x = (-132.0f - 57.0f) * 2;
	spriteC.setPosition(C_x, 0.0f);

	Texture textureCclosed;
	textureCclosed.loadFromFile("graphics/__C-___closed.png");
	Sprite spriteCclosed;
	spriteCclosed.setTexture(textureCclosed);
	float Cclosed_x = (-132.0f) * 2;
	spriteCclosed.setPosition(Cclosed_x, 0.0f);

	SoundBuffer buffer;
	buffer.loadFromFile("sounds/Pacman_Introduction_Music-KP-826387403(1).wav");
	Sound Soundpacman;
	Soundpacman.setBuffer(buffer);
	Soundpacman.setLoop(true);
	Soundpacman.play();

	Clock clock;
	Clock scaleClock;

	while (window.isOpen())
	{

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Key::Escape))
			{
				window.close();
			}
			else if (Keyboard::isKeyPressed(Keyboard::Key::Space)) {
				Soundpacman.pause();
				UsernameWindow(window);
			}
		}

		if (C_x > 731.0f - 132.0f and scaleClock.getElapsedTime().asSeconds() >= 6) {
			Soundpacman.pause();
			UsernameWindow(window);
		}

		if (clock.getElapsedTime().asSeconds() >= 1) {

			clock.restart();
			if (C_x < 731.0f - 132.0f) {
				C_x += 132.0f * 2;
			}
			if (Cclosed_x < (731.0f - 132.0f) * 2) {
				Cclosed_x += 132.0f * 2;
			}
			spriteC.setPosition(C_x, 0.0f);
			spriteCclosed.setPosition(Cclosed_x, 0.0f);
		}

		window.clear();

		// draw the scene
		if (clock.getElapsedTime().asSeconds() <= 0.5 && C_x < 731.0f - 132.0f)
			window.draw(spriteC);
		else if (clock.getElapsedTime().asSeconds() >= 0.5 && C_x < 731.0f - 132.0f)
			window.draw(spriteCclosed);
		else window.draw(spritePAC_MAN);

		window.display();

	}
}

// to Know who is playing our game and honoring him
void UsernameWindow(RenderWindow& window) {
	Texture usernameTexture;
	usernameTexture.loadFromFile("graphics/username.png");
	Sprite usernameSprite;
	usernameSprite.setTexture(usernameTexture);

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");

	Text text;
	text.setCharacterSize(50);
	text.setFont(font);
	text.setFillColor(Color(255, 255, 0));
	FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left, textRect.top + textRect.height / 2.0f);
	text.setPosition(960.0f, 450.0f);

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event)) {
			//please don't escape )':
			if (event.type == Event::Closed || event.key.code == Keyboard::Key::Escape) {
				// suppose to make ******************AreYouSure()***************
				window.close();
			}

			// save username
			if (Keyboard::isKeyPressed(Keyboard::Enter)) {
				mainmenu(window);
			}

			// enter your username
			if (event.type == Event::TextEntered) {
				if (event.text.unicode < 128) {
					if (event.text.unicode == 8 and username.size() > 0) {
						username.erase(username.size() - 1);
					}
					else if (!(event.text.unicode == 8 || event.text.unicode == Keyboard::Key::Escape)) {
						username += static_cast<char>(event.text.unicode);
					}
					text.setString(username);
					FloatRect textRect = text.getLocalBounds();
					text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
					text.setPosition(960.0f, 580.0f);
				}
			}
		}

		window.clear(sf::Color::White);

		window.draw(usernameSprite);
		window.draw(text);

		window.display();
	}
}

// Shut The Freak Up and don't play here it's not the game
void SaveNewScore(int score) {
	// Saving the new high score if you got it
	Users[username] = max(Users[username], score);
}

// Also here 
void LoadHighScores() {
	// Loading data
	ifstream takeInput("data/Your_Save_File.txt");
	ifstream takeScore("data/Your_Save_File2.txt");
	string user;
	int HighScore;

	while (takeInput >> user) {
		if (takeScore >> HighScore) {
			Users.insert({ user, HighScore });
		}
	}

	takeInput.close();
	takeScore.close();
}

// Yeah and here
void SaveHighScores() {
	// saving data
	ofstream saveUsernames("data/Your_Save_File.txt");
	ofstream saveScores("data/Your_Save_File2.txt");

	for (auto& pair : Users) {
		saveUsernames << pair.first;
		saveScores << pair.second;
	}

	saveUsernames.close();
	saveScores.close();
}

// Loading easy map
template <size_t ROW, size_t COL>
void LoadEasyMap(int (&map)[ROW][COL]) {
	// open the data file to load the map
	ifstream LoadMap("data/Easy_Map.txt");

	// adding the map to our game
	for (int i = 0; i < NUMBERROW; i++) {
		for (int j = 0; j < NUMBERCOLUMNS; j++) {
			LoadMap >> map[i][j];
		}
	}
	
	// closing the file
	LoadMap.close();
}