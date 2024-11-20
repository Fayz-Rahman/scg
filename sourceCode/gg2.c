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
#define PLAYER_MAX_HP 15
#define ENEMY_SIZE 30
#define CIRCLE_RADIUS 50
#define RED_DOT_RADIUS 5
#define ENEMY_COUNT 10
#define SPEED 200
#define INITIAL_ENEMY_SPEED 80.00
#define PI 3.14159265358979323846
#define MAX_BULLETS 9

typedef struct {
    float x, y;
    int size;
    bool alive;
    int hp;  
} Entity;

typedef struct {
    float x, y;
    float angle;
    bool active;
} Bullet;


SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* backgroundTexture;
SDL_Texture* menuBackgroundTexture;
SDL_Texture* playerTexture;
SDL_Texture* enemyTexture;
TTF_Font* font;
TTF_Font* gameOverFont;


int SCREEN_WIDTH = INITIAL_SCREEN_WIDTH;
int SCREEN_HEIGHT = INITIAL_SCREEN_HEIGHT;
int score = 0;
int playerHP = 10;
int ENEMY_SPEED = INITIAL_ENEMY_SPEED;
bool running = true;
bool isFullScreen = false;
bool gameOver = false;
Bullet bullets[MAX_BULLETS];

int loadHighScore();

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

void shootBullets(float x, float y, float targetX, float targetY, int numBullets) {
    float baseAngle = atan2(targetY - y, targetX - x);
    float spread = 0.3f;

    if(score >= 30){
        for (int i = 0; i < numBullets; i++) {
            for (int j = 0; j < MAX_BULLETS; j++) {
                if (!bullets[j].active) {
                    float angleOffset = (i - (numBullets - 1) / 2.0f) * spread;
                    bullets[j].x = x;
                    bullets[j].y = y;
                    bullets[j].angle = baseAngle + angleOffset;
                    bullets[j].active = true;
                    break;
                }
            }
        }
    }
}

void updateBullets(float deltaTime) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            float speed = 300.0f; 
            bullets[i].x += cos(bullets[i].angle) * speed * deltaTime;
            bullets[i].y += sin(bullets[i].angle) * speed * deltaTime;

            if (bullets[i].x < 0 || bullets[i].x > SCREEN_WIDTH ||
                bullets[i].y < 0 || bullets[i].y > SCREEN_HEIGHT) {
                bullets[i].active = false;
            }
        }
    }
}

void renderBullets() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            SDL_Rect bulletRect = { (int)bullets[i].x - 5, (int)bullets[i].y - 5, 10, 10 };
            SDL_RenderFillRect(renderer, &bulletRect);
        }
    }
}

void checkBulletCollisions(Entity enemies[]) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < ENEMY_COUNT; j++) {
                if (enemies[j].alive && checkCollision(
                        bullets[i].x, bullets[i].y, 10, // Bullet size
                        enemies[j].x + enemies[j].size / 2, 
                        enemies[j].y + enemies[j].size / 2, 
                        enemies[j].size)) {
                    bullets[i].active = false;
                    enemies[j].alive = false;
                    score++;
                    ENEMY_SPEED += 1.5;
                    break;
                }
            }
        }
    }
}

int calculateNumBullets() {
    if (score < 60) return 1;
    if (score < 100) return 2;
    return 3; // Cap at 3 bullets
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

    menuBackgroundTexture = IMG_LoadTexture(renderer, "./assets/images/mmbg.png");
    backgroundTexture = IMG_LoadTexture(renderer, "./assets/images/bg.png");
    playerTexture = IMG_LoadTexture(renderer, "./assets/images/player.png");
    enemyTexture = IMG_LoadTexture(renderer, "./assets/images/enemy.png");
    font = TTF_OpenFont("./assets/fonts/VCR_OSD_MONO.ttf",24);
    gameOverFont = TTF_OpenFont("./assets/fonts/VCR_OSD_MONO.ttf",100);
    
    srand(time(NULL));
}

void cleanup() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(menuBackgroundTexture);
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

void renderHPBar(Entity* player) {
    int barWidth = 200;  
    int barHeight = 20;  

    int x = (SCREEN_WIDTH - barWidth) / 2; 
    int y = SCREEN_HEIGHT - barHeight - 10;

    float hpPercentage = (float)player->hp / PLAYER_MAX_HP;
    int currentBarWidth = (int)(barWidth * hpPercentage);

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);  // Gray 
    SDL_Rect backgroundRect = { x, y, barWidth, barHeight };
    SDL_RenderFillRect(renderer, &backgroundRect);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red 
    SDL_Rect hpRect = { x, y, currentBarWidth, barHeight };
    SDL_RenderFillRect(renderer, &hpRect);
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
 
    renderHPBar(player);

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

    
    int highScore = loadHighScore();

    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", highScore);
    SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText, (SDL_Color){255, 255, 255, 255});
    SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
        
    SDL_Rect highScoreRect = {10, 70, highScoreSurface->w, highScoreSurface->h};
    SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);

    SDL_FreeSurface(highScoreSurface);
    SDL_DestroyTexture(highScoreTexture);
    renderBullets();

    SDL_RenderPresent(renderer);
}

void renderGameOverText() {
    const char* gameOverText = "Game Over";
    SDL_Color color = { 255, 0, 0, 255 };
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(gameOverFont, gameOverText, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect;
    textRect.x = (SCREEN_WIDTH - textSurface->w) / 2;
    textRect.y = (SCREEN_HEIGHT - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void saveHighScore(int score) {
    FILE* file = fopen("highscore.dat", "wb");
    if (file) {
        fwrite(&score, sizeof(int), 1, file);
        fclose(file);
    }
}

int loadHighScore() {
    FILE* file = fopen("highscore.dat", "rb");
    int highScore = 0;
    if (file) {
        fread(&highScore, sizeof(int), 1, file);  // Read the score from the file
        fclose(file);
    }
    return highScore;
}

void renderButton( SDL_Rect button, const char* label) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &button);

    SDL_Color color = { 255, 255, 255, 255 }; 
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, label, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect;
    textRect.x = button.x + (button.w - textSurface->w) / 2;
    textRect.y = button.y + (button.h - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void updateButtonPositions(SDL_Rect* startButton, SDL_Rect* optionButton) {
    int buttonWidth = SCREEN_WIDTH / 4;
    int buttonHeight = SCREEN_HEIGHT / 10;
    
    *startButton = (SDL_Rect){
        .x = SCREEN_WIDTH / 2 - buttonWidth / 2,
        .y = SCREEN_HEIGHT / 2 - buttonHeight / 2, // -10 instead of /2
        .w = buttonWidth,
        .h = buttonHeight
    };
    
    // *optionButton = (SDL_Rect){
    //     .x = SCREEN_WIDTH / 2 - buttonWidth / 2,
    //     .y = SCREEN_HEIGHT / 2 + 10,
    //     .w = buttonWidth,
    //     .h = buttonHeight
    // };
}

bool allEnemiesDefeated(Entity enemies[]) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (enemies[i].alive) {
            return false;
        }
    }
    return true;
}

void updateEnemyPositions(Entity enemies[], Entity* player, float deltaTime) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (enemies[i].alive) {
            float dx = player->x - enemies[i].x;
            float dy = player->y - enemies[i].y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance != 0) {
                dx /= distance;
                dy /= distance;
            }
            enemies[i].x += dx * ENEMY_SPEED * deltaTime;
            enemies[i].y += dy * ENEMY_SPEED * deltaTime;
        }
    }
}

void gameRunning() {
    Entity player = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, PLAYER_SIZE, true, PLAYER_MAX_HP };
    Entity enemies[ENEMY_COUNT];
    for (int i = 0; i < ENEMY_COUNT; i++) {
        spawnEnemy(&enemies[i]);
    }

    Uint32 lastTime = SDL_GetTicks();
    float angle = 0.0f;
    gameOver = false;

    Uint32 lastDamageTime = 0;
    const Uint32 damageCooldown = 500;  

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                return;
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

                    float scaleX = (float)width / SCREEN_WIDTH;
                    float scaleY = (float)height / SCREEN_HEIGHT;

                    scalePositionsAndSizes(scaleX, scaleY, &player, enemies, ENEMY_COUNT);

                    SCREEN_WIDTH = width;
                    SCREEN_HEIGHT = height;
                }
            }

            if (gameOver && e.type == SDL_MOUSEBUTTONDOWN) {
                return;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    int numBullets = calculateNumBullets();
                    shootBullets(player.x + PLAYER_SIZE / 2, player.y + PLAYER_SIZE / 2, mouseX, mouseY, numBullets);
                }
            }
            
        }

        if (gameOver) {
            int highScore = loadHighScore();
            if (score > highScore) {
                saveHighScore(score);
            }

            render(&player, enemies, 0, 0);  // Render the frozen game state
            renderGameOverText();  
            SDL_RenderPresent(renderer);
            continue;  // Skip game logic updates
        }


        Uint32 currenTime = SDL_GetTicks();
        float deltaTime = (currenTime - lastTime) / 1000.0f;
        lastTime = currenTime;

        // Player movement
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W]) {
            player.y -= SPEED * deltaTime;
        }
        if (state[SDL_SCANCODE_S]) {
            player.y += SPEED * deltaTime;
        }
        if (state[SDL_SCANCODE_A]) {
            player.x -= SPEED * deltaTime;
        }
        if (state[SDL_SCANCODE_D]) {
            player.x += SPEED * deltaTime;
        }

        if (player.x < 0) player.x = 0;
        if (player.x > SCREEN_WIDTH - PLAYER_SIZE) player.x = SCREEN_WIDTH - PLAYER_SIZE;
        if (player.y < 0) player.y = 0;
        if (player.y > SCREEN_HEIGHT - PLAYER_SIZE) player.y = SCREEN_HEIGHT - PLAYER_SIZE;

        // Revolving red dot
        angle += 2.0f * PI * deltaTime;
        float redDotX = player.x + PLAYER_SIZE / 2 + CIRCLE_RADIUS * cos(angle);
        float redDotY = player.y + PLAYER_SIZE / 2 + CIRCLE_RADIUS * sin(angle);
        
        // Check collisions
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (enemies[i].alive && checkCollision(redDotX, redDotY, RED_DOT_RADIUS * 2,
                enemies[i].x + enemies[i].size / 2, enemies[i].y + enemies[i].size / 2, enemies[i].size)) {
                enemies[i].alive = false;
                score++;
                ENEMY_SPEED+= 1.5;
            }

            if (enemies[i].alive && checkCollision(player.x + PLAYER_SIZE / 2, player.y + PLAYER_SIZE / 2, PLAYER_SIZE,
                enemies[i].x + enemies[i].size / 2, enemies[i].y + enemies[i].size / 2, enemies[i].size)) {
                if (currenTime - lastDamageTime >= damageCooldown){
                    player.hp--; 
                    lastDamageTime = currenTime;
                }
                if (player.hp <= 0) {
                    gameOver = true;  // Trigger game over if HP reaches 0
                    break;
                }
            }
        }

        checkBulletCollisions(enemies);

        if (allEnemiesDefeated(enemies)) {
            for (int i = 0; i < ENEMY_COUNT; i++) {
                spawnEnemy(&enemies[i]);
            }
        }

        // Enemy chase player
        updateEnemyPositions(enemies, &player, deltaTime);

        updateBullets(deltaTime);

        render(&player, enemies, redDotX, redDotY);
    }
}

void mainMenu() {
    SDL_Rect startButton, optionButton;
    updateButtonPositions(&startButton, &optionButton);

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                return;
            }
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                SCREEN_WIDTH = e.window.data1;
                SCREEN_HEIGHT = e.window.data2;
                updateButtonPositions(&startButton, &optionButton);
            }            
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                
                // Check click start
                if (x > startButton.x && x < startButton.x + startButton.w &&
                    y > startButton.y && y < startButton.y + startButton.h) {
                    running = true; 
                    score = 0;
                    ENEMY_SPEED= INITIAL_ENEMY_SPEED;
                    gameRunning();  
                    gameOver = false;  
                    break; 
                }
                // Check click option
                // if (x > optionButton.x && x < optionButton.x + optionButton.w &&
                //     y > optionButton.y && y < optionButton.y + optionButton.h) {
                //     // funtionalities
                // }
            }
        }
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, NULL);
        renderButton(startButton, "Start");
        //renderButton(optionButton, "Option");
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char* argv[]) {
    init();
    
    mainMenu();

    cleanup();
    return 0;
} 