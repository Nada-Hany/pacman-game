#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<sstream>
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>

using namespace std;
using namespace sf;

//map
#define ll long long
#define NUMBERCOLUMNS 20
#define NUMBERROW 22
#define TILESIZE 40
#define HALF_TILESIZE TILESIZE / 2
#define offset_x 580
#define offset_y 100
//pacman
#define player_width 38
#define player_height 38
const int diff = ((TILESIZE - player_width) / 2);
#define baseSpeed 5
//ghost 
#define speedInPowerUp 2
#define ghostSpeed 4
#define ghost_width 38
#define ghost_height 38
#define ghosts_number 4

int changing_map[NUMBERROW][NUMBERCOLUMNS];

enum class direction {
	up, right, left, down
};
//map
enum class tile_type
{
	wall, score, powerup, pacman, ghosts, none, cherry
	//Wall=0 ,Point=1 ,Space=2 ,powerup=3 ,Ghosts=4 ,Pacmam=5 ,cherry=6
};

struct tile
{
	Sprite cherry;
	CircleShape cipoint;
	CircleShape cpowerup;
	RectangleShape recwall;
	tile_type type;
	int column;
	int row;
	struct tile* parent = NULL;

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
	int initial_x, initial_y;
	float delay = 0.2f; // deathPAC
	bool isAlive = true;
	bool cherry_taken = false;
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
	Sprite sprite;
	Sprite home_sprite;
	int moving_direction = -1;
	// 0 right , 1 up , 2 left , 3 down
	int frames;
	// 0 normal , 1 power up , 2 face , 3 end time
	int animation;
	int initial_x, initial_y;
	bool isDead = false;
	bool gotHome = true;
	bool outOfhome = false;
	bool isBFS = false;
	float speed = ghostSpeed;
	int frames_per_tile;

	vector<tile> shortest_path;
	vector<tile> home_path;
	vector<tile> corner_path;
	int shortest_path_index;
	int algo_window_BFS;
	int step_counts_BFS;
	int num_tiles_past_BFS;

	int step_counts_rand;

}ghosts[4];
// 0 red , 1 pink , 2 orange , 3 blue 

Texture red_ghost_texture;
Texture pink_ghost_texture;
Texture orange_ghost_texture;
Texture blue_ghost_texture;
Texture powerup_ghost_texture;
Texture face_ghost_texture;
Texture endtime_ghost_texture;

int selected(Text mainmenu[3], RenderWindow& window);
void selected2(Text mainmenu[3], RenderWindow& window);

void mainmenu(RenderWindow& window);
void play(RenderWindow& window);
void mainmenu2(RenderWindow& window);
void Easy(RenderWindow& window);
void originaleasywindow(RenderWindow& window);
void introduction_window(RenderWindow& window);
void Medium(RenderWindow& window);
void originalmediumwindow(RenderWindow& window);
void Hard(RenderWindow& window);
void originalhardwindow(RenderWindow& window);
void areyousure(RenderWindow& window);
void gameover(RenderWindow& window);

// our small data base
string username;
map<string, int> Users;
void SaveHighScores();
void SaveNewScore(int score);
void LoadHighScores();
void UsernameWindow(RenderWindow& window);
template <size_t ROW, size_t COL>
void LoadEasyMap(int(&map)[ROW][COL]);
template <size_t ROW2, size_t COL2>
void LoadmediumMap(int(&map)[ROW2][COL2]);
template <size_t ROW3, size_t COL3>
void LoadhardMap(int(&map)[ROW3][COL3]);
void LoadingWindow(RenderWindow& window);

void pause(RenderWindow& window, bool& pressed_pause, bool& newGame);

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

void move_right(Sprite& sprite, int& lastKeyPressed, float speed = baseSpeed) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_horz(sprite);

	int row, col;

	get_tile_cor(x + (player_width / 2) + speed + 0.0001, y, row, col);
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
			sprite.move(speed, 0);
			lastKeyPressed = 0;
		}
	}
}
void move_left(Sprite& sprite, int& moving_direction, float speed = baseSpeed) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_horz(sprite);

	int row, col;


	get_tile_cor(x - ((player_width / 2)) - speed - 0.001, y, row, col);

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
			sprite.move(-speed, 0);
			moving_direction = 2;
		}
	}
}
void move_up(Sprite& sprite, int& moving_direction, float speed = baseSpeed) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_vert(sprite);

	int row, col;

	get_tile_cor(x, y - speed - (player_height / 2) - 0.001, row, col);

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
			sprite.move(0, -speed), moving_direction = 1;
}
void move_down(Sprite& sprite, int& moving_direction , float speed = baseSpeed) {
	float y = sprite.getPosition().y, x = sprite.getPosition().x;
	bool condition_1 = false, condition_2 = true;

	condition_1 = same_tile_vert(sprite);

	int row, col;

	get_tile_cor(x, y + speed + (player_height / 2) + 0.001, row, col);
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
		if (condition_1 && condition_2) sprite.move(0, speed), moving_direction = 3;
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

void ghosts_animation(struct Ghosts ghosts[]);
bool check_wall(int& direction, Sprite& ghost);
void move_random(struct Ghosts ghosts[]);

void restart_pacman(PACMAN& pacman);
void restart_ghost(Ghosts& ghosts);

//BFS
bool exist_in_closed(tile* tile, vector <struct tile>& closed);	
void find_optimal_path(tile* current, tile* target, vector <tile>* get_path);
void catch_target(Ghosts& ghost, Sprite& target);
void random_direction(Sprite& sprite, int& direction);
void move_ghost (Ghosts& ghosts) {
	switch ((ghosts.moving_direction)) {
	case 0:
		move_right(ghosts.sprite, (ghosts.moving_direction), ghosts.speed);
		break;
	case 1:
		move_up(ghosts.sprite, (ghosts.moving_direction), ghosts.speed);
		break;
	case 2:
		move_left(ghosts.sprite, (ghosts.moving_direction), ghosts.speed);
		break;
	case 3:
		move_down(ghosts.sprite, (ghosts.moving_direction), ghosts.speed);
		break;
	}
}

int num = 3, num2 = 0;
bool newGame = true;
bool isPaused = false, sec3_timer = true;
int timer_sec = 0, timer_min = 0;
//1-> easy, 2-> medium, 3-> hard
int current_map = -1;

//small ghosts of mainmenu
Texture redghost[4];


int main() {

	red_ghost_texture.loadFromFile("pngs/red ghost sprite.png");
	pink_ghost_texture.loadFromFile("pngs/pink ghost sprite.png");
	orange_ghost_texture.loadFromFile("pngs/orange ghost sprite.png");
	blue_ghost_texture.loadFromFile("pngs/blue ghost sprite.png");
	powerup_ghost_texture.loadFromFile("pngs/ghosts in power up mode.png");
	face_ghost_texture.loadFromFile("pngs/eyes-26.png");
	endtime_ghost_texture.loadFromFile("pngs/blue and grey ghosts.png");

	RenderWindow window(VideoMode(1920, 1080), "Main Menu", Style::Fullscreen);
	window.setFramerateLimit(30);

	Image icon;
	icon.loadFromFile("pngs/cherry.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	while (window.isOpen()) {
		if (num == 3) {
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
				return;
			}
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Escape) {
					soundclick.play();
					window.close();
					return;
					//return;
				}
			}
			//to make the sound of button
			if (event.type == Event::MouseMoved) {
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
			// 0 -> play ,, 1 -> highscore ,, 2 -> exit
			if (event.type == Event::MouseButtonPressed) {
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (mainmenu[0].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					//open mainmenu
					mainmenu2(window);
					//window.close();


				}
				if (mainmenu[1].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					//open high score menu
					mainmenu2(window);
					//window.close();


				}
				if (mainmenu[2].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					//window.close();

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
				return;
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
			if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Escape) {
					soundclick.play();
					mainmenu(window);
				}
			}
			if (event.type == Event::MouseButtonPressed) {
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (mainmenu2[0].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					//alhassan
					LoadEasyMap(changing_map);
					Easy(window);
					LoadingWindow(window);
				}
			}

			if (event.type == Event::MouseButtonPressed) {
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (mainmenu2[1].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					LoadmediumMap(changing_map);
					Medium(window);
					LoadingWindow(window);
				}
			}

			if (event.type == Event::MouseButtonPressed) {
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				if (mainmenu2[2].getGlobalBounds().contains(mousePos)) {
					soundclick.play();
					LoadhardMap(changing_map);
					Hard(window);
					LoadingWindow(window);
				}
			}

			if (event.type == Event::MouseMoved) {
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
		if (newGame) {
			for (int i = 0; i < ghosts_number; i++)
				restart_ghost(ghosts[i]);
			restart_pacman(pacman);
		}
		window.clear();
		LoadingWindow(window);
		originaleasywindow(window);
		window.display();
	}
}

//original window of the game includes string/map.....rest of the game
void originaleasywindow(RenderWindow& window) {

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

	SoundBuffer eat;
	eat.loadFromFile("sounds/Pac-Man eat.wav");
	Sound eatsound(eat);

	SoundBuffer eatCherry;
	eatCherry.loadFromFile("sounds/pacman_eatfruit.wav");
	Sound eatcherrysound(eatCherry);

	SoundBuffer death;
	death.loadFromFile("sounds/pacman death.wav");
	Sound deathSound(death);

	SoundBuffer powerup;
	powerup.loadFromFile("sounds/PowerUP.wav");
	Sound powerUp_Sound(powerup);

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");
	Text s;
	Text timer, sec3_text;

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
	s.setCharacterSize(35);
	s.setFillColor(Color::White);
	s.setPosition(offset_x, offset_y - 40);


	//time 
	timer.setFont(font);
	timer.setCharacterSize(35);
	timer.setFillColor(Color::White);
	timer.setPosition(offset_x + 565, offset_y - 40);

	Texture background_texture;
	background_texture.loadFromFile("pngs/easy_background.png");
	Sprite background_sprite;
	background_sprite.setTexture(background_texture);

	//cherry
	Texture cherryTexture;
	cherryTexture.loadFromFile("pngs/cherry.png");
	Sprite cherrySprite;
	cherrySprite.setTexture(cherryTexture);
	cherrySprite.setOrigin(player_width / 2, player_height / 2);
	float cherry_x = 9 * TILESIZE + TILESIZE / 2 + offset_x;
	float cherry_y = 3 * TILESIZE + TILESIZE / 2 + offset_y;
	cherrySprite.setPosition(cherry_x, cherry_y);
	bool cherry = false;

	Font numberFont;
	numberFont.loadFromFile("fonts/Joystix.ttf");
	bool hundredshow = false;
	Text hundred;
	hundred.setFont(numberFont);
	hundred.setString("100");
	hundred.setCharacterSize(16);
	hundred.setFillColor(Color::Yellow);
	hundred.setPosition(cherry_x, cherry_y + 10);
	hundred.setOrigin((player_width / 2), (player_height / 2));

	sec3_text.setFont(font);
	sec3_text.setCharacterSize(200);

	//hole
	RectangleShape rect_right;
	rect_right.setSize(Vector2f(50, 50));
	rect_right.setFillColor(Color::Black);
	rect_right.setPosition(offset_x + TILESIZE * NUMBERCOLUMNS - TILESIZE, offset_y + 11 * TILESIZE);
	RectangleShape rect_left;
	rect_left.setSize(Vector2f(50, 50));
	rect_left.setFillColor(Color::Black);
	rect_left.setPosition(offset_x - rect_left.getSize().x, offset_y + 11 * TILESIZE);

	RectangleShape blackRect(Vector2f(1920.0f, 1080.0f));
	blackRect.setFillColor(Color{ 0,0,0,100 });

	//GHOSTS	
	// 0 red , 1 pink , 2 orange , 3 blue 

	ghosts[0].isBFS = true;
	ghosts[0].outOfhome = true;

	//setting posiiton only if its the first game or the player pressed exit 
	if (newGame)
	{
		//pacman
		pacman.alivePac_texture.loadFromFile("pngs/alive pacman2-20.png");
		pacman.deadPac_texture.loadFromFile("pngs/dead pacman.png");
		pacman.sprite.setTexture(pacman.alivePac_texture);
		pacman.initial_x = 9 * TILESIZE + TILESIZE / 2 + offset_x;
		pacman.initial_y = 15 * TILESIZE + TILESIZE / 2 + offset_y;
		pacman.sprite.setOrigin((player_width / 2), (player_height / 2));
		pacman.sprite.setPosition(pacman.initial_x, pacman.initial_y);
		pacman.sprite.setTextureRect(IntRect(0, 0, player_width, player_height)); //x y w h

		//ghosts
		ghosts[0].initial_x = offset_x + 9 * TILESIZE + HALF_TILESIZE;
		ghosts[0].initial_y = offset_y + 7 * TILESIZE + HALF_TILESIZE;
		ghosts[0].sprite.setPosition(ghosts[0].initial_x, ghosts[0].initial_y);

		ghosts[1].initial_x = ghosts[0].initial_x;
		ghosts[1].initial_y = offset_y + 10 * TILESIZE + HALF_TILESIZE;
		ghosts[1].sprite.setPosition(ghosts[1].initial_x, ghosts[1].initial_y);

		ghosts[2].initial_x = offset_x + 8 * TILESIZE + HALF_TILESIZE;
		ghosts[2].initial_y = offset_y + 9 * TILESIZE + HALF_TILESIZE;
		ghosts[2].sprite.setPosition(ghosts[2].initial_x, ghosts[2].initial_y);

		ghosts[3].initial_x = offset_x + 10 * TILESIZE + HALF_TILESIZE;
		ghosts[3].initial_y = offset_y + 9 * TILESIZE + HALF_TILESIZE;
		ghosts[3].sprite.setPosition(ghosts[3].initial_x, ghosts[3].initial_y);
	}
	
	for (int i = 0; i < ghosts_number; i++) {
		//setting home sprite 
		ghosts[i].home_sprite.setTexture(cherryTexture);
		ghosts[i].home_sprite.setOrigin(TILESIZE / 2, TILESIZE / 2);
		ghosts[i].home_sprite.setPosition(ghosts[i].initial_x, ghosts[i].initial_y);
		//ghosts origin
		ghosts[i].sprite.setOrigin(ghost_width / 2, ghost_height / 2);
		//bfs 
		ghosts[i].algo_window_BFS = 10;
		ghosts[i].num_tiles_past_BFS = ghosts[i].algo_window_BFS;
		ghosts[i].frames_per_tile = TILESIZE / ghostSpeed;

	}


	bool sound = 0, sound2 = 0;
	bool gamess = 1;
	int timer_3seconds = 4, powerUp_8secTimer = 0;
	float elapsedTime_cherry = 0;

	gameS.play();
	Time resettime = seconds(4.0f);
	Clock clock_cherry, play_clock, sec3_clock, powerUp_clock;;

	isPaused = true , sec3_timer = true;

	bool pressed_pause = false;

	while (window.isOpen()) {

		int current_score = 0;

		for (int i = 0; i < NUMBERROW; i++)
		{
			for (int j = 0; j < NUMBERCOLUMNS; j++)
			{

				if (changing_map[i][j] == 2)
				{
					map_[i][j].type = tile_type::none;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);
				}
				else if (changing_map[i][j] == 1)
				{
					map_[i][j].type = tile_type::score; ;
					current_score++;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);

					map_[i][j].cipoint.setRadius(3.0f);
					map_[i][j].cipoint.setOrigin(3, 3);
					map_[i][j].cipoint.setPosition((j * TILESIZE + TILESIZE / 2) + offset_x, (i * TILESIZE + TILESIZE / 2) + offset_y);
					map_[i][j].cipoint.setFillColor(Color{ 255, 250, 165 });
				}
				else if (changing_map[i][j] == 0)
				{
					map_[i][j].type = tile_type::wall;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color{ 36, 31, 201 });
				}
				else if (changing_map[i][j] == 3)
				{
					map_[i][j].type = tile_type::powerup;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);

					map_[i][j].cpowerup.setRadius(10.0f);
					map_[i][j].cpowerup.setOrigin(10, 10);
					map_[i][j].cpowerup.setPosition((j * TILESIZE + TILESIZE / 2) + offset_x, (i * TILESIZE + TILESIZE / 2) + offset_y);
					map_[i][j].cpowerup.setFillColor(Color{ 223, 217, 130 });

				}
				else if (changing_map[i][j] == 8) {
					map_[i][j].type = tile_type::wall;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);
				}
				map_[i][j].recwall.setPosition(j * TILESIZE + offset_x, i * TILESIZE + offset_y);
				map_[i][j].row = i;
				map_[i][j].column = j;
			}

		}

		if (current_score == 0) {
			//victory
			//save highscore
		}

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
					current_map = 1;
					isPaused = true;
					powerUp_Sound.pause();
					gameS.stop();
					//pause(window);
					pressed_pause = true;
					//return;
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

		//3 seconds timer
		stringstream sec3_manip;

		if (sec3_timer && sec3_clock.getElapsedTime().asSeconds() >= 1) {
			timer_3seconds--;
			sec3_clock.restart();
			if (timer_3seconds == 0)
			{
				sec3_timer = false;
				isPaused = false;
			}
		}

		sec3_manip << timer_3seconds;
		sec3_text.setString(sec3_manip.str());
		FloatRect sec3_floatrect = sec3_text.getLocalBounds();
		sec3_text.setOrigin(sec3_floatrect.left + sec3_floatrect.width / 2.0f, sec3_floatrect.top + sec3_floatrect.height / 2.0f);
		sec3_text.setPosition(1920 / 2, 1080 / 2);

		//timer text
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

		//update score text
		stringstream score_manip;
		score_manip << "score:" << pacman.score;
		s.setString(score_manip.str());


		if (!isPaused && sec3_timer == false)
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
				pacman.animation_alive %= 8;
			}
			else if (pacman.moving_direction == 2 && pacman.isAlive) {
				move_left(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(-1, 1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, 0, player_width, player_height));
				pacman.animation_alive %= 8;


			}
			else if (pacman.moving_direction == 1 && pacman.isAlive) {
				move_up(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(1, 1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, player_height, player_width, player_height));
				pacman.animation_alive %= 8;

			}
			else if (pacman.moving_direction == 3 && pacman.isAlive) {
				move_down(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(1, -1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, player_height, player_width, player_height));
				pacman.animation_alive %= 8;

			}

			//red ghost catch pacman
			for (int i = 0; i < ghosts_number; i++) {
				if (!( ghosts[i].isBFS && !ghosts[i].isDead) )
					continue;
				catch_target(ghosts[i], pacman.sprite);
				switch ((ghosts[i].moving_direction)) {
				case 0:
					move_right(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 1:
					move_up(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 2:
					move_left(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 3:
					move_down(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				}
			}

			//rest of the ghosts move random
			move_random(ghosts);
			
			//setting the texture for all ghost based on the mode and direction of moving.
			ghosts_animation(ghosts);

			//eat dots
			if (map_[row_pac][col_pac].type == tile_type::score) {
				if (map_[row_pac][col_pac].cipoint.getGlobalBounds().contains(pacman.sprite.getPosition().x, pacman.sprite.getPosition().y))
				{
					//eatsound.play();
					changing_map[row_pac][col_pac] = 2;
					pacman.score++;

				}
				
			}

			//eat powerBall
			if (map_[row_pac][col_pac].type == tile_type::powerup) {
				if (map_[row_pac][col_pac].cpowerup.getGlobalBounds().contains(pacman.sprite.getPosition().x, pacman.sprite.getPosition().y)) {
					changing_map[row_pac][col_pac] = 2;
					powerUp_Sound.play();
					pacman.powerBallBool = true; // for mai ghost
					for (int i = 0; i < ghosts_number; i++)
					{
						ghosts[i].animation = 1;
						ghosts[i].speed = speedInPowerUp;

					}
				}
			}

			//if power up mode activated
			if (pacman.powerBallBool)
			{

				for (int i = 0; i < ghosts_number; i++)
				{
					// check if pacman ate a ghost    // check timer ??
					if (ghosts[i].animation == 1)
					{
						if (pacman.sprite.getGlobalBounds().intersects(ghosts[i].sprite.getGlobalBounds()))
						{
							float y_ghost = ghosts[i].sprite.getPosition().y,
								x_ghost = ghosts[i].sprite.getPosition().x;
							int row_ghost, col_ghost;
							get_tile_cor(x_ghost, y_ghost, row_ghost, col_ghost);
							ghosts[i].sprite.setPosition(col_ghost * TILESIZE + offset_x + HALF_TILESIZE, row_ghost* TILESIZE + offset_y + HALF_TILESIZE);
							ghosts[i].animation = 2;
							ghosts[i].isDead = true;
							ghosts[i].step_counts_BFS = 0;
							ghosts[i].num_tiles_past_BFS = ghosts[i].algo_window_BFS;
						}
					}
				}
				//check if the time passed after pacman ate the power up ball is eight seconds then
				//return the ghosts to the normal mode/animation and stop the sound.
				if (powerUp_clock.getElapsedTime().asSeconds() >= 1) {
					powerUp_8secTimer++;
					powerUp_clock.restart();
					if (powerUp_8secTimer == 8) {
						powerUp_Sound.stop();
						powerUp_8secTimer = 0;
						for (int i = 0; i < ghosts_number; i++)
						{
							//retring every uneaten ghost back to normal.
							if (ghosts[i].isDead)
								continue;
							ghosts[i].animation = 0;
							ghosts[i].speed = ghostSpeed;
							ghosts[i].gotHome = false;
						}
					}
				}
			}

			//eaten ghosts go home! 
			for (int i = 0; i < ghosts_number; i++) {
				if (! ( ghosts[i].isDead &&  ghosts[i].outOfhome == true) )
					continue;

				ghosts[i].speed = ghostSpeed;

				catch_target(ghosts[i], ghosts[i].home_sprite);
				switch ((ghosts[i].moving_direction)) {
				case 0:
					move_right(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 1:
					move_up(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 2:
					move_left(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 3:
					move_down(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				}

				float x_ghost = ghosts[i].sprite.getPosition().x,
					y_ghost = ghosts[i].sprite.getPosition().y;
				int row_ghost, col_ghost;
				float x_home = ghosts[i].home_sprite.getPosition().x,
					y_home = ghosts[i].home_sprite.getPosition().y;
				int row_home, col_home;
				get_tile_cor(x_ghost, y_ghost, row_ghost, col_ghost);
				get_tile_cor(x_home, y_home, row_home, col_home);
				if (&map_[row_home][col_home] == &map_[row_ghost][col_ghost]) {
					ghosts[i].outOfhome = false;
					ghosts[i].isDead = false;
					ghosts[i].animation = 0;
				}
			}
			
			//ghosts in home go out 
			for (int i = 1; i < ghosts_number; i++) {
  				if (ghosts[i].outOfhome == true)
					continue;
				catch_target(ghosts[i], ghosts[0].home_sprite);
				switch ((ghosts[i].moving_direction)) {
				case 0:
					move_right(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 1:
					move_up(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 2:
					move_left(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 3:
					move_down(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				}
				float x_ghost = ghosts[i].sprite.getPosition().x,
					y_ghost = ghosts[i].sprite.getPosition().y;
				int row_ghost, col_ghost;
				float x_home = ghosts[0].home_sprite.getPosition().x,
					y_home = ghosts[0].home_sprite.getPosition().y;
				int row_home, col_home;
				get_tile_cor(x_ghost, y_ghost, row_ghost, col_ghost);
				get_tile_cor(x_home, y_home, row_home, col_home);
				if (&map_[row_home][col_home] == &map_[row_ghost][col_ghost]) {
					ghosts[i].outOfhome = true;
				}
			}

			//collision with the ghost so pacman would die
			for (int i = 0; i < ghosts_number; i++) {
				if (ghosts[i].sprite.getGlobalBounds().intersects(pacman.sprite.getGlobalBounds()) && !ghosts[i].isDead) {
					pacman.isAlive = false;
					isPaused = true;
					pacman.sprite.setTexture(pacman.deadPac_texture);
				}
			}

			// if pamcan is dead->play death sound
			if (pacman.isAlive == false && pacman.deathSound == false) {
				deathSound.play();
				pacman.deathSound = true;
			}

			//cherry appearing
			elapsedTime_cherry = clock_cherry.getElapsedTime().asSeconds();
			if (elapsedTime_cherry >= 5 && hundredshow == false && pacman.isAlive && pacman.cherry_taken == false) {
				cherry = true;
			}
			if (elapsedTime_cherry >= 10) {
				cherry = false;
				hundredshow = false;
				clock_cherry.restart();
			}
			//eat cherry
			if (pacman.sprite.getGlobalBounds().intersects(cherrySprite.getGlobalBounds()) && cherry == true && pacman.cherry_taken == false) {
				cherry = false;
				pacman.cherry_taken = true;
				eatcherrysound.play();
				pacman.score += 100;
				hundredshow = true;
			}
			
			//hole for both pacman and ghosts
			int left_hole = offset_x, right_hole = offset_x + (NUMBERCOLUMNS * TILESIZE) - TILESIZE;

			if (x_pac + TILESIZE / 2 <= left_hole && pacman.moving_direction == 2) {
				pacman.sprite.setPosition(right_hole + TILESIZE / 2, y_pac);
			}
			if (x_pac - player_width / 2 >= right_hole && pacman.moving_direction == 0) {
				pacman.sprite.setPosition(left_hole - TILESIZE / 2, y_pac);
			}
			for (int i = 0; i < ghosts_number; i++) {
				float x_ghost = ghosts[i].sprite.getPosition().x,
					y_ghost = ghosts[i].sprite.getPosition().y;
				if (x_ghost + TILESIZE / 2 <= left_hole && ghosts[i].moving_direction == 2) {
					ghosts[i].sprite.setPosition(right_hole + TILESIZE / 2, y_ghost);
				}
				if (x_ghost - player_width / 2 >= right_hole && ghosts[i].moving_direction == 0) {
					ghosts[i].sprite.setPosition(left_hole - TILESIZE / 2, y_ghost);
				}
			}

		}
		if (!pacman.isAlive) {
			if (pacman.animetion_dead != 11) {
				pacman.sprite.setScale(1, 1);

				pacman.sprite.setTextureRect(IntRect(38 * pacman.animetion_dead, 0, 38, player_height));
				pacman.animetion_dead++;
				pacman.animetion_dead %= 12;
			}
			else {
				// -> animation of death finished 
				// game over , wanna play again ?
				// save highscore
			}
		}

		window.clear();
		//map
		window.draw(background_sprite);

		for (int i = 0; i < NUMBERROW; i++)
		{
			for (int j = 0; j < NUMBERCOLUMNS; j++)
			{
				//window.draw(map_[i][j].recwall);

				if (map_[i][j].type == tile_type::score)
					window.draw(map_[i][j].cipoint);
				if (map_[i][j].type == tile_type::powerup)
					window.draw(map_[i][j].cpowerup);
			}
		}


		//pause button and score text
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
				pressed_pause = true;
				isPaused = true;
				current_map = 1;
				pause(window, pressed_pause, newGame);
				
			}
		}
		else {
			line1.setFillColor(Color::White);
			line2.setFillColor(Color::White);
		}

		//pacman 
		if (cherry)
			window.draw(cherrySprite);

		for (int i = 0; i < ghosts_number; i++)
		{
			if(pacman.isAlive)
				window.draw(ghosts[i].sprite);
		}

		window.draw(pacman.sprite);

		if (hundredshow)
			window.draw(hundred);

		if (!pacman.isAlive)
			sf::sleep(sf::seconds(pacman.delay));

		if (sec3_timer)
			window.draw(sec3_text);

		window.draw(rect_right);
		window.draw(rect_left);

		if (pressed_pause)
		{
			window.draw(blackRect);
			pause(window, pressed_pause, newGame);
		}
		window.display();
	}
}

//medium windows
void Medium(RenderWindow& window) {
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
		if (newGame) {
			for (int i = 0; i < ghosts_number; i++)
				restart_ghost(ghosts[i]);
			restart_pacman(pacman);
		}
		window.clear();
		LoadingWindow(window);
		originalmediumwindow(window);
		window.display();
	}
}

//original medium window
void originalmediumwindow(RenderWindow& window) {
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

	SoundBuffer eat;
	eat.loadFromFile("sounds/Eat dots.wav");
	Sound eatsound(eat);

	SoundBuffer eatCherry;
	eatCherry.loadFromFile("sounds/pacman_eatfruit.wav");
	Sound eatcherrysound(eatCherry);

	SoundBuffer death;
	death.loadFromFile("sounds/pacman death.wav");
	Sound deathSound(death);

	SoundBuffer powerup;
	powerup.loadFromFile("sounds/PowerUP.wav");
	Sound powerUp_Sound(powerup);

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");
	Text s;
	Text timer, sec3_text;
	sec3_text.setCharacterSize(200);
	sec3_text.setFont(font);

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

	Texture background_texture;
	background_texture.loadFromFile("pngs/medium_background.png");
	Sprite background_sprite;
	background_sprite.setTexture(background_texture);

	RectangleShape blackRect(Vector2f(1920.0f, 1080.0f));
	blackRect.setFillColor(Color{ 0,0,0,100 });

	//score text
	s.setFont(font);
	s.setCharacterSize(35);
	s.setFillColor(Color::White);
	s.setPosition(offset_x, offset_y - 40);

	//time 
	timer.setFont(font);
	timer.setCharacterSize(35);
	timer.setFillColor(Color::White);
	timer.setPosition(offset_x + 565 ,offset_y - 40);

	//cherry
	Texture cherryTexture;
	cherryTexture.loadFromFile("pngs/strawberry.png");
	Sprite cherrySprite;
	cherrySprite.setTexture(cherryTexture);
	cherrySprite.setOrigin(player_width / 2, player_height / 2);
	float cherry_x = 9 * TILESIZE + TILESIZE / 2 + offset_x;
	float cherry_y = 3 * TILESIZE + TILESIZE / 2 + offset_y;
	cherrySprite.setPosition(cherry_x, cherry_y);
	bool cherry = false;

	//afterEdit
	Font numberFont;
	numberFont.loadFromFile("fonts/Joystix.ttf");
	bool hundredshow = false;
	Text hundred;
	hundred.setFont(numberFont);
	hundred.setString("100");
	hundred.setCharacterSize(16);
	hundred.setFillColor(Color::Yellow);
	hundred.setPosition(cherry_x, cherry_y + 10);
	hundred.setOrigin((player_width / 2), (player_height / 2));

	//hole
	RectangleShape rect_right;
	rect_right.setSize(Vector2f(50, 50));
	rect_right.setFillColor(Color::Black);
	rect_right.setPosition(offset_x + TILESIZE * NUMBERCOLUMNS - TILESIZE, offset_y + 12 * TILESIZE);
	RectangleShape rect_left;
	rect_left.setSize(Vector2f(50, 50));
	rect_left.setFillColor(Color::Black);
	rect_left.setPosition(offset_x - rect_left.getSize().x, offset_y + 12 * TILESIZE);
	RectangleShape rect_right2;
	rect_right2.setSize(Vector2f(50, 50));
	rect_right2.setFillColor(Color::Black);
	rect_right2.setPosition(offset_x + TILESIZE * NUMBERCOLUMNS - TILESIZE, offset_y + 10 * TILESIZE);
	RectangleShape rect_left2;
	rect_left2.setSize(Vector2f(50, 50));
	rect_left2.setFillColor(Color::Black);
	rect_left2.setPosition(offset_x - rect_left2.getSize().x, offset_y + 10 * TILESIZE);

	//fireball
	int fireBall_baseSpeed = 4;
	Texture fireBallTexture;
	fireBallTexture.loadFromFile("pngs/fireball.png");
	Sprite fireBallSprite;
	fireBallSprite.setTexture(fireBallTexture);
	fireBallSprite.setPosition((18 * TILESIZE) + (offset_x), (20 * TILESIZE) + (TILESIZE / 2) + (offset_y));
	fireBallSprite.setOrigin((TILESIZE / 2), (TILESIZE / 2));
	fireBallSprite.setTextureRect(IntRect(0, 0, TILESIZE, TILESIZE)); //x y w h 
	int animationFireBall = 0;

	Sprite fireBall_2_Sprite;
	fireBall_2_Sprite.setTexture(fireBallTexture);
	fireBall_2_Sprite.setPosition((4 * TILESIZE) + (offset_x), (16 * TILESIZE) + (TILESIZE / 2) + (offset_y));
	fireBall_2_Sprite.setOrigin((TILESIZE / 2), (TILESIZE / 2));
	fireBall_2_Sprite.setTextureRect(IntRect(0, 0, TILESIZE, TILESIZE)); //x y w h 
	int animationFireBall_2 = 0;

	Sprite fireBall_4_Sprite;
	fireBall_4_Sprite.setTexture(fireBallTexture);
	fireBall_4_Sprite.setPosition((2 * TILESIZE) + (offset_x), (8 * TILESIZE) + (TILESIZE / 2) + (offset_y));
	fireBall_4_Sprite.setOrigin((TILESIZE / 2), (TILESIZE / 2));
	fireBall_4_Sprite.setTextureRect(IntRect(0, 0, TILESIZE, TILESIZE)); //x y w h 
	int animationFireBall_4 = 0;

	//wall light
	Texture wallLightTexture;
	wallLightTexture.loadFromFile("pngs/wall light.png");
	Sprite wallLightSprite;
	wallLightSprite.setTexture(wallLightTexture);
	wallLightSprite.setPosition((18 * TILESIZE) + (offset_x)+4, (20 * TILESIZE) + (TILESIZE / 2) + (offset_y));
	wallLightSprite.setOrigin((8 / 2), (TILESIZE / 2));
	wallLightSprite.setTextureRect(IntRect(0, 0, 8, TILESIZE)); //x y w h 
	int animationWallLight = 0;

	Sprite wallLight_2_Sprite;
	wallLight_2_Sprite.setTexture(wallLightTexture);
	wallLight_2_Sprite.setPosition((4 * TILESIZE) + (offset_x)-4, (16 * TILESIZE) + (TILESIZE / 2) + (offset_y));
	wallLight_2_Sprite.setOrigin((8 / 2), (TILESIZE / 2));
	wallLight_2_Sprite.setTextureRect(IntRect(0, 0, 8, TILESIZE)); //x y w h

	Sprite wallLight_4_Sprite;
	wallLight_4_Sprite.setTexture(wallLightTexture);
	wallLight_4_Sprite.setPosition((2 * TILESIZE) + (offset_x)-4, (8 * TILESIZE) + (TILESIZE / 2) + (offset_y));
	wallLight_4_Sprite.setOrigin((8 / 2), (TILESIZE / 2));
	wallLight_4_Sprite.setTextureRect(IntRect(0, 0, 8, TILESIZE)); //x y w h

	//setting posiiton only if its the first game or the player pressed exit 
	if (newGame)
	{
		//pacman
		pacman.alivePac_texture.loadFromFile("pngs/alive pacman2-20.png");
		pacman.deadPac_texture.loadFromFile("pngs/dead pacman.png");
		pacman.sprite.setTexture(pacman.alivePac_texture);
		pacman.initial_x = 9 * TILESIZE + TILESIZE / 2 + offset_x;
		pacman.initial_y = 15 * TILESIZE + TILESIZE / 2 + offset_y;
		pacman.sprite.setOrigin((player_width / 2), (player_height / 2));
		pacman.sprite.setPosition(9 * TILESIZE + TILESIZE / 2 + offset_x, 12 * TILESIZE + TILESIZE / 2 + offset_y);
		pacman.sprite.setTextureRect(IntRect(0, 0, player_width, player_height)); //x y w h

		//ghosts
		ghosts[0].initial_x = offset_x + 9 * TILESIZE + HALF_TILESIZE;
		ghosts[0].initial_y = offset_y + 8 * TILESIZE + HALF_TILESIZE;
		ghosts[0].sprite.setPosition(ghosts[0].initial_x, ghosts[0].initial_y);

		ghosts[1].initial_x = ghosts[0].initial_x;
		ghosts[1].initial_y = offset_y + 10 * TILESIZE + HALF_TILESIZE;
		ghosts[1].sprite.setPosition(ghosts[1].initial_x, ghosts[1].initial_y);

		ghosts[2].initial_x = offset_x + 8 * TILESIZE + HALF_TILESIZE;
		ghosts[2].initial_y = offset_y + 10 * TILESIZE + HALF_TILESIZE;
		ghosts[2].sprite.setPosition(ghosts[2].initial_x, ghosts[2].initial_y);

		ghosts[3].initial_x = offset_x + 10 * TILESIZE + HALF_TILESIZE;
		ghosts[3].initial_y = offset_y + 10 * TILESIZE + HALF_TILESIZE;
		ghosts[3].sprite.setPosition(ghosts[3].initial_x, ghosts[3].initial_y);
	}
	//GHOSTS	
	// 0 red , 1 pink , 2 orange , 3 blue 

	ghosts[0].isBFS = true;
	ghosts[0].outOfhome = true;
	ghosts[1].isBFS = true;

	//home sprite and bfs needs
	for (int i = 0; i < ghosts_number; i++)	 {
		//setting home sprite 
		ghosts[i].home_sprite.setTexture(cherryTexture);
		ghosts[i].home_sprite.setOrigin(TILESIZE / 2, TILESIZE / 2);
		ghosts[i].home_sprite.setPosition(ghosts[i].initial_x, ghosts[i].initial_y);
		//ghosts origin
		ghosts[i].sprite.setOrigin(ghost_width / 2, ghost_height / 2);
		//bfs 
		ghosts[i].algo_window_BFS = 15;
		ghosts[i].num_tiles_past_BFS = ghosts[i].algo_window_BFS;
		ghosts[i].frames_per_tile = TILESIZE / ghostSpeed;

	}

	bool sound = 0, sound2 = 0;
	bool gamess = 1;
	int timer_3seconds = 4, powerUp_8secTimer = 0;
	float elapsedTime_cherry = 0;
	float elapsedTime_afterEat = 0;

	gameS.play();

	Time resettime = seconds(4.0f);
	Clock clock_cherry, play_clock, sec3_clock, powerUp_clock, afterEat_clock;;

	isPaused = true, sec3_timer = true;
	bool pressed_pause = false;


	while (window.isOpen()) {

		int current_score = 0;
		for (int i = 0; i < NUMBERROW; i++)
		{
			for (int j = 0; j < NUMBERCOLUMNS; j++)
			{

				if (changing_map[i][j] == 2)
				{
					map_[i][j].type = tile_type::none;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);
				}
				else if (changing_map[i][j] == 1)
				{
					current_score++;
					map_[i][j].type = tile_type::score; ;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);

					map_[i][j].cipoint.setRadius(3.0f);
					map_[i][j].cipoint.setOrigin(3, 3);
					map_[i][j].cipoint.setPosition((j * TILESIZE + TILESIZE / 2) + offset_x, (i * TILESIZE + TILESIZE / 2) + offset_y);
					map_[i][j].cipoint.setFillColor(Color{ 255, 250, 165 });
				}
				else if (changing_map[i][j] == 0)
				{
					map_[i][j].type = tile_type::wall;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color{ 36, 31, 201 });
				}
				else if (changing_map[i][j] == 3)
				{
					map_[i][j].type = tile_type::powerup;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);

					map_[i][j].cpowerup.setRadius(10.0f);
					map_[i][j].cpowerup.setOrigin(10, 10);
					map_[i][j].cpowerup.setPosition((j * TILESIZE + TILESIZE / 2) + offset_x, (i * TILESIZE + TILESIZE / 2) + offset_y);
					map_[i][j].cpowerup.setFillColor(Color{ 223, 217, 130 });

				}
				else if (changing_map[i][j] == 8) {
					map_[i][j].type = tile_type::wall;
					map_[i][j].recwall.setSize(Vector2f(TILESIZE, TILESIZE));
					map_[i][j].recwall.setFillColor(Color::Black);
				}
				map_[i][j].recwall.setPosition(j * TILESIZE + offset_x, i * TILESIZE + offset_y);
				map_[i][j].row = i;
				map_[i][j].column = j;
			}

		}
		// victory 
		if (current_score == 0) {
			
			//save highscore
		}

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
					pressed_pause = true;
					isPaused = true;
					current_map = 2;
					gameS.stop();
					pause(window, pressed_pause, newGame);
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

		//3 seconds timer
		stringstream sec3_manip;

		if (sec3_timer && sec3_clock.getElapsedTime().asSeconds() >= 1) {
			timer_3seconds--;
			sec3_clock.restart();
			if (timer_3seconds == 0)
			{
				sec3_timer = false;
				isPaused = false;
			}
		}

		sec3_manip << timer_3seconds;
		sec3_text.setString(sec3_manip.str());
		FloatRect sec3_floatrect = sec3_text.getLocalBounds();
		sec3_text.setOrigin(sec3_floatrect.left + sec3_floatrect.width / 2.0f, sec3_floatrect.top + sec3_floatrect.height / 2.0f);
		sec3_text.setPosition(1920 / 2, 1080 / 2);

		//timer text
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

		//update score text
		stringstream score_manip;
		score_manip << "score:" << pacman.score;
		s.setString(score_manip.str());

		//pacman, ghosts move only when the game is not paused after the sound ends.
		if (!isPaused && sec3_timer == false)
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
				pacman.animation_alive %= 8;
			}
			else if (pacman.moving_direction == 2 && pacman.isAlive) {
				move_left(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(-1, 1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, 0, player_width, player_height));
				pacman.animation_alive %= 8;


			}
			else if (pacman.moving_direction == 1 && pacman.isAlive) {
				move_up(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(1, 1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, player_height, player_width, player_height));
				pacman.animation_alive %= 8;

			}
			else if (pacman.moving_direction == 3 && pacman.isAlive) {
				move_down(pacman.sprite, pacman.moving_direction);
				pacman.animation_alive++;
				pacman.sprite.setScale(1, -1);
				pacman.sprite.setTextureRect(IntRect(player_width * pacman.animation_alive, player_height, player_width, player_height));
				pacman.animation_alive %= 8;

			}

			//ghosts[0], ghosts[1] -> catch pacman
			for (int i = 0; i < ghosts_number; i++) {
				if (!(ghosts[i].isBFS && !ghosts[i].isDead))
					continue;
				catch_target(ghosts[i], pacman.sprite);
				switch ((ghosts[i].moving_direction)) {
				case 0:
					move_right(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 1:
					move_up(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 2:
					move_left(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 3:
					move_down(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				}
			}

			//ghosts in home go out 
			for (int i = 1; i < ghosts_number; i++) {
				if (ghosts[i].outOfhome == true)
					continue;
				catch_target(ghosts[i], ghosts[0].home_sprite);
				switch ((ghosts[i].moving_direction)) {
				case 0:
					move_right(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 1:
					move_up(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 2:
					move_left(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 3:
					move_down(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				}
				float x_ghost = ghosts[i].sprite.getPosition().x,
					y_ghost = ghosts[i].sprite.getPosition().y;
				int row_ghost, col_ghost;
				float x_home = ghosts[0].home_sprite.getPosition().x,
					y_home = ghosts[0].home_sprite.getPosition().y;
				int row_home, col_home;
				get_tile_cor(x_ghost, y_ghost, row_ghost, col_ghost);
				get_tile_cor(x_home, y_home, row_home, col_home);
				if (&map_[row_home][col_home] == &map_[row_ghost][col_ghost]) {
					ghosts[i].outOfhome = true;
				}
			}

			//rest of the ghosts move random
			move_random(ghosts);

			//setting the texture for all ghost based on the mode and direction of moving.
			ghosts_animation(ghosts);

			//fruit appearing
			elapsedTime_cherry = clock_cherry.getElapsedTime().asSeconds();
			if (elapsedTime_cherry >= 5 && hundredshow == false && pacman.isAlive && pacman.cherry_taken == false) {
				cherry = true;
			}
			if (elapsedTime_cherry >= 10) {
				cherry = false;
				hundredshow = false;
				clock_cherry.restart();
			}
			//eat cherry
			if (pacman.sprite.getGlobalBounds().intersects(cherrySprite.getGlobalBounds()) && cherry == true && pacman.cherry_taken == false) {
				cherry = false;
				pacman.cherry_taken = true;
				eatcherrysound.play();
				pacman.score += 100;
				hundredshow = true;
			}

			//eat dots and eat sound
			float timeBeforeEat = afterEat_clock.getElapsedTime().asSeconds();

			if (map_[row_pac][col_pac].type == tile_type::score) {
				if (map_[row_pac][col_pac].cipoint.getGlobalBounds().contains(pacman.sprite.getPosition().x, pacman.sprite.getPosition().y))
				{
					pacman.scoreSound = true;
					changing_map[row_pac][col_pac] = 2;
					pacman.score++;
					elapsedTime_afterEat = timeBeforeEat;
				}
			}
			if (pacman.scoreSound) {
				if (eatsound.getStatus() != SoundSource::Playing) {
					eatsound.play();
					pacman.scoreSound = false;
				}

			}
			if (timeBeforeEat - 0.4 > elapsedTime_afterEat && map_[row_pac][col_pac].type == tile_type::none) {
				eatsound.stop();
			}


			//eat powerBall
			if (map_[row_pac][col_pac].type == tile_type::powerup) {
				if (map_[row_pac][col_pac].cpowerup.getGlobalBounds().contains(pacman.sprite.getPosition().x, pacman.sprite.getPosition().y)) {
					changing_map[row_pac][col_pac] = 2;
					powerUp_Sound.play();
					pacman.powerBallBool = true; // for mai ghost
					for (int i = 0; i < ghosts_number; i++)
					{
						ghosts[i].animation = 1;
						ghosts[i].speed = speedInPowerUp;
					}
				}
			}
			//if power up mode activated -> play sound, check if pacman ate any ghosts
			if (pacman.powerBallBool)
			{

				for (int i = 0; i < ghosts_number; i++)
				{
					if (ghosts[i].animation == 1)
					{
						// check if pacman ate a ghost 
						if (pacman.sprite.getGlobalBounds().intersects(ghosts[i].sprite.getGlobalBounds()))
						{
							float y_ghost = ghosts[i].sprite.getPosition().y,
								x_ghost = ghosts[i].sprite.getPosition().x;
							int row_ghost, col_ghost;
							get_tile_cor(x_ghost, y_ghost, row_ghost, col_ghost);
							ghosts[i].sprite.setPosition(col_ghost * TILESIZE + offset_x + HALF_TILESIZE, row_ghost * TILESIZE + offset_y + HALF_TILESIZE);
							ghosts[i].animation = 2;
							ghosts[i].isDead = true;
							ghosts[i].step_counts_BFS = 0;
							ghosts[i].num_tiles_past_BFS = ghosts[i].algo_window_BFS;
						}
					}
				}
				//check if the time passed after pacman ate the power up ball is eight seconds then
				//return the ghosts to the normal mode/animation and stop the sound.
				if (powerUp_clock.getElapsedTime().asSeconds() >= 1) {
					powerUp_8secTimer++;
					powerUp_clock.restart();
					if (powerUp_8secTimer == 8) {
						powerUp_Sound.stop();
						powerUp_8secTimer = 0;
						for (int i = 0; i < ghosts_number; i++)
						{
							//reterning every uneaten ghost back to normal.
							if (ghosts[i].isDead)
								continue;
							ghosts[i].animation = 0;
							ghosts[i].speed = ghostSpeed;
							ghosts[i].gotHome = false;
						}
					}
				}
			}

			//eaten ghosts go home! 
			for (int i = 0; i < ghosts_number; i++) {
				if (!(ghosts[i].isDead && ghosts[i].outOfhome == true))
					continue;

				ghosts[i].speed = ghostSpeed;

				catch_target(ghosts[i], ghosts[i].home_sprite);
				switch ((ghosts[i].moving_direction)) {
				case 0:
					move_right(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 1:
					move_up(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 2:
					move_left(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				case 3:
					move_down(ghosts[i].sprite, (ghosts[i].moving_direction), ghosts[i].speed);
					break;
				}

				float x_ghost = ghosts[i].sprite.getPosition().x,
					y_ghost = ghosts[i].sprite.getPosition().y;
				int row_ghost, col_ghost;
				float x_home = ghosts[i].home_sprite.getPosition().x,
					y_home = ghosts[i].home_sprite.getPosition().y;
				int row_home, col_home;
				get_tile_cor(x_ghost, y_ghost, row_ghost, col_ghost);
				get_tile_cor(x_home, y_home, row_home, col_home);
				if (&map_[row_home][col_home] == &map_[row_ghost][col_ghost]) {
					ghosts[i].outOfhome = false;
					ghosts[i].isDead = false;
					ghosts[i].animation = 0;
				}
			}

			//collision with the ghost so pacman would die
			for (int i = 0; i < ghosts_number; i++) {
				if (ghosts[i].sprite.getGlobalBounds().intersects(pacman.sprite.getGlobalBounds()) && !ghosts[i].isDead) {
					pacman.isAlive = false;
					isPaused = true;
					pacman.sprite.setTexture(pacman.deadPac_texture);
					
				}
			}

			//if pacman is dead -> play death sound 
			if (pacman.isAlive == false && pacman.deathSound == false) {
				deathSound.play();
				pacman.deathSound = true;
			}

			//hole for both pacman and ghosts
			int left_hole = offset_x, right_hole = offset_x + (NUMBERCOLUMNS * TILESIZE) - TILESIZE;

			if (x_pac + TILESIZE / 2 <= left_hole && pacman.moving_direction == 2) {
				pacman.sprite.setPosition(right_hole + TILESIZE / 2, y_pac);
			}
			if (x_pac - player_width / 2 >= right_hole && pacman.moving_direction == 0) {
				pacman.sprite.setPosition(left_hole - TILESIZE / 2, y_pac);
			}
			for (int i = 0; i < ghosts_number; i++) {
				float x_ghost = ghosts[i].sprite.getPosition().x,
					y_ghost = ghosts[i].sprite.getPosition().y;
				if (x_ghost + TILESIZE / 2 <= left_hole && ghosts[i].moving_direction == 2) {
					ghosts[i].sprite.setPosition(right_hole + TILESIZE / 2, y_ghost);
				}
				if (x_ghost - player_width / 2 >= right_hole && ghosts[i].moving_direction == 0) {
					ghosts[i].sprite.setPosition(left_hole - TILESIZE / 2, y_ghost);
				}
			}


			//light wall
			if (pacman.isAlive) {
				animationWallLight++;
				wallLightSprite.setScale(1, 1);
				wallLightSprite.setTextureRect(IntRect(8 * animationWallLight, 0, 8, TILESIZE));
				animationWallLight %= 5;

				wallLight_2_Sprite.setScale(1, 1);
				wallLight_2_Sprite.setTextureRect(IntRect(8 * animationWallLight, 0, 8, TILESIZE));

				wallLight_4_Sprite.setScale(1, 1);
				wallLight_4_Sprite.setTextureRect(IntRect(8 * animationWallLight, 0, 8, TILESIZE));
			}

			//fire ball
			fireBallSprite.move(-fireBall_baseSpeed, 0);
			animationFireBall++;
			fireBallSprite.setScale(1, 1);
			fireBallSprite.setTextureRect(IntRect(TILESIZE * animationFireBall, 0, TILESIZE, TILESIZE));
			animationFireBall %= 7;
			float x_fire = fireBallSprite.getPosition().x, y_fire = fireBallSprite.getPosition().y;
			int row_fire, col_fire;
			get_tile_cor(x_fire, y_fire, row_fire, col_fire);
			if ((x_fire - (TILESIZE / 2)) < (1 * TILESIZE) + (offset_x)) {
				fireBallSprite.setPosition((18 * TILESIZE) + (offset_x)-5, (20 * TILESIZE) + (TILESIZE / 2) + (offset_y));
			}

			fireBall_2_Sprite.move(fireBall_baseSpeed, 0);
			animationFireBall_2++;
			fireBall_2_Sprite.setScale(-1, 1);
			fireBall_2_Sprite.setTextureRect(IntRect(TILESIZE * animationFireBall_2, 0, TILESIZE, TILESIZE));
			animationFireBall_2 %= 7;
			float x_fire_2 = fireBall_2_Sprite.getPosition().x, y_fire_2 = fireBall_2_Sprite.getPosition().y;
			int row_fire_2, col_fire_2;
			get_tile_cor(x_fire_2, y_fire_2, row_fire_2, col_fire_2);
			if ((x_fire_2 + (TILESIZE / 2)) > (15 * TILESIZE) + (offset_x)) {
				fireBall_2_Sprite.setPosition((4 * TILESIZE) + (offset_x)+5, (16 * TILESIZE) + (TILESIZE / 2) + (offset_y));
			}

			fireBall_4_Sprite.move(fireBall_baseSpeed, 0);
			animationFireBall_4++;
			fireBall_4_Sprite.setScale(-1, 1);
			fireBall_4_Sprite.setTextureRect(IntRect(TILESIZE * animationFireBall_4, 0, TILESIZE, TILESIZE));
			animationFireBall_4 %= 7;
			float x_fire_4 = fireBall_4_Sprite.getPosition().x, y_fire_4 = fireBall_4_Sprite.getPosition().y;
			int row_fire_4, col_fire_4;
			get_tile_cor(x_fire_4, y_fire_4, row_fire_4, col_fire_4);
			if ((x_fire_4 + (TILESIZE / 2)) > (17 * TILESIZE) + (offset_x)) {
				fireBall_4_Sprite.setPosition((2 * TILESIZE) + (offset_x)+5, (8 * TILESIZE) + (TILESIZE / 2) + (offset_y));
			}
			//collision with fireballs -> pacman dies 
			if (pacman.sprite.getGlobalBounds().intersects(fireBallSprite.getGlobalBounds()) ||
				pacman.sprite.getGlobalBounds().intersects(fireBall_2_Sprite.getGlobalBounds()) ||
				pacman.sprite.getGlobalBounds().intersects(fireBall_4_Sprite.getGlobalBounds())) {
				pacman.isAlive = false;
				isPaused = true;
				pacman.sprite.setTexture(pacman.deadPac_texture);
			}


		}
		if (!pacman.isAlive) {
			if (pacman.animetion_dead != 11) {
				pacman.sprite.setScale(1, 1);

				pacman.sprite.setTextureRect(IntRect(38 * pacman.animetion_dead, 0, 38, player_height));
				pacman.animetion_dead++;
				pacman.animetion_dead %= 12;
			}
			else {
				// -> animation of death finished 
				// game over , wanna play again ?
				// save highscore
			}
		}

		window.clear();
		//map
		window.draw(background_sprite);

		for (int i = 0; i < NUMBERROW; i++)
		{
			for (int j = 0; j < NUMBERCOLUMNS; j++)
			{
				//window.draw(map_[i][j].recwall);

				if (map_[i][j].type == tile_type::score)
					window.draw(map_[i][j].cipoint);
				if (map_[i][j].type == tile_type::powerup)
					window.draw(map_[i][j].cpowerup);
			}
		}

		//pause button and score text
		window.draw(s);
		window.draw(timer);
		window.draw(circle);
		window.draw(line1);
		window.draw(line2);

		Mouse mouse;
		//check pause button if pressed 
		if (circle.getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			line1.setFillColor(Color::Red);
			line2.setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				gameS.stop();
				line1.setFillColor(Color::White);
				line2.setFillColor(Color::White);
				pressed_pause = true;
				isPaused = true;
				soundclick.play();
				pause(window, pressed_pause, newGame);

			}
		}
		else {
			line1.setFillColor(Color::White);
			line2.setFillColor(Color::White);
		}

		//pacman 
		if (cherry)
			window.draw(cherrySprite);

		//ghosts
		for (int i = 0; i < 4; i++)
		{
			if (pacman.isAlive)
				window.draw(ghosts[i].sprite);
		}

		window.draw(pacman.sprite);

		if (hundredshow)
			window.draw(hundred);

		if (!pacman.isAlive)
			sf::sleep(sf::seconds(pacman.delay));

		if (sec3_timer)
			window.draw(sec3_text);

		window.draw(wallLightSprite);
		window.draw(wallLight_2_Sprite);
		window.draw(wallLight_4_Sprite);
		if(pacman.isAlive)
		{
			window.draw(fireBallSprite);
			window.draw(fireBall_2_Sprite);
			window.draw(fireBall_4_Sprite);
		}
		window.draw(rect_right);
		window.draw(rect_left);
		window.draw(rect_right2);
		window.draw(rect_left2);

		if (pressed_pause)
		{
			window.draw(blackRect);
			pause(window, pressed_pause, newGame);
		}

		window.display();
	}
}

//hard window
void Hard(RenderWindow& window) {

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
		if (newGame) {
			for (int i = 0; i < ghosts_number; i++)
				restart_ghost(ghosts[i]);
			restart_pacman(pacman);
		}
		window.clear();
		LoadingWindow(window);
		originalhardwindow(window);
		window.display();
	}
}

//original hard window
void originalhardwindow(RenderWindow& window) {
}

//to pause in the middle of the game
void pause(RenderWindow& window , bool &pressed_pause, bool& newGame) {
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
	isPaused = true;

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
				//NOT HERE BUT WE PUT HERE PAUSE WINDOW.
				restart_pacman(pacman);
				for (int i = 0; i < ghosts_number; i++) {
					restart_ghost(ghosts[i]);
				}
				newGame = true;
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
					soundselect.play();
				}
				sound2 = true;
			}
			else {
				sound2 = false;
			}

		}
	}
	
	Mouse mouse;
	//if continue got clicked 
	if (menupause[0].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {
		menupause[0].setFillColor(Color::Red);

		if (Mouse::isButtonPressed(Mouse::Left)) {
			menupause[0].setFillColor(Color::White);
			soundclick.play();
			pressed_pause = false;
			newGame = false;
			if (current_map == 1)
				originaleasywindow(window);
			else if (current_map == 2)
				originalmediumwindow(window);
			else if (current_map == 3)
				originalhardwindow(window);
				
		}
	}
	else {
		menupause[0].setFillColor(Color::White);
	}
	// exit
	if (menupause[1].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

		menupause[1].setFillColor(Color::Red);

		if (Mouse::isButtonPressed(Mouse::Left)) {
			menupause[1].setFillColor(Color::White);
			soundclick.play();

			//reloading the map
			changing_map[NUMBERROW][NUMBERCOLUMNS] = {};
			newGame = true;
			restart_pacman(pacman);
			for (int i = 0; i < ghosts_number; i++) {
				restart_ghost(ghosts[i]);
			}
			newGame = true;

			mainmenu(window);
		}
	}
	else {
		menupause[1].setFillColor(Color::White);
	}
	for (int i = 0; i < 2; i++) {
		window.draw(menupause[i]);
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
	texturePAC_MAN.loadFromFile("pngs/PAC-MAN.png");
	Sprite spritePAC_MAN;
	spritePAC_MAN.setTexture(texturePAC_MAN);
	//spritePAC_MAN.setPosition(0.0f, -400.0f);

	Texture texturePA_MAN;
	texturePA_MAN.loadFromFile("pngs/PA_-MAN.png");
	Sprite spritePA_MAN;
	spritePA_MAN.setTexture(texturePA_MAN);

	Texture textureC;
	textureC.loadFromFile("pngs/__C-___.png");
	Sprite spriteC;
	spriteC.setTexture(textureC);
	float C_x = (-132.0f - 57.0f) * 2;
	//spriteC.setPosition(C_x, -400.0f);
	spriteC.setPosition(C_x, 0);

	Texture textureCclosed;
	textureCclosed.loadFromFile("pngs/__C-___closed.png");
	Sprite spriteCclosed;
	spriteCclosed.setTexture(textureCclosed);
	float Cclosed_x = (-132.0f) * 2;
	//spriteCclosed.setPosition(Cclosed_x, -400.0f);
	spriteCclosed.setPosition(Cclosed_x, 0);

	SoundBuffer buffer;
	buffer.loadFromFile("sounds/Pacman_Introduction_Music-KP-826387403(1).wav");
	Sound Soundpacman;
	Soundpacman.setBuffer(buffer);
	//Soundpacman.setLoop(true);
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
				num = 0;
				window.close();
			}
			else if (Keyboard::isKeyPressed(Keyboard::Key::Space)) {
				num = 0;
				Soundpacman.pause();
				UsernameWindow(window);
			}
		}

		if (C_x > 731.0f - 132.0f and scaleClock.getElapsedTime().asSeconds() >= 6) {
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
			spriteC.setPosition(C_x, -400.0f);
			spriteCclosed.setPosition(Cclosed_x, -400.0f);
			spriteC.setPosition(C_x, 0);
			spriteCclosed.setPosition(Cclosed_x, 0);
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

	//click sound
	SoundBuffer click;
	click.loadFromFile("sounds/enter sound.wav");
	Sound soundclick;
	soundclick.setBuffer(click);

	//pacman
	Texture texturePAC_MAN;
	texturePAC_MAN.loadFromFile("pngs/PAC-MAN.png");
	Sprite spritePAC_MAN;
	spritePAC_MAN.setTexture(texturePAC_MAN);
	spritePAC_MAN.setPosition(0.0f, -400.0f);

	Texture usernameTexture;
	usernameTexture.loadFromFile("pngs/username.png");
	Sprite usernameSprite;
	usernameSprite.setTexture(usernameTexture);

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");

	spritePAC_MAN.setScale(.05f, 0.5f);
	spritePAC_MAN.setPosition(0, -400.0f);

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
				soundclick.play();
				soundclick.play();
				window.close();
			}

			// save username
			if (Keyboard::isKeyPressed(Keyboard::Enter)) {
				soundclick.play();
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
		window.draw(spritePAC_MAN);

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
void LoadEasyMap(int(&map)[ROW][COL]) {
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

//load medium map
template <size_t ROW2, size_t COL2>
void LoadmediumMap(int(&map)[ROW2][COL2]) {
	// open the data file to load the map
	ifstream LoadMap("data/Medium_Map.txt");

	// adding the map to our game
	for (int i = 0; i < NUMBERROW; i++) {
		for (int j = 0; j < NUMBERCOLUMNS; j++) {
			LoadMap >> map[i][j];
		}
	}

	// closing the file
	LoadMap.close();
}

//load hard map
template <size_t ROW3, size_t COL3>
void LoadhardMap(int(&map)[ROW3][COL3]) {
	// open the data file to load the map
	ifstream LoadMap("data/Hard_Map.txt");

	// adding the map to our game
	for (int i = 0; i < NUMBERROW; i++) {
		for (int j = 0; j < NUMBERCOLUMNS; j++) {
			LoadMap >> map[i][j];
		}
	}

	// closing the file
	LoadMap.close();
}

//ghost
void ghosts_animation(struct Ghosts ghosts[])
{
	for (int i = 0; i < ghosts_number; i++)
	{
		if (ghosts[i].animation == 0)
		{
			switch (i)
			{
			case 0:
				ghosts[i].sprite.setTexture(red_ghost_texture);
				break;
			case 1:
				ghosts[i].sprite.setTexture(pink_ghost_texture);
				break;
			case 2:
				ghosts[i].sprite.setTexture(orange_ghost_texture);
				break;
			case 3:
				ghosts[i].sprite.setTexture(blue_ghost_texture);
			}
			ghosts[i].frames++;
			ghosts[i].frames %= 2;
			switch (ghosts[i].moving_direction)
			{
			case 0:
				ghosts[i].sprite.setTextureRect(IntRect(ghost_width * ghosts[i].frames, 0, ghost_width, ghost_height));
				break;
			case 1:
				ghosts[i].sprite.setTextureRect(IntRect(ghost_width * (ghosts[i].frames + 4), 0, ghost_width, ghost_height));
				break;
			case 2:
				ghosts[i].sprite.setTextureRect(IntRect(ghost_width * (ghosts[i].frames + 2), 0, ghost_width, ghost_height));
				break;
			case 3:
				ghosts[i].sprite.setTextureRect(IntRect(ghost_width * (ghosts[i].frames + 6), 0, ghost_width, ghost_height));
				break;
			}
		}
		else if (ghosts[i].animation == 1)
		{
			ghosts[i].sprite.setTexture(powerup_ghost_texture);
			ghosts[i].frames++;
			ghosts[i].frames %= 2;
			ghosts[i].sprite.setTextureRect(IntRect(ghost_width * ghosts[i].frames, 0, ghost_width, ghost_height));
		}
		else if (ghosts[i].animation == 2)
		{
			ghosts[i].sprite.setTexture(face_ghost_texture);
			switch (ghosts[i].moving_direction)
			{
			case 0:
				ghosts[i].sprite.setTextureRect(IntRect(0, 0, 38, 38));
				break;
			case 1:
				ghosts[i].sprite.setTextureRect(IntRect(38, 0, 38, 38));
				break;
			case 2:
				ghosts[i].sprite.setTextureRect(IntRect(38 * 2, 0, 38, 38));
				break;
			case 3:
				ghosts[i].sprite.setTextureRect(IntRect(38 * 3, 0, 38, 38));
				break;
			}
		}
		else if (ghosts[i].animation == 3)
		{
			ghosts[i].sprite.setTexture(endtime_ghost_texture);
			ghosts[i].frames++;
			ghosts[i].frames %= 4;
			ghosts[i].sprite.setTextureRect(IntRect(ghost_width * ghosts[i].frames, 0, ghost_width, ghost_height));
		}
	}
}

bool check_wall(int& direction, Sprite& ghost)
{
	bool can_move = true, condition_1 = false;
	float x = ghost.getPosition().x, y = ghost.getPosition().y;
	int row_1, row_2, col_1, col_2;
	int row, col;

	if (direction == 0)
	{
		condition_1 = same_tile_horz(ghost);

		get_tile_cor(x + (ghost_width / 2) + ghostSpeed + 0.001, y, row, col);
		if (map_[row][col].type == tile_type::wall || !condition_1)
			can_move = false;
	}
	else if (direction == 1)
	{
		condition_1 = same_tile_vert(ghost);

		get_tile_cor(x, y - ghostSpeed - (ghost_height / 2) - 0.001, row, col);
		if (map_[row][col].type == tile_type::wall || !condition_1)
			can_move = false;
	}
	else if (direction == 2)
	{
		condition_1 = same_tile_horz(ghost);

		get_tile_cor(x - ((ghost_width / 2) - 0.001) - ghostSpeed, y, row, col);
		if (map_[row][col].type == tile_type::wall || !condition_1)
			can_move = false;

	}
	else if (direction == 3)
	{
		condition_1 = same_tile_vert(ghost);

		get_tile_cor(x, y + ghostSpeed + (ghost_height / 2) + 0.001, row, col);
		if (map_[row][col].type == tile_type::wall || !condition_1)
			can_move = false;
	}
	return can_move;
}

void move_random(Ghosts ghost[])
{
	srand((int)time(0));

	for (int i = 0; i < 4; i++)
	{
		if (ghosts[i].isBFS || ghosts[i].isDead)
			continue;
		if (ghosts[i].outOfhome == false)
			continue;
		float x_ghost = ghosts[i].sprite.getPosition().x,
			y_ghost = ghosts[i].sprite.getPosition().y;
		int row_ghost, col_ghost;
		float x_home = ghosts[0].home_sprite.getPosition().x,
			y_home = ghosts[0].home_sprite.getPosition().y;
		int row_home, col_home;
		get_tile_cor(x_ghost, y_ghost, row_ghost, col_ghost);
		get_tile_cor(x_home, y_home, row_home, col_home);

		int avaialble_ways = 0;

		for (int moves = 0; moves < 4; moves++)
		{
			if (moves != (2 + ghost[i].moving_direction) % 4)
			{
				if (check_wall(moves, ghost[i].sprite))

					avaialble_ways++;
			}
		}

		int random_direction = rand() % 4;
		if (avaialble_ways > 0)
		{
			while (check_wall(random_direction, ghost[i].sprite) == 0 || random_direction == (2 + ghost[i].moving_direction) % 4)
			{
				random_direction = rand() % 4;
			}

			ghost[i].moving_direction = random_direction;
		}
		else
		{
			ghost[i].moving_direction = (2 + ghost[i].moving_direction) % 4;
		}
		if (ghost[i].moving_direction == 0)
			move_right(ghosts[i].sprite, ghosts[i].moving_direction, ghosts[i].speed);

		else  if (ghost[i].moving_direction == 1)
			move_up(ghosts[i].sprite, ghosts[i].moving_direction, ghosts[i].speed);

		else if (ghost[i].moving_direction == 2)
			move_left(ghosts[i].sprite, ghosts[i].moving_direction, ghosts[i].speed);

		else if (&map_[row_home][col_home] != &map_[row_ghost][col_ghost] && ghosts[i].moving_direction == 3)
			move_down(ghosts[i].sprite, ghosts[i].moving_direction, ghosts[i].speed);

	}


}


void restart_pacman(PACMAN& pacman) {

	timer_min = 0, timer_sec = 0;
	pacman.moving_direction = -1;
	pacman.keyPressed = -1;
	pacman.score = 0;
	pacman.animation_alive = 0;
	pacman.animetion_dead = 0;
	pacman.powerBallBool = false;
	pacman.sprite.setTexture(pacman.alivePac_texture);
	pacman.sprite.setTextureRect(IntRect(0, 0, player_width, player_height));
	pacman.sprite.setTextureRect(IntRect(0, 0, player_width, player_height));
	pacman.isAlive = true;
	pacman.deathSound = false;
	pacman.scoreSound = false;
	pacman.powerBallSound = false;
	pacman.cherrySound = false;
	pacman.ateGhostSound = false;
	pacman.powerBallBool = false;
}
void restart_ghost(Ghosts& ghosts) {

	ghosts.sprite.setPosition(ghosts.initial_x, ghosts.initial_y);
	ghosts.moving_direction = -1;
	ghosts.animation = 0;
	ghosts.isDead = false;
	ghosts.step_counts_BFS = 0;
	ghosts.frames = 0;
	ghosts.num_tiles_past_BFS = ghosts.algo_window_BFS;
	ghosts.gotHome = false;
	ghosts.outOfhome = false;

}
 

//BFS 
bool exist_in_closed(tile* tile, vector <struct tile>& closed) {
	bool ans = 0;
	for (int i = 0; i < closed.size(); i++) {

		if (tile->row == closed[i].row && tile->column == closed[i].column)
			ans = 1;
	}
	return ans;
}

void find_optimal_path(tile* current, tile* target, vector <tile>* get_path) {
	queue <tile> open;
	vector <tile> closed;

	open.push(*current);

	while (!open.empty()) {

		current = &map_[open.front().row][open.front().column];

		open.pop();

		if (current == target)
			break;

		// right
		if (current->column + 1 <= NUMBERCOLUMNS) {
			tile* next_tile = &map_[current->row][current->column + 1];
			if (next_tile->type != tile_type::wall)
			{
				bool check_right = exist_in_closed(next_tile, closed);
				if (!check_right) {
					open.push(*next_tile);
					next_tile->parent = &map_[current->row][current->column];
				}
			}
		}

		//left
		if (current->column - 1 >= 0) {
			tile* next_tile = &map_[current->row][current->column - 1];

			if (next_tile->type != tile_type::wall)
			{
				bool check_left = exist_in_closed(next_tile, closed);
				if (!check_left) {
					open.push(*next_tile);
					next_tile->parent = &map_[current->row][current->column];
				}
			}
		}

		//up
		if (current->row - 1 >= 0) {
			tile* next_tile = &map_[current->row - 1][current->column];
			if (next_tile->type != tile_type::wall)
			{
				bool check_up = exist_in_closed(next_tile, closed);
				if (!check_up) {
					open.push(*next_tile);
					next_tile->parent = &map_[current->row][current->column];
				}
			}
		}

		//down
		if (current->row + 1 <= NUMBERROW - 1) {
			tile* next_tile = &map_[current->row + 1][current->column];

			if (next_tile->type != tile_type::wall)
			{
				bool check_down = exist_in_closed(next_tile, closed);
				if (!check_down) {
					open.push(*next_tile);
					next_tile->parent = &map_[current->row][current->column];
				}
			}
		}
		closed.push_back(*current);
	}

	(*get_path).clear();
	while (current->parent != NULL) {
		(*get_path).push_back(*current);
		current = current->parent;
	}
}

void catch_target(Ghosts& ghost, Sprite& target) {

	//if the ghost finished a whole tile 
	if (ghost.step_counts_BFS % ghost.frames_per_tile == 0) {
		int row, col;
		float x = ghost.sprite.getPosition().x,
			y = ghost.sprite.getPosition().y;
		get_tile_cor(x, y, row, col);
		ghost.step_counts_BFS = 0;

		//if the tile past equals the tiles needed to repeat the algo, or if it's first time for the algo to run.
		if (ghost.num_tiles_past_BFS == ghost.algo_window_BFS) {
			int row_1, col_1;
			float x_1 = target.getPosition().x,
				y_1 = target.getPosition().y;

			get_tile_cor(x_1, y_1, row_1, col_1);
			tile* start_pointer = &map_[row][col];
			start_pointer->parent = NULL;
			tile* target_pointer = &map_[row_1][col_1];

			find_optimal_path(start_pointer, target_pointer, &ghost.shortest_path);
			ghost.num_tiles_past_BFS = 0;
			ghost.shortest_path_index = ghost.shortest_path.size() - 1;

		}
		else {
			ghost.num_tiles_past_BFS++;
		}

		//if the ghost finished the whole path needed to catch the player last run -> move random.
		if (ghost.shortest_path_index == -1) {
			if(!ghost.isDead)
				random_direction(ghost.sprite, ghost.moving_direction);
			else {
				ghost.num_tiles_past_BFS == ghost.algo_window_BFS;
			}
		}
		else
		{

			tile next_tile = ghost.shortest_path[ghost.shortest_path_index];
			ghost.shortest_path_index--;
			int col_diff = col - next_tile.column;
			int row_diff = row - next_tile.row;
			//left
			if (col_diff == 1) {
				ghost.moving_direction = 2;
			}
			//right
			else if (col_diff == -1) {
				ghost.moving_direction = 0;
			}
			//up
			else if (row_diff == 1) {
				ghost.moving_direction = 1;
			}
			//down
			else if (row_diff == -1) {
				ghost.moving_direction = 3;
			}
		}
	}
	ghost.step_counts_BFS++;
}

void random_direction(Sprite& sprite, int& direction) {

	srand((int)time(0));

	int avaialble_ways = 0;

	int random_direction = rand() % 4;

	for (int moves = 0; moves < 4; moves++)
	{
		if (moves != (2 + direction) % 4)
		{
			if (check_wall(moves, sprite) != 0) {
				direction = moves;
				return;
			}
			
		}
	}
	direction = (2 + direction) % 4;

}

void LoadingWindow(RenderWindow& window)
{
	Texture texturePacman;
	texturePacman.loadFromFile("pngs/pacman2.png");
	Sprite spritepacman;
	spritepacman.setTexture(texturePacman);
	spritepacman.setPosition(1760, 960);
	FloatRect pacmanRect = spritepacman.getLocalBounds();
	spritepacman.setOrigin(74.0f, 70.5f);
	spritepacman.setScale(0.75f, 0.75f);

	float angle = 0.f;
	float rotationSpeed = 5.0f;
	Clock clock;

	bool paused = true;

	Texture Text;
	Text.loadFromFile("pngs/Text.png");
	Sprite messageText;
	messageText.setTexture(Text);
	messageText.setPosition(0.0f, 0.0f);

	while (window.isOpen())
	{

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				return;
			}

			if (Keyboard::isKeyPressed(Keyboard::Space) || clock.getElapsedTime().asSeconds() >= 10.0f) {
				//put the original window
				return;
			}
		}

		//Rotate pacman
		angle += rotationSpeed;
		if (angle >= 360.f) {
			angle = 0.0f;
		}

		spritepacman.setRotation(angle);


		window.clear();

		window.draw(messageText);
		window.draw(spritepacman);

		window.display();
	}
}

//are you sure window
void areyousure(RenderWindow& window) {

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

	Font font;
	font.loadFromFile("fonts/CrackMan.ttf");

	//ARE YOU SURE
	Texture textsure;
	textsure.loadFromFile("pngs/are_you_sure.jpg");
	Sprite spritesure;
	spritesure.setTexture(textsure);
	FloatRect rectsure = spritesure.getLocalBounds();
	spritesure.setOrigin(rectsure.left + rectsure.width / 2.0f, rectsure.top + rectsure.height / 2.0f);
	spritesure.setPosition(Vector2f(960, 540));

	//yes no
	Text yesno[2];

	yesno[0].setFont(font);
	yesno[1].setFont(font);


	yesno[0].setFillColor(Color::White);
	yesno[1].setFillColor(Color::White);

	yesno[0].setString("yes");
	yesno[1].setString("no");

	yesno[0].setCharacterSize(75);
	yesno[1].setCharacterSize(75);

	FloatRect textrect = yesno[0].getLocalBounds();
	FloatRect textrect1 = yesno[1].getLocalBounds();

	yesno[0].setOrigin(textrect.left + textrect.width / 2.0f, textrect.top + textrect.height / 2.0f);
	yesno[1].setOrigin(textrect1.left + textrect1.width / 2.0f, textrect1.top + textrect1.height / 2.0f);


	yesno[0].setPosition(Vector2f(760, 680));
	yesno[1].setPosition(Vector2f(1165, 670));

	bool sound = 0, sound2 = 0;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				break;
			}
			if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Escape) {
					soundselect.play();
					window.close();
					return;
				}
			}
			if (event.type == Event::MouseButtonPressed) {
				Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
				for (int i = 0; i < 2; i++) {
					if (yesno[i].getGlobalBounds().contains(mousePos)) {
						soundclick.play();

					}

				}
			}
			if (event.type == Event::MouseMoved) {
				// Check if the mouse is over the button
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				if (yesno[0].getGlobalBounds().contains(mousePos)) {
					if (!sound) {
						// Play the sound if the mouse just entered the button
						soundselect.play();
					}
					sound = true;
				}
				else {
					sound = false;
				}

				if (yesno[1].getGlobalBounds().contains(mousePos)) {
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

		}

		Mouse mouse;
		if (yesno[0].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			yesno[0].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				window.close();
				yesno[0].setFillColor(Color::White);
				break;
			}
		}
		else {
			yesno[0].setFillColor(Color::White);
		}

		if (yesno[1].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

			yesno[1].setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left)) {
				mainmenu(window);
				yesno[1].setFillColor(Color::White);
				break;
			}
		}
		else {
			yesno[1].setFillColor(Color::White);
		}
		window.clear();
		window.draw(spritesure);
		window.draw(yesno[0]);
		window.draw(yesno[1]);

		window.display();
	}
}

//game over window
void gameover(RenderWindow& window) {

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

	Font font;
	font.loadFromFile("fonts/DEBUG FREE TRIAL.ttf");

	//game over
	Texture textgameover;
	textgameover.loadFromFile("pngs/game over.png");
	Sprite spritegameover;
	spritegameover.setTexture(textgameover);
	FloatRect rectgameover = spritegameover.getLocalBounds();
	spritegameover.setOrigin(rectgameover.left + rectgameover.width / 2.0f, rectgameover.top + rectgameover.height / 2.0f);
	spritegameover.setPosition(Vector2f(960, 540));

	Text yesno[2];

	yesno[0].setFont(font);
	yesno[1].setFont(font);


	yesno[0].setFillColor(Color::White);
	yesno[1].setFillColor(Color::White);

	yesno[0].setString("yes");
	yesno[1].setString("no");

	yesno[0].setCharacterSize(100);
	yesno[1].setCharacterSize(100);

	FloatRect textrect = yesno[0].getLocalBounds();
	FloatRect textrect1 = yesno[1].getLocalBounds();

	yesno[0].setOrigin(textrect.left + textrect.width / 2.0f, textrect.top + textrect.height / 2.0f);
	yesno[1].setOrigin(textrect1.left + textrect1.width / 2.0f, textrect1.top + textrect1.height / 2.0f);

	yesno[0].setPosition(Vector2f(760, 680));
	yesno[1].setPosition(Vector2f(1165, 680));

	bool sound = 0, sound2 = 0;
	//0 -> yes , 1-> no

	Event event;
	while (window.pollEvent(event)) {

		if (event.type == Event::KeyReleased) {
			if (event.key.code == Keyboard::Escape) {
				soundselect.play();
				newGame = true;
				mainmenu(window);
				return;
			}
		}
		//no -> mainmenu1
		if (event.type == Event::MouseButtonPressed) {
			Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
			for (int i = 0; i < 2; i++) {
				if (yesno[i].getGlobalBounds().contains(mousePos)) 
				{
					soundclick.play();
					newGame = true;
					mainmenu(window);
				}

			}
		}
		if (event.type == Event::MouseMoved) {
			// Check if the mouse is over the button
			sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			if (yesno[0].getGlobalBounds().contains(mousePos)) {
				if (!sound)
					// Play the sound if the mouse just entered the button
					soundselect.play();
				sound = true;
			}
			else 
				sound = false;

			if (yesno[1].getGlobalBounds().contains(mousePos)) {
				if (!sound2) 
					// Play the sound if the mouse just entered the button
					soundselect.play();
				
				sound2 = true;
			}
			else 
				sound2 = false;
		}
	}

	Mouse mouse;
	// yes -> continue 
	if (yesno[0].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

		yesno[0].setFillColor(Color::Red);

		if (Mouse::isButtonPressed(Mouse::Left)) {
			//ALL FROM RESTART
			newGame = true;
			if (current_map == 1)
				originaleasywindow(window);
			else if (current_map == 2)
				originalmediumwindow(window);
			else if (current_map == 3)
				originalhardwindow(window);

			yesno[0].setFillColor(Color::White);

		}
	}
	else {
		yesno[0].setFillColor(Color::White);
	}

	if (yesno[1].getGlobalBounds().contains(mouse.getPosition(window).x, mouse.getPosition(window).y)) {

		yesno[1].setFillColor(Color::Red);

		if (Mouse::isButtonPressed(Mouse::Left)) {
			mainmenu(window);
			yesno[1].setFillColor(Color::White);
			//break;
		}
	}
	else {
		yesno[1].setFillColor(Color::White);
	}

	window.clear();
	window.draw(spritegameover);
	window.draw(yesno[0]);
	window.draw(yesno[1]);
	window.display();
	}


