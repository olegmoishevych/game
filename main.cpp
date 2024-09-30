#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> 

const float gravity = 9.8f;

struct Bullet {
    sf::RectangleShape shape;
    float speed;

    Bullet(float x, float y, float dir = -1.0f) { 
        shape.setSize(sf::Vector2f(5.0f, 10.0f));
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y);
        speed = 300.0f * dir; 
    }

    void update(float deltaTime) {
        shape.move(0, speed * deltaTime);
    }
};

struct Enemy {
    sf::RectangleShape shape;
    float speed;
    float fallSpeed; 
    float shootCooldown;

    Enemy(float x, float y) {
        shape.setSize(sf::Vector2f(50.0f, 50.0f));
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(x, y);
        speed = 100.0f;
        fallSpeed = 0.0f; 
        shootCooldown = static_cast<float>(rand() % 5 + 1); 
    }

    void update(float deltaTime) {
        
        fallSpeed += gravity * deltaTime;
        shape.move(0, fallSpeed * deltaTime);

        shootCooldown -= deltaTime;
    }

    bool canShoot() {
        return shootCooldown <= 0.0f;
    }

    void resetShootCooldown() {
        shootCooldown = static_cast<float>(rand() % 5 + 1);
    }
};

int main() {
    
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tank Game with Enemy Shooting");

    
    sf::RectangleShape tankBody(sf::Vector2f(50.0f, 30.0f));
    tankBody.setFillColor(sf::Color::Green);
    tankBody.setPosition(375.0f, 500.0f); 

    sf::RectangleShape tankCannon(sf::Vector2f(30.0f, 10.0f));
    tankCannon.setFillColor(sf::Color::Red);
    tankCannon.setPosition(385.0f, 490.0f); 

    
    bool isJumping = false;
    bool isGameOver = false; 
    bool isRestarting = false;
    float jumpSpeed = -300.0f; 
    float verticalSpeed = 0.0f; 

    
    float playerSpeed = 200.0f;


    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Bullet> enemyBullets;

    sf::Clock spawnClock;
    sf::Clock gameClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }


if (isGameOver) {
    window.clear(sf::Color::Black);
    window.display();


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        isGameOver = false;
        isJumping = false;
        bullets.clear();
        enemies.clear();
        enemyBullets.clear();

        
        tankBody.setPosition(375.0f, 500.0f);
        tankCannon.setPosition(385.0f, 490.0f);
    }
    continue;
}

        float deltaTime = gameClock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && tankBody.getPosition().x > 0) {
            tankBody.move(-playerSpeed * deltaTime, 0);
            tankCannon.move(-playerSpeed * deltaTime, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && tankBody.getPosition().x + tankBody.getSize().x < 800) {
            tankBody.move(playerSpeed * deltaTime, 0);
            tankCannon.move(playerSpeed * deltaTime, 0);
        }

    
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !isJumping) {
            isJumping = true;
            verticalSpeed = jumpSpeed;
        }

    
        if (isJumping) {
            verticalSpeed += gravity * 50 * deltaTime;
            tankBody.move(0, verticalSpeed * deltaTime);
            tankCannon.move(0, verticalSpeed * deltaTime);

    
            if (tankBody.getPosition().y >= 500.0f) {
                tankBody.setPosition(tankBody.getPosition().x, 500.0f);
                tankCannon.setPosition(tankCannon.getPosition().x, 490.0f);
                isJumping = false;
                verticalSpeed = 0.0f;
            }
        }

        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            bullets.push_back(Bullet(tankCannon.getPosition().x + tankCannon.getSize().x / 2, tankCannon.getPosition().y));
        }

        
        for (auto& bullet : bullets) {
            bullet.update(deltaTime);
        }


        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) {
            return b.shape.getPosition().y < 0;
        }), bullets.end());

        
        if (spawnClock.getElapsedTime().asSeconds() > 1.0f) {
            enemies.push_back(Enemy(rand() % 750, -50.0f)); 
            spawnClock.restart();
        }

        
        for (auto& enemy : enemies) {
            enemy.update(deltaTime);

        
            if (enemy.canShoot()) {
                enemyBullets.push_back(Bullet(enemy.shape.getPosition().x + enemy.shape.getSize().x / 2, enemy.shape.getPosition().y, 1.0f)); // пуля вниз
                enemy.resetShootCooldown();
            }
        }

        
        for (auto& bullet : enemyBullets) {
            bullet.update(deltaTime);
        }

        
        enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(), [](Bullet& b) {
            return b.shape.getPosition().y > 600;
        }), enemyBullets.end());

        
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy& e) {
            return e.shape.getPosition().y > 600;
        }), enemies.end());

        
        for (auto& bullet : bullets) {
            for (auto& enemy : enemies) {
                if (bullet.shape.getGlobalBounds().intersects(enemy.shape.getGlobalBounds())) {
                    bullet.shape.setPosition(-100, -100); 
                    enemy.shape.setPosition(-100, -100); 
                }
            }
        }

        
        for (auto& bullet : enemyBullets) {
            if (bullet.shape.getGlobalBounds().intersects(tankBody.getGlobalBounds())) {
                isGameOver = true; 
            }
        }


        window.clear();
        window.draw(tankBody);
        window.draw(tankCannon);


        for (auto& bullet : bullets) {
            window.draw(bullet.shape);
        }


        for (auto& bullet : enemyBullets) {
            window.draw(bullet.shape);
        }

        
        for (auto& enemy : enemies) {
            window.draw(enemy.shape);
        }

        window.display();
    }

    return 0;
}
