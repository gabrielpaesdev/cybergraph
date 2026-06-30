#include "engine.h"
#include "font8x8.h"

std::string BUILD_VERSION = "v1.0.0";
bool isDarkMode = true;
Language currentLang = LANG_EN;

std::vector<Node> nodes;
std::vector<Edge> edges;
GameTurn currentTurn = TURN_ROUTER;
GameStatus status = PLAYING;
AppState appState = STATE_MAIN_MENU;

int initialTimeSeconds = 0;
float routerTimeLeft = 0.0f;
float saboteurTimeLeft = 0.0f;
Uint32 lastFrameTime = 0;
bool wonByTimeout = false;


SDL_Color GetColorWhite()  { return isDarkMode ? SDL_Color{255, 255, 255, 255} : SDL_Color{20,  20,  20,  255}; }
SDL_Color GetColorGreen()  { return isDarkMode ? SDL_Color{0,   255, 128, 255} : SDL_Color{0,   160, 80,  255}; }
SDL_Color GetColorRed()    { return isDarkMode ? SDL_Color{255, 50,  80,  255} : SDL_Color{200, 20,  40,  255}; }
SDL_Color GetColorYellow() { return isDarkMode ? SDL_Color{255, 180, 0,   255} : SDL_Color{200, 120, 0,   255}; }
SDL_Color GetColorBlue()   { return isDarkMode ? SDL_Color{0,   150, 255, 255} : SDL_Color{0,   100, 200, 255}; }
SDL_Color GetColorGray()   { return isDarkMode ? SDL_Color{100, 100, 100, 255} : SDL_Color{150, 150, 150, 255}; }


std::string Tr(const std::string& pt, const std::string& en) {
    return currentLang == LANG_PT ? pt : en;
}



void RenderText(SDL_Renderer* renderer, void* unused, const std::string& text, int x, int y, SDL_Color color, bool center) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int scale = 2; 
    if (center) x -= (text.length() * 8 * scale) / 2;

    for (char c : text) {
        unsigned char uc = (unsigned char)c;
        if (uc < 32 || uc > 127) uc = 32;
        const unsigned char* glyph = font8x8[uc - 32];
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (glyph[row] & (0x80 >> col)) {
                    SDL_Rect r = { x + col * scale, y + row * scale, scale, scale };
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }
        x += 8 * scale;
    }
}


void DrawFilledCircle(SDL_Renderer* renderer, int x0, int y0, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; 
            int dy = radius - h; 
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x0 + dx, y0 + dy);
            }
        }
    }
}


void DrawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length == 0) return;

    float nx = -dy / length;
    float ny = dx / length;

    for (int i = -thickness / 2; i <= thickness / 2; ++i) {
        int offsetX = static_cast<int>(nx * i);
        int offsetY = static_cast<int>(ny * i);
        SDL_RenderDrawLine(renderer, x1 + offsetX, y1 + offsetY, x2 + offsetX, y2 + offsetY);
    }
}

// --- Procedural Topology Generation ---
void generateProceduralGraph(int cols, int rows) {
    nodes.clear();
    edges.clear();
    std::srand(std::time(nullptr));

    nodes.push_back({0, 50, 300}); // Source

    int startId = 1;
    int xSpacing = 700 / (cols + 1);
    int ySpacing = 500 / (rows + 1);

    for (int c = 0; c < cols; ++c) {
        for (int r = 0; r < rows; ++r) {
            int id = startId + c * rows + r;
            int jitterX = (std::rand() % 40) - 20;
            int jitterY = (std::rand() % 40) - 20;
            nodes.push_back({id, 50 + (c + 1) * xSpacing + jitterX, 50 + (r + 1) * ySpacing + jitterY});
        }
    }

    int targetId = cols * rows + 1;
    nodes.push_back({targetId, 750, 300}); // Destination

    int edgeId = 0;

    auto addEdge = [&](int u, int v) {
        for (const auto& e : edges) {
            if ((e.u == u && e.v == v) || (e.u == v && e.v == u)) return;
        }
        edges.push_back({edgeId++, u, v, NORMAL});
    };

    std::vector<int> startCols(rows);
    for (int i = 0; i < rows; ++i) startCols[i] = i;
    for (int i = rows - 1; i > 0; --i) std::swap(startCols[i], startCols[std::rand() % (i + 1)]);

    int minStart = (rows <= 3) ? 3 : (rows <= 5) ? 4 : 5; 
    std::vector<bool> col0HasIn(rows, false);

    for (int i = 0; i < minStart && i < rows; ++i) {
        addEdge(0, startId + startCols[i]);
        col0HasIn[startCols[i]] = true;
    }
    for (int i = minStart; i < rows; ++i) {
        if (std::rand() % 100 > 10) { 
            addEdge(0, startId + startCols[i]);
            col0HasIn[startCols[i]] = true;
        }
    }

    for (int r = 0; r < rows; ++r) {
        if (!col0HasIn[r]) {
            int u = startId + r;
            int neighbor = (r > 0) ? (u - 1) : (u + 1);
            addEdge(neighbor, u); 
        }
    }

    for (int c = 0; c < cols - 1; ++c) {
        for (int r = 0; r < rows; ++r) {
            int u = startId + c * rows + r;
            addEdge(u, u + rows); 
            if (r > 0       && std::rand() % 100 > 35) addEdge(u, u + rows - 1); 
            if (r < rows-1  && std::rand() % 100 > 35) addEdge(u, u + rows + 1); 
            if (r < rows-1  && std::rand() % 100 > 45) addEdge(u, u + 1);        
        }
    }

    std::vector<int> endCols(rows);
    for (int i = 0; i < rows; ++i) endCols[i] = i;
    for (int i = rows - 1; i > 0; --i) std::swap(endCols[i], endCols[std::rand() % (i + 1)]);

    int minEnd = (rows <= 3) ? 3 : (rows <= 5) ? 4 : 5; 
    std::vector<bool> lastColHasOut(rows, false);

    for (int i = 0; i < minEnd && i < rows; ++i) {
        addEdge(startId + (cols - 1) * rows + endCols[i], targetId);
        lastColHasOut[endCols[i]] = true;
    }
    for (int i = minEnd; i < rows; ++i) {
        if (std::rand() % 100 > 10) { 
            addEdge(startId + (cols - 1) * rows + endCols[i], targetId);
            lastColHasOut[endCols[i]] = true;
        }
    }

    for (int r = 0; r < rows; ++r) {
        if (!lastColHasOut[r]) {
            int u = startId + (cols - 1) * rows + r;
            int neighbor = (r > 0) ? (u - 1) : (u + 1);
            addEdge(u, neighbor);
        }
    }

    auto countSourceEdges = [&]() {
        int count = 0;
        for (const auto& e : edges) if (e.u == 0 || e.v == 0) count++;
        return count;
    };

    int attempts = 0;
    while (countSourceEdges() < 3 && attempts < rows) {
        int r = std::rand() % rows;
        addEdge(0, startId + r);
        attempts++;
    }

    auto countTargetEdges = [&]() {
        int count = 0;
        for (const auto& e : edges) if (e.u == targetId || e.v == targetId) count++;
        return count;
    };

    attempts = 0;
    while (countTargetEdges() < 3 && attempts < rows) {
        int r = std::rand() % rows;
        addEdge(startId + (cols - 1) * rows + r, targetId);
        attempts++;
    }


    lastFrameTime = SDL_GetTicks();
    routerTimeLeft = static_cast<float>(initialTimeSeconds);
    saboteurTimeLeft = static_cast<float>(initialTimeSeconds);
    wonByTimeout = false;
    status = PLAYING;
    currentTurn = TURN_ROUTER;
}


bool checkPathExists(bool onlyReinforced) {
    std::vector<bool> visited(nodes.size(), false);
    std::queue<int> q;
    
    q.push(0);
    visited[0] = true;
    int targetNodeId = nodes.size() - 1;

    while (!q.empty()) {
        int curr = q.front();
        q.pop();

        if (curr == targetNodeId) return true;

        for (const auto& edge : edges) {
            if (onlyReinforced && edge.state != REINFORCED) continue;
            if (!onlyReinforced && edge.state == CUT) continue;

            int nextNode = (edge.u == curr) ? edge.v : ((edge.v == curr) ? edge.u : -1);

            if (nextNode != -1 && !visited[nextNode]) {
                visited[nextNode] = true;
                q.push(nextNode);
            }
        }
    }
    return false;
}

void updateWinConditions() {
    if (!checkPathExists(false)) {
        status = SABOTEUR_WINS;
    } else if (checkPathExists(true)) {
        status = ROUTER_WINS;
    }
}


float pointToLineDist(int px, int py, int x1, int y1, int x2, int y2) {
    float line_len_sq = pow(x2 - x1, 2) + pow(y2 - y1, 2);
    if (line_len_sq == 0.0) return sqrt(pow(px - x1, 2) + pow(py - y1, 2));
    float t = std::max(0.0f, std::min(1.0f, ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / line_len_sq));
    float proj_x = x1 + t * (x2 - x1);
    float proj_y = y1 + t * (y2 - y1);
    return sqrt(pow(px - proj_x, 2) + pow(py - proj_y, 2));
}

void handleGameMouseClick(int mouseX, int mouseY) {
    if (status != PLAYING) return;

    for (auto& edge : edges) {
        int x1 = nodes[edge.u].x, y1 = nodes[edge.u].y;
        int x2 = nodes[edge.v].x, y2 = nodes[edge.v].y;

        if (pointToLineDist(mouseX, mouseY, x1, y1, x2, y2) < 18.0f) { 
            if (currentTurn == TURN_ROUTER && edge.state == NORMAL) {
                edge.state = REINFORCED;
                currentTurn = TURN_SABOTEUR;
                updateWinConditions();
                return;
            } 
            else if (currentTurn == TURN_SABOTEUR) {
                if (edge.state == NORMAL) {
                    edge.state = CUT;
                    currentTurn = TURN_ROUTER;
                    updateWinConditions();
                    return;
                }
            }
        }
    }
}

void handleMainMenuClick(int mouseX, int mouseY) {
    if (mouseX >= 250 && mouseX <= 550) {
        if (mouseY >= 200 && mouseY <= 260) appState = STATE_TIME_SELECT; 
        else if (mouseY >= 300 && mouseY <= 360) appState = STATE_OPTIONS;
        else if (mouseY >= 400 && mouseY <= 460) appState = STATE_CREDITS;
    }
}

void handleTimeSelectClick(int mouseX, int mouseY) {
    if (mouseX >= 250 && mouseX <= 550) {
        if (mouseY >= 140 && mouseY <= 190)      { initialTimeSeconds = 0;   appState = STATE_LEVEL_SELECT; }
        else if (mouseY >= 210 && mouseY <= 260) { initialTimeSeconds = 60;  appState = STATE_LEVEL_SELECT; }
        else if (mouseY >= 280 && mouseY <= 330) { initialTimeSeconds = 180; appState = STATE_LEVEL_SELECT; }
        else if (mouseY >= 350 && mouseY <= 400) { initialTimeSeconds = 300; appState = STATE_LEVEL_SELECT; }
        else if (mouseY >= 420 && mouseY <= 470) { initialTimeSeconds = 600; appState = STATE_LEVEL_SELECT; }
        else if (mouseY >= 500 && mouseY <= 540) { appState = STATE_MAIN_MENU; } 
    }
}

void handleLevelSelectClick(int mouseX, int mouseY) {
    if (mouseX >= 250 && mouseX <= 550) {
        if (mouseY >= 200 && mouseY <= 260) {
            generateProceduralGraph(4, 3);
            appState = STATE_PLAYING;
        } else if (mouseY >= 300 && mouseY <= 360) {
            generateProceduralGraph(6, 5);
            appState = STATE_PLAYING;
        } else if (mouseY >= 400 && mouseY <= 460) {
            generateProceduralGraph(8, 6);
            appState = STATE_PLAYING;
        } else if (mouseY >= 500 && mouseY <= 540) {
            appState = STATE_TIME_SELECT; 
        }
    }
}

void handleOptionsClick(int mouseX, int mouseY) {
    if (mouseX >= 250 && mouseX <= 550) {
        if (mouseY >= 250 && mouseY <= 310) {
            isDarkMode = !isDarkMode; 
        } else if (mouseY >= 450 && mouseY <= 510) {
            appState = STATE_MAIN_MENU; 
        }
    }
    
    
    if (mouseY >= 370 && mouseY <= 420) {
        if (mouseX >= 250 && mouseX <= 390) currentLang = LANG_EN;
        else if (mouseX >= 410 && mouseX <= 550) currentLang = LANG_PT;
    }
}

void handleCreditsClick(int mouseX, int mouseY) {
    if (mouseX >= 250 && mouseX <= 550) {
        if (mouseY >= 450 && mouseY <= 510) {
            appState = STATE_MAIN_MENU;
        }
    }
}
