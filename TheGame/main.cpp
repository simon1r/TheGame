#include <iostream>
#include <vector>
#include<algorithm>
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
bool dead;

class CustomRectangle : public sf::RectangleShape {//};, public sf::Sprite {
public:
    float time =0;
    CustomRectangle(){}
    CustomRectangle(float sx,float sy ) : sf::RectangleShape(sf::Vector2f(sx,sy)){}
    CustomRectangle(float sx,float sy,sf::Texture &tex ) : sf::RectangleShape(sf::Vector2f(sx,sy))
    {
        setTexture(&tex);
    }
    bool collide(CustomRectangle a) {
        return getGlobalBounds().intersects(a.getGlobalBounds());
    }
    void move(sf::Vector2f m){
        setPosition(getPosition() + m);
    }
    void move(float x, float y){
        setPosition(getPosition() + sf::Vector2(x,y));
    }
};

CustomRectangle m[400];
int sizeX = 30;
int added = 0;

class Player : public CustomRectangle{
public:
    float health = 1;
    sf::Texture tex[5][6];
    float speed = 500;
    Player():CustomRectangle(50,80) {
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 6; x++)
                tex[y][x].loadFromFile("player.png", sf::IntRect(26 * x, 36 * y, 28, 36));
        tex[4][0].loadFromFile("player.png", sf::IntRect(26 * 1, 36 * 0, 28, 36));

        setTexture(&tex[0][0]);
        setPosition(475, 440);
    }
    void update(float t){
        sf::Vector2f movement;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            movement += sf::Vector2f(0, -speed * t);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            movement += sf::Vector2f(0, speed * t);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            movement += sf::Vector2f(-speed * t,0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            movement += sf::Vector2f(speed * t,0);
        tryMove(sf::Vector2f(movement.x,0));
        tryMove(sf::Vector2f(0,movement.y));


        if (movement.x == 0 && movement.y ==0)
            time = 0;
        else time += t;

        int d=4;
        if(movement.x > 0) d = 2;
        if(movement.x < 0) d = 1;
        if(movement.y > 0) d =0 ;
        if(movement.y < 0) d = 3;

        setTexture(&tex[d][(int)(time * 10) % 6]      );


        if (health <= 0) dead = true;
    }
    void tryMove(sf::Vector2f movement){
        move(movement);
        for (int i = 0; i < added; i++)
            if (collide(m[i])) {
                move(-movement);
                break;
            }
    }
};
Player player;
void  RemoveBullet(int i);
int bulletCount;




class Bullet : public CustomRectangle{
public:
    float dmg;
    sf::Vector2f velocity;
    int index;
    Bullet(){};
    Bullet(float sx , float sy,    sf::Vector2f velocity,float dmg,sf::Texture &tex):CustomRectangle(sx,sy)
    {
        //this->tex=tex;
        setTexture(&tex);
        this->velocity = velocity;
        this->dmg = dmg;
    }
    void update(float t){
        move(velocity * t);
        for (int i=0;i<added;i++)
            if (collide(m[i])) {
                RemoveBullet(index);
                return;
            }
        if (collide(player)) {
            player.health -= dmg;
            RemoveBullet(index);
            return;
        }
    }
};
const int maxBullets=100;
Bullet bullets[maxBullets];

void RemoveBullet(int n){
    for (int i=n;i<bulletCount-1;i++) {
        bullets[i] = bullets[i + 1];
        bullets[i].index--;
    }
    if (n<=bulletCount)bulletCount--;
}

class Cannon : public CustomRectangle{
public:
    Bullet bullet;
    sf::Vector2f dir;
    float offset,shootSpeed;
    float rotation;
    Cannon(){}
    Cannon(float sx , float sy, sf::Texture &tex,Bullet &b,float shootSpeed,float offset=0):CustomRectangle(sx,sy)
    {

        this->shootSpeed = shootSpeed;
        this->dir = dir;
        bullet = b;
        this->offset = offset;
        setTexture(&tex);
        while (offset > shootSpeed) offset -= shootSpeed;
        time = offset;




    }
    void rot(float rotation){
        this->rotation = rotation;
        if (rotation==1) move (getSize().x,0);
        if (rotation == 2) move (getSize().x,getSize().y);
        if(rotation ==3) move (0,getSize().y);
        rotate(rotation  * 90);
    }
    void update(float t){
        time += t;
        if(time > shootSpeed) {
            time -= shootSpeed;
            //    sf::Vector2f dir = sf::Vector2f(sf::Mouse::getPosition(window).x , sf::Mouse::getPosition(window).y )+cameraPos - player.getPosition() ;
            //   dir = dir/sqrt(dir.x * dir.x + dir.y * dir.y);


            if (bulletCount < maxBullets) {
                bullets[bulletCount] = bullet;
                bullets[bulletCount].index = bulletCount;
                bullets[bulletCount].velocity = sf::Vector2f( bullet.velocity.x * cos(rotation* 6.28/4) - bullet.velocity.y * sin(rotation* 6.28/4) ,bullet.velocity.x * sin(rotation* 6.28/4) + bullet.velocity.y * cos(rotation* 6.28/4)) ;

                sf::Vector2f m;
                if ( rotation == 0 ) m = getSize() / 2.0f - bullet.getSize() / 2.0f - sf::Vector2f(0,getSize().y/2.0);
                else if ( rotation == 1 ) m = -getSize() / 2.0f - bullet.getSize() / 2.0f + sf::Vector2f(getSize().x/2.0,getSize().y);
                else if ( rotation == 2 ) m = -getSize() / 2.0f - bullet.getSize() / 2.0f + sf::Vector2f(0,getSize().y/2.0);
                else if ( rotation == 3 ) m = -getSize() / 2.0f - bullet.getSize() / 2.0f + sf::Vector2f(getSize().x/2.0,0);
                //   else if ( rotation == 1)
                bullets[bulletCount].setPosition(getPosition() + m);
                bulletCount++;
            }
        }
    }
};

class Enemy : CustomRectangle{
public:
    sf::Vector2f a,b;
    float speed;
    float dmg;
    Enemy(float sx , float sy, sf::Texture &tex):CustomRectangle(sx,sy)
    {

        setTexture(&tex);
    }

};

class Saw : public CustomRectangle{
public:
    sf::Vector2f start,dislocation;
    float speed;
    float dmg;
    Saw(){}
    Saw(float sx , float sy, sf::Texture &tex,float dmg=0,float speed=0,float offset=0):CustomRectangle(sx,sy)
    {
        this->dmg = dmg;
        this->speed = speed;
        setTexture(&tex);
        time = offset;
    }
    void update(float t){
        time += t;
        while (time > speed) time -=   speed;
        if ( time  < speed / 2.f)setPosition(start + dislocation * (time / speed) * 2.f);
        else setPosition(start + dislocation * ( (speed-time) / speed) *2.0f);
        if(collide(player)) player.health -= dmg * t;
    }
};

int main() {


    sf::Texture brick,background,bulletTex,cannon,defeatTex;
    brick.loadFromFile("brick.jpg");
    bulletTex.loadFromFile("bullet.png");
    cannon.loadFromFile("cannon.png");
    background.loadFromFile("bg.jpg");
    defeatTex.loadFromFile("def.png");
    background.setRepeated(true);
    CustomRectangle bg(3000,3000,background);
    CustomRectangle defeat(1000,500,defeatTex);
    defeat.setFillColor(sf::Color(200,20,50));
    defeat.setPosition(0,160);
    //bg.setScale(0.1,0.1);
    bg.setTextureRect({ 0, 0, 7000, 7000 });

    for (int x = 0; x < sizeX; x++) {
        for (int y = 0; y < sizeX; y++) {
            if (x == 0 || y == 0 || y == sizeX - 1 || x == sizeX - 1 || (x == 15 && y < 10)
                || y==15 && x <  sizeX - 5 || y > 17 && x == sizeX - 13 || y < sizeX - 4 && y > 15 && x == sizeX - 8  || y< 15 && y > 5 && x < sizeX - 5 && x > sizeX - 8) {

                CustomRectangle c = CustomRectangle(100, 100);
                c.setTexture(&brick);
                c.setFillColor(sf::Color(255, 0, 0));
                c.setPosition(x * 100, y * 100);
                m[added] = c;
                added++;
            }
        }
    }


    int cannonCount = 8;
    Cannon cannons[cannonCount];
    Bullet cannonBullet(50,50,sf::Vector2f(0,-100),0.2,bulletTex);
    for(int i=0;i<4;i++) {
        Cannon c(210/3.0, 260/3.0, cannon, cannonBullet ,1,i);
        c.setPosition(400 * i + 400, 300);
        cannons[i] = c;
    }

    for(int i=0;i<4;i++) {
        Cannon c(210/3.0, 260/3.0, cannon, cannonBullet ,1,i);
        c.setPosition(400 * i + 400, 300);
        c.rot(i);
        cannons[i+4] = c;
    }

    sf::Texture sawTex;
    sawTex.loadFromFile("saw.png");
    int sawCount = 8;
    Saw saws[sawCount];
    for(int i=0;i<sawCount;i++) {
        Saw s(210/3.0, 260/3.0,sawTex,0.5,3.14,i);
        s.start = sf::Vector2f(100*i,400);
        s.dislocation = sf::Vector2f(0,400);
        saws[i] = s;
    }

    sf::Clock clock;
    bool shoot;
    while (window.isOpen()) {
        float t = clock.restart().asSeconds();

        sf::Event event;

        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }
        if(!dead){
            if( sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
                if(!shoot &&bulletCount < maxBullets) {
                    shoot = true;
                    sf::Vector2f dir = sf::Vector2f(sf::Mouse::getPosition(window).x , sf::Mouse::getPosition(window).y )+cameraPos - player.getPosition() ;
                    dir = dir/sqrt(dir.x * dir.x + dir.y * dir.y);
                    bullets[bulletCount] =  Bullet(40,40,dir * 500.f,10,bulletTex);
                    bullets[bulletCount].index = bulletCount;
                    bullets[bulletCount].setPosition(player.getPosition());
                    bulletCount++;
                }}
            else shoot = false;


            for (int i = 0; i < bulletCount; i++)bullets[i].update(t);

            player.update(t);

            sf::Vector2f dis =  player.getPosition() - cameraPos - sf::Vector2f (500,500);
            cameraPos += dis * (5* t) ;


            for (int i = 0; i < cannonCount; i++)  cannons[i].update(t);
            for (int i = 0; i < sawCount; i++)  saws[i].update(t);
            window.clear();

        }
        sf::View view(sf::FloatRect(cameraPos.x, cameraPos.y, 1000.f, 1000.0));
        window.setView(view);

        window.draw(bg);

        for (int i = 0; i < added; i++)
            window.draw(m[i]);


        for (int i = 0; i < cannonCount; i++) window.draw(cannons[i]);
        for (int i = 0; i < sawCount; i++) window.draw(saws[i]);


        for (int i = 0; i < bulletCount; i++) {
            window.draw(bullets[i]);
        }




        window.draw(player);
        if(dead){
            sf::View view(sf::FloatRect(0,0, 1000.f, 1000.0));
            window.setView(view);
            window.draw(defeat);
        }
        window.display();
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}



