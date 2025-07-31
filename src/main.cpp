#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>

sf::RectangleShape createPlatforms(sf::Vector2f size, sf::Vector2f position, sf::Color color) {
    sf::RectangleShape platform (size);
    platform.setFillColor(color);
    platform.setPosition(position);
    return platform;
}

sf::Sprite createSupermanPickup (std::string& filePathName, sf::Vector2f position, sf::Vector2f scale) {
    sf::Texture supermanLogoTexture (filePathName);
    sf::Sprite supermanLogoSprite (supermanLogoTexture);
    supermanLogoSprite.setPosition(position);
    supermanLogoSprite.setScale(scale);
    return supermanLogoSprite;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "Multiplayer Platformer");

    sf::RectangleShape ground ({1920, 200});
    ground.setPosition({0, 600});
    ground.setFillColor(sf::Color::Black);
    sf::FloatRect groundCollisionBox = ground.getGlobalBounds();

    sf::Texture playerTexture ("assets/supermanSprite.png");
    sf::Sprite playerSprite (playerTexture);
    playerSprite.setScale({0.2, 0.2});
    sf::Vector2f playerSpritePosition = {0, groundCollisionBox.position.y - playerSprite.getGlobalBounds().size.y};
    playerSprite.setPosition(playerSpritePosition);
    float playerSpeed = 200.f;
    float gravity = 1000.f;
    bool isOnGround = true;
    float jumpStrength = 600.f;
    float velocityY = 0.f;

    sf::Clock clock;
    float totalTime = 0;

    std::vector<sf::RectangleShape> platforms;
    platforms.push_back(createPlatforms({200,50}, {200,450}, sf::Color::Black));
    platforms.push_back(createPlatforms({200, 50}, {500, 400}, sf::Color::Black));
    platforms.push_back(createPlatforms({200, 50}, {800, 450}, sf::Color::Black));

    std::vector<sf::Sprite> supermanPickups;
    std::string filePathName = "assets/SupermanLogo.png";
    sf::Texture supermanLogoTexture (filePathName);
    supermanPickups.push_back(createSupermanPickup(filePathName,{200, 350}, {0.2,0.2}));
    supermanPickups.push_back(createSupermanPickup(filePathName,{500, 400}, {0.2,0.2}));
    for (auto & supermanPickup : supermanPickups) {
        supermanPickup.setTexture(supermanLogoTexture);
    }

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        totalTime += deltaTime;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            playerSpritePosition.x -= playerSpeed * deltaTime;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            playerSpritePosition.x += playerSpeed * deltaTime;
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && isOnGround) {
            velocityY = -jumpStrength;
            isOnGround = false;
        }
        velocityY += gravity * deltaTime;
        playerSpritePosition.y += velocityY * deltaTime;
        if (playerSpritePosition.y >= groundCollisionBox.position.y - playerSprite.getGlobalBounds().size.y) {
            playerSpritePosition.y = groundCollisionBox.position.y - playerSprite.getGlobalBounds().size.y;
            isOnGround = true;
            velocityY = 0.f;
        }

        for (auto &platform : platforms) {
            sf::FloatRect platformCollisionBox = platform.getGlobalBounds();
            sf::FloatRect playerCollisionBox = playerSprite.getGlobalBounds();
            playerCollisionBox.size.x = platform.getGlobalBounds().size.x * 0.5f;

            if (const std::optional intersection = platformCollisionBox.findIntersection(playerCollisionBox)) {
                if (velocityY > 0 && playerSpritePosition.y + playerCollisionBox.size.y <= platform.getPosition().y + 10) {
                    playerSpritePosition.y = platformCollisionBox.position.y - playerSprite.getGlobalBounds().size.y;
                    isOnGround = true;
                    velocityY = 0.f;
                }
            }
        }

        if (playerSpritePosition.x <= 0) {
            playerSpritePosition.x = 0;
        } else if (playerSpritePosition.x >= 1350) {
            playerSpritePosition.x = 1350;
        }

        playerSprite.setPosition(playerSpritePosition);

        for (auto& supermanPickup : supermanPickups) {
            float pickupPosX = supermanPickup.getPosition().x;
            float bounceY = 320.f + std::sin(totalTime * 4.5) * 50.f;
            supermanPickup.setPosition({pickupPosX, bounceY});
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear(sf::Color::Blue);
        window.draw(ground);
        window.draw(playerSprite);
        for (auto &platform : platforms) {
            window.draw(platform);
        }
        for (auto &supermanPickup : supermanPickups) {
            window.draw(supermanPickup);
        }
        window.display();
    }
}
