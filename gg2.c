#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define INITIAL_SCREEN_WIDTH 800
#define INITIAL_SCREEN_HEIGHT 600
#define PLAYER_SIZE 40
#define ENEMY_SIZE 30
#define CIRCLE_RADIUS 50
#define RED_DOT_RADIUS 5
#define ENEMY_COUNT 10
#define SPEED 200
#define ENEMY_SPEED 100
#define PI 3.14159265358979323846

typedef struct {
    float x, y;
    int size;
    bool alive;
} Entity;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* backgroundTexture;
SDL_Texture* playerTexture;
SDL_Texture* enemyTexture;
TTF_Font* font;

int SCREEN_WIDTH = INITIAL_SCREEN_WIDTH;
int SCREEN_HEIGHT = INITIAL_SCREEN_HEIGHT;
int score=0;

void spawnEnemy(Entity* enemy) {
    enemy->x = rand() % (SCREEN_WIDTH - ENEMY_SIZE);
    enemy->y = rand() % (SCREEN_HEIGHT - ENEMY_SIZE);
    enemy->size = ENEMY_SIZE;
    enemy->alive = true;
}

bool checkCollision(float x1, float y1, int size1, float x2, float y2, int size2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (size1 / 2 + size2 / 2);
}

void scalePositionsAndSizes(float scaleX, float scaleY, Entity* player, Entity enemies[], int enemyCount) {
    // playerPos
    player->x *= scaleX;
    player->y *= scaleY;

    // enemyPos
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].alive) {
            enemies[i].x *= scaleX;
            enemies[i].y *= scaleY;
        }
    }
}

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("GameProj-02",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    
    if (!window) {
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    //switch to software rendering
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    backgroundTexture = IMG_LoadTexture(renderer, "./assets/images/bg.png");
    playerTexture = IMG_LoadTexture(renderer, "./assets/images/player.png");
    enemyTexture = IMG_LoadTexture(renderer, "./assets/images/enemy.png");
    font= TTF_OpenFont("./assets/fonts/VCR_OSD_MONO.ttf",27);
    
    srand(time(NULL));
}

void cleanup() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(enemyTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void renderBackground() {
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
}

void render(Entity* player, Entity enemies[], float redDotX, float redDotY) {
    SDL_RenderClear(renderer);
    renderBackground();

    // Render player
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect playerRect = { (int)player->x, (int)player->y, PLAYER_SIZE, PLAYER_SIZE };
    SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);

    // Render enemies
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (enemies[i].alive) {
            SDL_Rect enemyRect = { (int)enemies[i].x, (int)enemies[i].y, enemies[i].size, enemies[i].size };
            SDL_RenderCopy(renderer, enemyTexture, NULL, &enemyRect);
        }
    }

    // Render revolving red dot
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect redDotRect = { (int)redDotX - RED_DOT_RADIUS, (int)redDotY - RED_DOT_RADIUS, RED_DOT_RADIUS * 2, RED_DOT_RADIUS * 2 };
    SDL_RenderFillRect(renderer, &redDotRect);

    char scoretxt[30];
    sprintf(scoretxt, "Score: %d", score);

    SDL_Color color= {255, 255, 255, 255};
    SDL_Surface* textSurface= TTF_RenderText_Solid(font, scoretxt, color);
    SDL_Texture* textTexture= SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect= {10,20, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    init();

    bool running = true;
    bool isFullScreen = false;
    Uint32 lastTime = SDL_GetTicks();
    Entity player = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, PLAYER_SIZE, true };
    Entity enemies[ENEMY_COUNT];
    enemies->alive=false;
    

    float angle = 0.0f;

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_f) {
                    // Toggle fullscreen
                    isFullScreen = !isFullScreen;
                    if (isFullScreen) {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    } else {
                        SDL_SetWindowFullscreen(window, 0);
                        SDL_SetWindowSize(window, INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT);
                    }
                }
            } else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int width, height;
                    SDL_GetWindowSize(window, &width, &height);

                    // Calculate scale factors
                    float scaleX = (float)width / SCREEN_WIDTH;
                    float scaleY = (float)height / SCREEN_HEIGHT;

                    // Scale positions of player and enemies
                    scalePositionsAndSizes(scaleX, scaleY, &player, enemies, ENEMY_COUNT);

                    // Update SCREEN_WIDTH and SCREEN_HEIGHT
                    SCREEN_WIDTH = width;
                    SCREEN_HEIGHT = height;
                }
            }
        }

        // Player movement
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP]) {
            player.y -= SPEED * deltaTime;
        }
        if (state[SDL_SCANCODE_DOWN]) {
            player.y += SPEED * deltaTime;
        }
        if (state[SDL_SCANCODE_LEFT]) {
            player.x -= SPEED * deltaTime;
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            player.x += SPEED * deltaTime;
        }

        // Keep player in bounds
        if (player.x < 0) player.x = 0;
        if (player.x > SCREEN_WIDTH - PLAYER_SIZE) player.x = SCREEN_WIDTH - PLAYER_SIZE;
        if (player.y < 0) player.y = 0;
        if (player.y > SCREEN_HEIGHT - PLAYER_SIZE) player.y = SCREEN_HEIGHT - PLAYER_SIZE;

        // Revolving red dot
        angle += 2.0f * PI * deltaTime;  // Update angle
        float redDotX = player.x + PLAYER_SIZE / 2 + CIRCLE_RADIUS * cos(angle);
        float redDotY = player.y + PLAYER_SIZE / 2 + CIRCLE_RADIUS * sin(angle);

        // Check collisions
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (enemies[i].alive && checkCollision(redDotX, redDotY, RED_DOT_RADIUS * 2, 
                enemies[i].x + enemies[i].size / 2, enemies[i].y + enemies[i].size / 2, enemies[i].size)) {
                enemies[i].alive = false;
                score++;
            }
        }
        
        if (enemies[0].alive==false && enemies[1].alive==false && enemies[2].alive==false && enemies[3].alive==false && enemies[4].alive==false && enemies[5].alive==false && enemies[6].alive==false && enemies[7].alive==false && enemies[8].alive==false && enemies[9].alive==false){
            for (int i = 0; i < ENEMY_COUNT; i++) {
                spawnEnemy(&enemies[i]);
            }
        }

        // Enemy chase player
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (enemies[i].alive) {
                // Calculate direction vector from enemy to player
                float dx = player.x - enemies[i].x;
                float dy = player.y - enemies[i].y;
                
                // Calculate distance and normalize the direction vector
                float distance = sqrt(dx * dx + dy * dy);
                if (distance != 0) {
                    dx /= distance;
                    dy /= distance;
                }

                // Update enemy position toward the player
                enemies[i].x += dx * ENEMY_SPEED * deltaTime;
                enemies[i].y += dy * ENEMY_SPEED * deltaTime;
            }
        }

        
        
        render(&player, enemies, redDotX, redDotY);
    }

    cleanup();
    return 0;
}
