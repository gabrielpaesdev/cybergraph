#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

// --- Global Variables (Config) ---
extern std::string BUILD_VERSION;
extern bool isDarkMode;

enum Language { LANG_PT, LANG_EN };
extern Language currentLang;

// --- Data Structures ---
struct Node {
    int id, x, y;
};

enum EdgeState { NORMAL, REINFORCED, CUT };

struct Edge {
    int id, u, v;
    EdgeState state;
};

enum GameTurn { TURN_ROUTER, TURN_SABOTEUR };
enum GameStatus { PLAYING, ROUTER_WINS, SABOTEUR_WINS };
enum AppState { STATE_MAIN_MENU, STATE_TIME_SELECT, STATE_LEVEL_SELECT, STATE_OPTIONS, STATE_CREDITS, STATE_PLAYING };

// --- Global Variables (State) ---
extern std::vector<Node> nodes;
extern std::vector<Edge> edges;
extern GameTurn currentTurn;
extern GameStatus status;
extern AppState appState;

// Chess Clock Variables
extern int initialTimeSeconds;
extern float routerTimeLeft;
extern float saboteurTimeLeft;
extern Uint32 lastFrameTime;
extern bool wonByTimeout;

// --- Dynamic Colors ---
SDL_Color GetColorWhite();
SDL_Color GetColorGreen();
SDL_Color GetColorRed();
SDL_Color GetColorYellow();
SDL_Color GetColorBlue();
SDL_Color GetColorGray();

// --- Localization Helper ---
std::string Tr(const std::string& pt, const std::string& en);

// Removemos o parâmetro TTF_Font* e colocamos void* (ou simplesmente remova)
void RenderText(SDL_Renderer* renderer, void* unused, const std::string& text, int x, int y, SDL_Color color, bool center = false);
void DrawFilledCircle(SDL_Renderer* renderer, int x0, int y0, int radius);
void DrawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness);

// --- Game Logic ---
void generateProceduralGraph(int cols, int rows);
bool checkPathExists(bool onlyReinforced);
void updateWinConditions();

// --- Input Handling ---
float pointToLineDist(int px, int py, int x1, int y1, int x2, int y2);
void handleGameMouseClick(int mouseX, int mouseY);
void handleMainMenuClick(int mouseX, int mouseY);
void handleTimeSelectClick(int mouseX, int mouseY);
void handleLevelSelectClick(int mouseX, int mouseY);
void handleOptionsClick(int mouseX, int mouseY);
void handleCreditsClick(int mouseX, int mouseY);

#endif // ENGINE_H
