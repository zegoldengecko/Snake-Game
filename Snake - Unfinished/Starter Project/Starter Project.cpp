// Starter Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "raylib.h"
#include <deque>
#include <raymath.h>

using namespace std;

//defining colours
Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };


//establishing cell sizes
float CellSize = 30;
int CellCount = 25;
float offset = 75;

double lastUpdateTime = 0;


//checking if interval has passed
bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool SpawnInSnake(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}
//The snake
class Snake {
public:
    deque<Vector2> body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
    Vector2 direction = { 1,0 };
    bool addSegment = false;
    //head image
    Image head = LoadImage("C:/Users/Alex/OneDrive/Documents/C++ Projects/Starter Project/Starter Project/head.png");
    Texture2D headtexture = LoadTextureFromImage(head);

    //Drawing the Snake
    void Draw() {
        for (unsigned int i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * CellSize, offset + y * CellSize, CellSize, CellSize };
            if (i == 0) {
                DrawTexture(headtexture, offset + x * CellSize, offset + y * CellSize, WHITE);
            }
            else {
                DrawRectangleRounded(segment, 0.5, 6, darkGreen);
            }
            
        }
    }

    //updating the snake position + adding segments
    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true) {
            addSegment = false;
        }
        else {
            body.pop_back();
            
        }
    }
    //Rest snake to standard position at game end
    void Reset() {
        body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
        direction = { 1, 0 };
    }
};

//designing the food
class Food {
public:
    Vector2 position = { 5,6 };
    Texture2D texture;

    //food call and location assignment
    Food(deque<Vector2> snakeBody) {
        
        Image image = LoadImage("C:/Users/Alex/OneDrive/Documents/C++ Projects/Starter Project/Starter Project/Food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    //unloading??
    ~Food() {
        UnloadTexture(texture);
    }

    //drawing it
    void Draw() {
        DrawTexture(texture, offset + position.x * CellSize, offset + position.y * CellSize, WHITE);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, CellCount - 1);
        float y = GetRandomValue(0, CellCount - 1);
        return Vector2{ x, y };
    }

    //assigning position
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomCell();
        while (SpawnInSnake(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
};

//main Game function, updating/displaying stuff
class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;

    //Getting sounds
    Sound eatSound;
    Sound deathSound;

    Game() {
        InitAudioDevice();
        eatSound = LoadSound("C:/Users/Alex/OneDrive/Documents/C++ Projects/Starter Project/Starter Project/Eat.mp3");
        deathSound = LoadSound("C:/Users/Alex/OneDrive/Documents/C++ Projects/Starter Project/Starter Project/Die.mp3");
    }

    ~Game() {
        UnloadSound(eatSound);
        UnloadSound(deathSound);
        CloseAudioDevice();
    }

    //Drawing food and snake
    void Draw() {
        food.Draw();
        snake.Draw();

    }

    //updating position
    void Update() {
        if (running) {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithtail();
        }
 
    }

    //checking if snake eats food
    void CheckCollisionWithFood() {
        if (Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    //checking if it collides with edges
    void CheckCollisionWithEdges() {
        if (snake.body[0].x == CellCount || snake.body[0].x == -1) {
            GameOver();
        }
        if (snake.body[0].y == CellCount || snake.body[0].y == -1) {
            GameOver();
        }
    }

    //checking if it collides with tail
    void CheckCollisionWithtail() {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (SpawnInSnake(snake.body[0], headlessBody)) {
            GameOver();
        }
    }

    //GameOver section
    void GameOver() {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        deathanimation();
        running = false;
        score = 0;
        PlaySound(deathSound);

    }
    void deathanimation() {
        Vector2 deathdirection = { -1, -1 };
        for (unsigned int i = 0; i < 5; i++) {
            for (unsigned int j = 0; j < snake.body.size(); j++) {
                snake.body.push_front(Vector2Add(snake.body[i], deathdirection));
                snake.body.pop_back();

            }       
        }
    }
};
int main(){
    InitWindow(2*offset + CellSize*CellCount, 2*offset + CellSize * CellCount, "Play Snake");
    SetTargetFPS(60);

    Game game = Game();

    while (WindowShouldClose() == false) {
        //setup functions
        BeginDrawing();

        if (eventTriggered(0.1)) {
            game.Update();
        }

        //Assigning the Direction
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 ) {
            game.snake.direction = { 0, -1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = {-1, 0 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
            game.snake.direction = { 1, 0 };
            game.running = true;
        }


        //Drawing, previous frame's graphics are wiped clean
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{offset-5, offset-5, CellSize*CellCount+10, CellSize*CellCount + 10}, 5, BLACK);
        DrawText("Snake", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset + 710, 20, 40, BLACK);
        game.Draw();


        EndDrawing();


    }

    CloseWindow();
    return 0;
}
