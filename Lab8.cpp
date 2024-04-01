// Lab8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    World world(Vector2f(0, 0)); //no pulling down gravity
    int score(0);
    int arrows(5);
    int fontSz(48);
    Font font;
    if (!font.loadFromFile("arial1.ttf")) {
        cout << "could not load arial1.ttf" << endl;
        exit(0);
    }

    PhysicsSprite crossBow;
    Texture cbowTex;
    LoadTex(cbowTex, "Lab08Images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "Lab08Images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, -5));
    top.setStatic(true);
    top.onCollision = [&arrow, &arrows, &world, &drawingArrow](PhysicsBodyCollisionResult res) {
        arrows--;
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };
    world.AddPhysicsBody(top);

    //ducks
    PhysicsSprite duck;
    Texture duckTex;
    LoadTex(duckTex, "Lab08Images/bird.png");
    duck.setTexture(duckTex);
    duck.setSize(Vector2f(100,100));
    PhysicsShapeList<PhysicsSprite> ducks;

    //duck wall
    PhysicsRectangle rightWall;
    rightWall.setSize(Vector2f(10, 600));
    rightWall.setCenter(Vector2f(950, 400)); //just over the right side of the viewport
    rightWall.setStatic(true);
    world.AddPhysicsBody(rightWall);


    //text
    Text arrowText;
    arrowText.setFont(font);
    arrowText.setCharacterSize(fontSz);

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(fontSz);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time lastDuckTime(clock.getElapsedTime());

    while (arrows > 0) {
        Time currentTime(clock.getElapsedTime());
        int deltaTime((currentTime - lastTime).asMilliseconds());

        if (deltaTime > 2) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaTime);

            if (Keyboard::isKeyPressed(Keyboard::Space) && !drawingArrow) {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1.2));
                world.AddPhysicsBody(arrow);
            }

            window.clear();
            window.draw(crossBow);
            if (drawingArrow) {
                window.draw(arrow);
            }
            //arrows text
            arrowText.setString(to_string(arrows));
            FloatRect arrowSz(arrowText.getGlobalBounds());
            arrowText.setPosition(Vector2f(10, 540));
            window.draw(arrowText);
            //score text
            scoreText.setString(to_string(score));
            FloatRect scoreSz(scoreText.getGlobalBounds());
            scoreText.setPosition(Vector2f(765, 540));
            window.draw(scoreText);

            //drawing ducks
            Time currentDuckTime(clock.getElapsedTime());
            if ((currentDuckTime - lastDuckTime).asMilliseconds() >= 600) {
                lastDuckTime = currentDuckTime;

                PhysicsSprite& newDuck = ducks.Create();
                newDuck.setTexture(duckTex);
                newDuck.setScale(Vector2f(1, 1));
                newDuck.setCenter(Vector2f(0, 120 - (newDuck.getGlobalBounds().height / 2)));
                newDuck.setVelocity(Vector2f(0.5, 0));
                world.AddPhysicsBody(newDuck);
                newDuck.onCollision = [&world, &arrow, &arrows, &drawingArrow, &newDuck, &ducks, &score, &rightWall](PhysicsBodyCollisionResult res) {
                    if (res.object2 == arrow) {
                        drawingArrow = false;
                        arrows--;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(newDuck);
                        ducks.QueueRemove(newDuck);
                        score += 10;
                    }
                    if (res.object2 == rightWall) {
                        world.RemovePhysicsBody(newDuck);
                        ducks.QueueRemove(newDuck);
                    }

                    };
            }
            for (auto duck : ducks) {
                window.draw(duck);
            }
            window.draw(rightWall); //wall to remove ducks from ducks

            window.display();
            ducks.DoRemovals();
        }


    }

    //end game screen
    bool exitGame(false);
    

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(fontSz);
    gameOverText.setFillColor(Color(255, 0, 0));
    FloatRect goSz = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(400 - (goSz.width / 2), 200 - (goSz.height)));

    Text endScoreText;
    endScoreText.setFont(font);
    endScoreText.setString("You scored: " + to_string(score) + " points!");
    endScoreText.setCharacterSize(fontSz);
    FloatRect estSz = endScoreText.getGlobalBounds();
    endScoreText.setPosition(Vector2f(400 - (estSz.width / 2), 300 - (estSz.height)));

    Text leaveText;
    leaveText.setFont(font);
    leaveText.setString("Press SPACE to exit game");
    leaveText.setCharacterSize(fontSz * 0.75);
    FloatRect lSz = leaveText.getGlobalBounds();
    leaveText.setPosition(Vector2f(400 - (lSz.width / 2), 400 - (lSz.height)));

    while (!exitGame) {
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            exitGame = true;
        }
        window.clear();
        
        window.draw(gameOverText);
        window.draw(endScoreText);
        window.draw(leaveText);
        window.display();
    }


}