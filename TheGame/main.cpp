#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>
#include <sstream>
#include <bits/stdc++.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>

using namespace std;

sf::RenderWindow window(sf::VideoMode(1000, 1000), "Dungeon Scroller");
sf::Vector2f cameraPos;

class CustomRectangle : public sf::RectangleShape {
public:
    float time = 0;

    CustomRectangle() {}
    CustomRectangle(float sx, float sy) : sf::RectangleShape(sf::Vector2f(sx, sy)) {}
    CustomRectangle(float sx, float sy, sf::Texture& tex) : sf::RectangleShape(sf::Vector2f(sx, sy)) {
        setTexture(&tex);
    }

    bool collide(CustomRectangle a) {
        return getGlobalBounds().intersects(a.getGlobalBounds());
    }

    void move(sf::Vector2f m) {
        setPosition(getPosition() + m);
    }
};

CustomRectangle m[400];
int sizeX = 30;
int added = 0;

class Player : public CustomRectangle {
public:
    sf::Texture tex[5][6];
    float speed = 500;

    Player() : CustomRectangle(50, 80) {
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 6; x++)
                tex[y][x].loadFromFile("player.png", sf::IntRect(26 * x, 36 * y, 28, 36));
        tex[4][0].loadFromFile("player.png", sf::IntRect(26 * 1, 36 * 0, 28, 36));

        setTexture(&tex[0][0]);
        setPosition(475, 440);
    }

    void update(float t) {
        sf::Vector2f movement;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            movement += sf::Vector2f(0, -speed * t);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            movement += sf::Vector2f(0, speed * t);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            movement += sf::Vector2f(-speed * t, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            movement += sf::Vector2f(speed * t, 0);
        tryMove(sf::Vector2f(movement.x, 0));
        tryMove(sf::Vector2f(0, movement.y));

        if (movement.x == 0 && movement.y == 0)
            time = 0;
        else
            time += t;

        int d = 4;
        if (movement.x > 0)
            d = 2;
        if (movement.x < 0)
            d = 1;
        if (movement.y > 0)
            d = 0;
        if (movement.y < 0)
            d = 3;

        setTexture(&tex[d][(int)(time * 10) % 6]);
    }

    void tryMove(sf::Vector2f movement) {
        move(movement);
        for (int i = 0; i < added; i++)
            if (collide(m[i])) {
                move(-movement);
                break;
            }
    }
};

Player player;
void RemoveBullet(int i);
int bulletCount;

class Bullet : public CustomRectangle {
public:
    float dmg;
    sf::Vector2f velocity;
    int index;

    Bullet() {}
    Bullet(float sx, float sy, sf::Vector2f velocity, float dmg, sf::Texture& tex)
        : CustomRectangle(sx, sy) {
        setTexture(&tex);
        this->velocity = velocity;
        this->dmg = dmg;
    }

    void update(float t) {
        move(velocity * t);
        for (int i = 0; i < added; i++)
            if (collide(m[i])) {
                RemoveBullet(index);
                return;
            }
    }
};

const int maxBullets = 100;
Bullet bullets[maxBullets];

void RemoveBullet(int n) {
    for (int i = n; i < bulletCount - 1; i++) {
        bullets[i] = bullets[i + 1];
        bullets[i].index--;
    }
    if (n <= bulletCount)
        bulletCount--;
}

class Cannon : public CustomRectangle {
public:
    Bullet bullet;
    sf::Vector2f dir;
    float offset;

    Cannon(float sx, float sy, sf::Texture& tex, Bullet b, sf::Vector2f dir, float offset = 0)
        : CustomRectangle(sx, sy) {
        this->dir = dir;
        bullet = b;
        this->offset = offset;
        setTexture(&tex);
    }

    void update(float t) {
        time += t;
        if (time + offset > 3) {
            time = 0;
            bullets[bulletCount] = bullet;
            bullets[bulletCount].index = bulletCount;
            bullets[bulletCount].velocity = dir;
            bullets[bulletCount].setPosition(getPosition());
            bulletCount++;
        }
    }
};

int main() {
    sf::Texture brick, background, bulletTex, cannon;
    brick.loadFromFile("brick.jpg");
    bulletTex.loadFromFile("bullet.png");
    cannon.loadFromFile("cannon.png");
    background.loadFromFile("bg.jpg");
    background.setRepeated(true);
    CustomRectangle bg(3000, 3000, background);

    for (int x = 0; x < sizeX; x++) {
        for (int y = 0; y < sizeX; y++) {
            if (x == 0 || y == 0 || y == sizeX - 1 || x == sizeX - 1 || (x == 15 && y < 10) ||
                (y == 15 && x < sizeX - 5) || (y > 17 && x == sizeX - 13) ||
                (y < sizeX - 4 && y > 15 && x == sizeX - 8) || (y < 15 && y > 5 && x < sizeX - 5 && x > sizeX - 13)) {

                CustomRectangle c = CustomRectangle(100, 100);
                c.setTexture(&brick);
                c.setFillColor(sf::Color(255, 255, 255, 100));
                c.setPosition(x * 100, y * 100);
                m[added] = c;
                added++;
            }
        }
    }

    sf::View view(sf::FloatRect(0, 0, 1000, 1000));
    window.setView(view);

    sf::Clock clock;
    float time = clock.restart().asSeconds();

    while (window.isOpen()) {
        float t = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        player.update(t);
        for (int i = 0; i < bulletCount; i++)
            bullets[i].update(t);

        view.setCenter(player.getPosition() + sf::Vector2f(0, -300));
        window.setView(view);

        window.clear();
        window.draw(bg);
        window.draw(player);
        for (int i = 0; i < added; i++)
            window.draw(m[i]);
        for (int i = 0; i < bulletCount; i++)
            window.draw(bullets[i]);
        window.display();
    }

    return 0;
}
