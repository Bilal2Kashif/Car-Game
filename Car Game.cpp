#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

sf::FloatRect getAdjustedBounds(const sf::Sprite& sprite, float shrinkX, float shrinkY) {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    bounds.left += shrinkX;
    bounds.width -= 2 * shrinkX;
    bounds.top += shrinkY;
    bounds.height -= 2 * shrinkY;
    return bounds;
}

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int screenWidth = desktop.width;
    unsigned int screenHeight = desktop.height;

    sf::RenderWindow window(desktop, "Car Sprite Game", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(nullptr)));

    float roadWidth = screenWidth * 0.6f;
    float roadLeft = (screenWidth - roadWidth) / 2.0f;
    float roadRight = roadLeft + roadWidth;

    // Load font
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/segoeui.ttf")) {
        return -1;
    }


    bool gameStarted = false;
    bool isPaused = false;
    bool gameOver = false;

    sf::Text startText("Press ENTER to Start", font, 48);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(screenWidth / 2 - 250, screenHeight / 2 - 100);

    sf::Text pauseText("PAUSED - Press P to Resume", font, 48);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(screenWidth / 2 - 350, screenHeight / 2 - 100);

    // Load player car texture
    sf::Texture carTexture;
    if (!carTexture.loadFromFile("C://Users//hp//Desktop//UNI//OOP//Car Game//car.png")) {
        return -1;
    }

    sf::Sprite carSprite;
    carSprite.setTexture(carTexture);
    carSprite.setScale(0.25f, 0.25f);
    float carStartX = screenWidth / 2.0f - carSprite.getGlobalBounds().width / 2.0f;
    float carStartY = screenHeight - carSprite.getGlobalBounds().height - 50;
    carSprite.setPosition(carStartX, carStartY);

    // Load enemy car texture
    sf::Texture enemyTexture;
    if (!enemyTexture.loadFromFile("C://Users//hp//Desktop//UNI//OOP//Car Game//enemy.png")) {
        return -1;
    }

    sf::Sprite tempEnemy;
    tempEnemy.setTexture(enemyTexture);
    tempEnemy.setScale(0.3f, 0.3f);
    float enemyWidth = tempEnemy.getGlobalBounds().width;
    float enemyHeight = tempEnemy.getGlobalBounds().height;

    // Black rectangles for footpaths
    sf::RectangleShape leftFootpath(sf::Vector2f(roadLeft, screenHeight));
    leftFootpath.setFillColor(sf::Color::Black);
    leftFootpath.setPosition(0, 0);

    sf::RectangleShape rightFootpath(sf::Vector2f(screenWidth - roadRight, screenHeight));
    rightFootpath.setFillColor(sf::Color::Black);
    rightFootpath.setPosition(roadRight, 0);

    sf::Texture coinTexture;
    if (!coinTexture.loadFromFile("C://Users//hp//Desktop//UNI//OOP//Car Game//coin.png")) {
        return -1;
    }

    int score = 0;
    sf::Text scoreText("Score: 0", font, 30);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(20, 20);  // Adjust to your side panel


    // Road lines
    vector<sf::RectangleShape> lines;
    int laneCount = 5;
    float laneWidth = roadWidth / laneCount;
    for (int i = 1; i < laneCount; i++) {
        float x = roadLeft + i * laneWidth;
        for (int y = 0; y < static_cast<int>(screenHeight); y += 100) {
            sf::RectangleShape l(sf::Vector2f(10, 50));
            l.setFillColor(sf::Color::White);
            l.setPosition(x - 5, y);
            lines.push_back(l);
        }
    }

    vector<float> enemySpawnPositions;
    for (int i = 0; i < laneCount; i++) {
        float laneCenter = roadLeft + (i * laneWidth) + (laneWidth / 2);
        enemySpawnPositions.push_back(laneCenter - (enemyWidth / 2));
    }

    struct EnemyCar {
        sf::Sprite sprite;
        int lane;
    };

    vector<EnemyCar> enemies;
    int spawnTimer = 0;
    const int spawnDelay = 40;

    struct Coin {
        sf::Sprite sprite;
        bool collected = false;
    };

    std::vector<Coin> coins;
    int coinSpawnTimer = 0;
    const int coinSpawnDelay = 50;  // Less frequent than enemies

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        // Handle start and pause toggles
        if (!gameStarted && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            gameStarted = true;
            sf::sleep(sf::milliseconds(200));
        }
        else if (gameStarted && sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            isPaused = !isPaused;
            sf::sleep(sf::milliseconds(200));
        }

        if (gameStarted && !isPaused && !gameOver) {
            float moveSpeed = 20.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && carSprite.getPosition().x > roadLeft)
                carSprite.move(-moveSpeed, 0);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
                carSprite.getPosition().x + carSprite.getGlobalBounds().width < roadRight)
                carSprite.move(moveSpeed, 0);

            for (auto& line : lines) {
                line.move(0, 20);
                if (line.getPosition().y > screenHeight)
                    line.setPosition(line.getPosition().x, -50);
            }

            spawnTimer++;
            if (spawnTimer >= spawnDelay) {
                spawnTimer = 0;
                EnemyCar e;
                e.sprite.setTexture(enemyTexture);
                e.sprite.setScale(0.3f, 0.3f);
                int lane = rand() % enemySpawnPositions.size();
                e.lane = lane;
                e.sprite.setPosition(enemySpawnPositions[lane], -enemyHeight);
                enemies.push_back(e);
            }

            coinSpawnTimer++;
            if (coinSpawnTimer >= coinSpawnDelay) {
                coinSpawnTimer = 0;
                Coin coin;
                coin.sprite.setTexture(coinTexture);
                coin.sprite.setScale(0.2f, 0.2f);
                int lane = rand() % enemySpawnPositions.size();
                coin.sprite.setPosition(enemySpawnPositions[lane]+10, -50);
                coins.push_back(coin);
            }


            for (auto& e : enemies)
                e.sprite.move(0, 30);

            enemies.erase(remove_if(enemies.begin(), enemies.end(),
                [screenHeight](const EnemyCar& e) {
                    return e.sprite.getPosition().y > screenHeight;
                }), enemies.end());

            sf::FloatRect playerBounds = getAdjustedBounds(carSprite, 35.f, 2.f);
            for (auto& e : enemies) {
                sf::FloatRect enemyBounds = getAdjustedBounds(e.sprite, 22.f, 18.f);
                if (playerBounds.intersects(enemyBounds)) {
                    gameOver = true;
                }
            }
            for (auto& coin : coins) {
                if (!coin.collected)
                    coin.sprite.move(0, 10);

                if (!coin.collected && carSprite.getGlobalBounds().intersects(coin.sprite.getGlobalBounds())) {
                    coin.collected = true;
                    score++;  // Increase score
                }
            }

        }

        // Draw everything
        window.clear(sf::Color(100, 100, 100)); // grey road
        window.draw(leftFootpath);
        window.draw(rightFootpath);

        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        for (auto& line : lines)
            window.draw(line);
        for (auto& e : enemies)
            window.draw(e.sprite);
        window.draw(carSprite);

        for (auto& coin : coins) {
            if (!coin.collected)
                window.draw(coin.sprite);
        }


        if (!gameStarted)
            window.draw(startText);
        else if (isPaused)
            window.draw(pauseText);

        window.display();

        if (gameOver) {
            sf::sleep(sf::seconds(1));
            window.close();
        }
    }

    return 0;
}
