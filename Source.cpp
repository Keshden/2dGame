//#include <iostream>
#include "SFML-2.5.1\include\SFML\Graphics.hpp"
#include "SFML-2.5.1\include\SFML\Audio.hpp"
#include "Hero.h"
#include <vector>
#include "Enemy.h"
#include "Rocket.h"

sf::Vector2f viewSize(1024.0f, 768.0f);
sf::VideoMode vm(1024, 768);
sf::RenderWindow window(vm, "Hello SFMLGame !!!", 
	sf::Style::Default);

//sf Vector2f to get Player left and right arrow key inputs
sf::Vector2f playerPosition;
bool playerMoving = false;

//Sky png Texture and Sprite
sf::Texture skyTexture;
sf::Sprite skySprite;

//Bg png Texture and Sprite
sf::Texture bgTexture;
sf::Sprite bgSprite;
Hero hero;
std::vector<Enemy*> enemies;
std::vector<Rocket*> rockets;



//add variables to keep track of time so that new enemies can be spawned at set intervals
float currentTime;
float prevTime = 0.0f;

//add score variable and variable to see if player went gameover or not.
int score = 0;
bool gameover = true;

//Text variable for in game text feature.
sf::Font headingFont;
sf::Text headingText;

//Text and Font for score
sf::Font scoreFont;
sf::Text scoreText;

//Tutorial information as a Text
sf::Text tutorialText;

//Create background music variable
sf::Music bgMusic;

//Sound buffers for rockets fired and enemies hit
sf::SoundBuffer fireBuffer;
sf::SoundBuffer hitBuffer;
sf::Sound fireSound(fireBuffer);
sf::Sound hitSound(hitBuffer);

//Prototype functions
void spawnEnemy();
void shoot();
bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2);
void init();
void update(float);
void draw();
void updateInput();
void reset();

int main()
{
	//Initialize timer for button pressed
	sf::Clock clock;
	window.setFramerateLimit(60);

	//Generate the texture
	init();
	
	while (window.isOpen())
	{
		//Update Input;
		updateInput();

		//Update Game
		sf::Time dt = clock.restart();
		if(!gameover)
			update(dt.asSeconds());
		
		//Clear window and display texture.
		window.clear(sf::Color::Blue);
		draw();

		window.display();

	}

	return 0;
}

void init()
{

	//Load each Texture
	skyTexture.loadFromFile("Assets/graphics/sky.png");
	bgTexture.loadFromFile("Assets/graphics/bg.png");

	//Set and Attach each Texture to corresponding Sprite
	skySprite.setTexture(skyTexture);
	bgSprite.setTexture(bgTexture);
	hero.init("Assets/graphics/heroAnim.png", 4, 1.0f, sf::Vector2f(viewSize.x * 0.25f, viewSize.y * 0.5f), 200);

	srand((int)time(0));

	//Load Font
	headingFont.loadFromFile("Assets/fonts/SnackerComic.ttf");

	//Set Heading Text
	headingText.setFont(headingFont);
	headingText.setString("Tiny Bazooka");
	headingText.setCharacterSize(84);
	headingText.setFillColor(sf::Color::Red);
	sf::FloatRect headingbounds = headingText.getLocalBounds();
	headingText.setOrigin(headingbounds.width / 2, headingbounds.height / 2);
	headingText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

	//set scoreFont and scoreText
	scoreFont.loadFromFile("Assets/fonts/arial.ttf");
	scoreText.setFont(scoreFont);
	scoreText.setString("Score: 0");
	scoreText.setCharacterSize(45);
	scoreText.setFillColor(sf::Color::Red);
	sf::FloatRect scoringBounds = scoreText.getLocalBounds();
	scoreText.setOrigin(scoringBounds.width / 2, scoringBounds.height / 2);
	scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

	//set tutorialText so player knows controls of game and how to play before starting.
	tutorialText.setFont(scoreFont);
	tutorialText.setString("Press Down Arrow to Fire and Start Game, Up Arrow to Jump");
	tutorialText.setCharacterSize(35);
	tutorialText.setFillColor(sf::Color::Red);

	//Set tutorialText positioning on menu/gameover screen
	sf::FloatRect tutorialBounds = tutorialText.getLocalBounds();
	tutorialText.setOrigin(tutorialBounds.width / 2, tutorialBounds.height / 2);
	tutorialText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.20f));

	//Initialize background music variable and play
	bgMusic.openFromFile("Assets/audio/bgMusic.ogg");
	bgMusic.play();

	//Initialize fire and sound buffers
	hitBuffer.loadFromFile("Assets/audio/hit.ogg");
	fireBuffer.loadFromFile("Assets/audio/fire.ogg");

}

void update(float dt)
{
	hero.update(dt);
	currentTime += dt;

	//Spawn Enemies
	if (currentTime >= prevTime + 1.125f)
	{
		spawnEnemy();
		prevTime = currentTime;
	}

	//Update Enemies
	for (int i = 0; i < enemies.size(); i++)
	{
		Enemy *enemy = enemies[i];

		enemy->update(dt);

		if (enemy->getSprite().getPosition().x < 0)
		{
			enemies.erase(enemies.begin() + i);
			delete(enemy);
			gameover = true;
		}
	}

	//Update Rockets
	for (int i = 0; i < rockets.size(); i++)
	{
		Rocket* rocket = rockets[i];
		rocket->update(dt);

		if (rocket->getSprite().getPosition().x > viewSize.x)
		{
			rockets.erase(rockets.begin() + i);
			delete(rocket);
		}
	}

	//Check Collisions between Rocket and Enemies
	for (int i = 0; i < rockets.size(); i++)
	{
		for (int j = 0; j < enemies.size(); j++)
		{
			Rocket* rocket = rockets[i];
			Enemy* enemy = enemies[j];

			if (checkCollision(rocket->getSprite(), enemy->getSprite()))
			{
				//When enemies get hit, play the hit sound
				hitSound.play();

				//If rocket hits enemy, update score and display current player score.
				score++;
				std::string finalScore = "Score: " + std::to_string(score);
				scoreText.setString(finalScore);
				sf::FloatRect scoreBounds = scoreText.getLocalBounds();
				scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
				scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

				//Rocket and enemy hit must 'disappear' to show that it was a hit.
				rockets.erase(rockets.begin() + i);
				enemies.erase(enemies.begin() + j);
				delete(rocket);
				delete(enemy);

				printf("Rocket intersects enemy \n");
			}
		}
	}
}

void updateInput()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		//Jump when up arrow key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Up)
			{
				hero.jump(750.0f);
			}

			if (event.key.code == sf::Keyboard::Down)
			{
				if (gameover)
				{
					gameover = false;
					reset();
				}
				else
				{
					shoot();
				}
			}
		}

		//Exit program if Esc is pressed
		if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed)
			window.close();
	}
}


void draw()
{
	// Draw/Display each Sprite
	window.draw(skySprite);
	window.draw(bgSprite);
	window.draw(hero.getSprite());

	for (Enemy *enemy : enemies)
	{
		window.draw(enemy->getSprite());
	}

	for (Rocket *rocket : rockets)
	{
		window.draw(rocket->getSprite());
	}

	if (gameover)
	{
		window.draw(headingText);
		window.draw(tutorialText);
	}
	else
	{
		window.draw(scoreText);
	}
}

void spawnEnemy()
{
	int randLoc = rand() % 3;
	sf::Vector2f enemyPos;
	float speed;

	switch (randLoc)
	{
	case 0: enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.75f);
		speed = -400;
		break;

	case 1: enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.60f);
		speed = -550;
		break;

	case 2: enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.40f);
		speed = -650;
		break;

	default: printf("incorrect y value \n");
		return;
	}

	Enemy* enemy = new Enemy();
	enemy->init("Assets/graphics/enemy.png", enemyPos, speed);

	enemies.push_back(enemy);

}

void shoot()
{
	Rocket* rocket = new Rocket();

	rocket->init("Assets/graphics/rocket.png", hero.getSprite().getPosition(), 400.0f);

	rockets.push_back(rocket);

	//play the rocket firing sound
	fireSound.play();
}

bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2)
{
	sf::FloatRect shape1 = sprite1.getGlobalBounds();
	sf::FloatRect shape2 = sprite2.getGlobalBounds();

	if (shape1.intersects(shape2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void reset()
{
	score = 0;
	currentTime = 0.0f;
	prevTime = 0.0;
	scoreText.setString("Score: 0");

	for (Enemy *enemy : enemies)
	{
		delete(enemy);
	}

	for (Rocket *rocket : rockets)
	{
		delete(rocket);
	}

	enemies.clear();
	rockets.clear();
}

/*

	//Former code to initialize shapes and such
	//Initialize Game Objects
	sf::RectangleShape rect(sf::Vector2f(500.0f, 300.0f));
	rect.setFillColor(sf::Color::Yellow);
	rect.setPosition(viewSize.x / 2, viewSize.y / 2);
	rect.setOrigin(sf::Vector2f(rect.getSize().x / 2, rect.getSize().y / 2));

	sf::CircleShape circle(100);
	circle.setFillColor(sf::Color::Green);
	circle.setPosition(viewSize.x / 2, viewSize.y / 2);
	circle.setOrigin(sf::Vector2f(circle.getRadius(), circle.getRadius()));

	sf::ConvexShape triangle;
	triangle.setPointCount(3);
	triangle.setPoint(0, sf::Vector2f(-100, 0));
	triangle.setPoint(1, sf::Vector2f(0, -100));
	triangle.setPoint(2, sf::Vector2f(100, 0));
	triangle.setFillColor(sf::Color(128, 0, 128, 255));
	triangle.setPosition(viewSize.x / 2, viewSize.y / 2);


	//Handle Keyboard Events
	//Update Game Objects in the Scene
	window.clear(sf::Color::Red);
	//Render Game Objects
	window.draw(rect);
	window.draw(circle);
	window.draw(triangle);
	window.display();
*/