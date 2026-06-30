#include "engine.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("CyberGraph", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                                         
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, 800, 600);

    bool running = true;
    SDL_Event event;

    while (running) {
 
        Uint32 currentFrameTime = SDL_GetTicks();
        float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentFrameTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } 
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (appState == STATE_MAIN_MENU) handleMainMenuClick(event.button.x, event.button.y);
                else if (appState == STATE_TIME_SELECT) handleTimeSelectClick(event.button.x, event.button.y);
                else if (appState == STATE_LEVEL_SELECT) handleLevelSelectClick(event.button.x, event.button.y);
                else if (appState == STATE_OPTIONS) handleOptionsClick(event.button.x, event.button.y);
                else if (appState == STATE_CREDITS) handleCreditsClick(event.button.x, event.button.y);
                else if (appState == STATE_PLAYING) handleGameMouseClick(event.button.x, event.button.y);
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_F11) {
                    Uint32 flags = SDL_GetWindowFlags(window);
                    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) SDL_SetWindowFullscreen(window, 0);
                    else SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (appState == STATE_PLAYING) {
                        appState = STATE_LEVEL_SELECT;
                        status = PLAYING;
                        currentTurn = TURN_ROUTER;
                    } else if (appState != STATE_MAIN_MENU) {
                        appState = STATE_MAIN_MENU;
                    }
                } else if (event.key.keysym.sym == SDLK_r && appState == STATE_PLAYING) {
                    int c = nodes.size() > 30 ? 8 : (nodes.size() > 15 ? 6 : 4);
                    int r = nodes.size() > 30 ? 6 : (nodes.size() > 15 ? 5 : 3);
                    generateProceduralGraph(c, r);
                }
            }
        }

        // --- CHESS CLOCK LOGIC ---
        if (appState == STATE_PLAYING && status == PLAYING && initialTimeSeconds > 0) {
            if (currentTurn == TURN_ROUTER) {
                routerTimeLeft -= deltaTime;
                if (routerTimeLeft <= 0) {
                    routerTimeLeft = 0;
                    status = SABOTEUR_WINS;
                    wonByTimeout = true;
                }
            } else {
                saboteurTimeLeft -= deltaTime;
                if (saboteurTimeLeft <= 0) {
                    saboteurTimeLeft = 0;
                    status = ROUTER_WINS;
                    wonByTimeout = true;
                }
            }
        }

        // --- THEME SETUP ---
        SDL_Color primaryText = GetColorWhite();
        if (appState != STATE_PLAYING) {
            SDL_SetRenderDrawColor(renderer, isDarkMode ? 10 : 240, isDarkMode ? 12 : 245, isDarkMode ? 18 : 250, 255);
            SDL_RenderClear(renderer);
        } else {
            if (status == SABOTEUR_WINS) SDL_SetRenderDrawColor(renderer, 30, 5, 5, 255);
            else if (status == ROUTER_WINS) SDL_SetRenderDrawColor(renderer, 5, 30, 10, 255);
            else SDL_SetRenderDrawColor(renderer, isDarkMode ? 10 : 240, isDarkMode ? 12 : 245, isDarkMode ? 18 : 250, 255);
            SDL_RenderClear(renderer);
        }


        if (appState == STATE_MAIN_MENU) {
            RenderText(renderer, nullptr, "CYBERGRAPH", 400, 100, primaryText, true);
            RenderText(renderer, nullptr, Tr("Jogar", "Play"), 400, 230, GetColorGreen(), true);
            RenderText(renderer, nullptr, Tr("Opcoes", "Options"), 400, 330, GetColorYellow(), true);
            RenderText(renderer, nullptr, Tr("Creditos", "Credits"), 400, 430, GetColorBlue(), true);
        } 
        else if (appState == STATE_TIME_SELECT) {
            RenderText(renderer, nullptr, Tr("LIMITE DE TEMPO", "TIME LIMIT"), 400, 80, primaryText, true);
            RenderText(renderer, nullptr, Tr("Sem limite", "No limit"), 400, 155, GetColorGreen(), true);
            RenderText(renderer, nullptr, "1:00", 400, 225, GetColorYellow(), true);
            RenderText(renderer, nullptr, "3:00", 400, 295, GetColorYellow(), true);
            RenderText(renderer, nullptr, "5:00", 400, 365, GetColorRed(), true);
            RenderText(renderer, nullptr, "10:00", 400, 435, GetColorRed(), true);
            RenderText(renderer, nullptr, Tr("Voltar", "Back"), 400, 520, primaryText, true);
        }
        else if (appState == STATE_LEVEL_SELECT) {
            RenderText(renderer, nullptr, Tr("SELECIONE A TOPOLOGIA", "SELECT TOPOLOGY"), 400, 100, primaryText, true);
            RenderText(renderer, nullptr, Tr("LAN (Pequeno)", "LAN (Small)"), 400, 230, GetColorGreen(), true);
            RenderText(renderer, nullptr, "Corporate WAN", 400, 330, GetColorYellow(), true);
            RenderText(renderer, nullptr, "Global Backbone", 400, 430, GetColorRed(), true);
            RenderText(renderer, nullptr, Tr("Voltar", "Back"), 400, 520, primaryText, true);
        }
        else if (appState == STATE_OPTIONS) {
            RenderText(renderer, nullptr, Tr("OPCOES", "OPTIONS"), 400, 100, primaryText, true);
            std::string themeText = isDarkMode ? Tr("Modo Escuro: ON", "Dark Mode: ON") : Tr("Modo Escuro: OFF", "Dark Mode: OFF");
            RenderText(renderer, nullptr, themeText, 400, 280, GetColorYellow(), true);

            SDL_Color enColor = (currentLang == LANG_EN) ? GetColorGreen() : GetColorWhite();
            SDL_Color ptColor = (currentLang == LANG_PT) ? GetColorGreen() : GetColorWhite();
            RenderText(renderer, nullptr, "EN", 320, 395, enColor, true);
            RenderText(renderer, nullptr, "PT", 480, 395, ptColor, true);

            RenderText(renderer, nullptr, Tr("Voltar", "Back"), 400, 480, primaryText, true);
        }
        else if (appState == STATE_CREDITS) {
            RenderText(renderer, nullptr, Tr("CREDITOS", "CREDITS"), 400, 100, primaryText, true);
            RenderText(renderer, nullptr, "Gabriel Paes", 400, 250, GetColorYellow(), true);
            RenderText(renderer, nullptr, "2026", 400, 290, primaryText, true);
            RenderText(renderer, nullptr, "gabrielpaesdev@proton.me", 400, 330, GetColorGreen(), true);
            RenderText(renderer, nullptr, Tr("Voltar", "Back"), 400, 480, primaryText, true);
        }
        else if (appState == STATE_PLAYING) {
            
            for (const auto& edge : edges) {
                SDL_Color edgeColor;
                int thickness = 4;
                switch (edge.state) {
                    case REINFORCED: edgeColor = GetColorGreen(); thickness = 6; break;
                    case CUT:         edgeColor = GetColorRed();   thickness = 2; break;
                    default:          edgeColor = GetColorGray();  thickness = 4; break;
                }
                SDL_SetRenderDrawColor(renderer, edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
                DrawThickLine(renderer, nodes[edge.u].x, nodes[edge.u].y, nodes[edge.v].x, nodes[edge.v].y, thickness);
            }

            
            for (size_t i = 0; i < nodes.size(); i++) {
                SDL_Color nodeColor;
                if (i == 0) nodeColor = GetColorBlue();                         
                else if (i == nodes.size() - 1) nodeColor = GetColorYellow();   
                else nodeColor = GetColorWhite();

                SDL_SetRenderDrawColor(renderer, nodeColor.r, nodeColor.g, nodeColor.b, nodeColor.a);
                DrawFilledCircle(renderer, nodes[i].x, nodes[i].y, 10);
            }

            
            std::string turnText = (currentTurn == TURN_ROUTER) ? Tr("Turno: Roteador", "Turn: Router") : Tr("Turno: Sabotador", "Turn: Saboteur");
            SDL_Color turnColor = (currentTurn == TURN_ROUTER) ? GetColorGreen() : GetColorRed();
            RenderText(renderer, nullptr, turnText, 400, 20, turnColor, true);

            
            if (initialTimeSeconds > 0) {
                char buf[64];
                snprintf(buf, sizeof(buf), "R %02d:%02d", (int)routerTimeLeft / 60, (int)routerTimeLeft % 60);
                RenderText(renderer, nullptr, buf, 100, 20, GetColorWhite(), true);
                snprintf(buf, sizeof(buf), "S %02d:%02d", (int)saboteurTimeLeft / 60, (int)saboteurTimeLeft % 60);
                RenderText(renderer, nullptr, buf, 700, 20, GetColorWhite(), true);
            }

            
            if (status == ROUTER_WINS) {
                RenderText(renderer, nullptr, Tr("ROTEADOR VENCEU!", "ROUTER WINS!"), 400, 50, GetColorGreen(), true);
            } else if (status == SABOTEUR_WINS) {
                RenderText(renderer, nullptr, Tr("SABOTADOR VENCEU!", "SABOTEUR WINS!"), 400, 50, GetColorRed(), true);
            }

            
            RenderText(renderer, nullptr, Tr("[ESC] Menu [R] Recriar Mapa [F11] Tela Cheia", "[ESC] Menu [R] Regenerate Map [F11] Fullscreen"), 400, 560, primaryText, true);
        }

        
        RenderText(renderer, nullptr, BUILD_VERSION, 400, 585, GetColorGray(), true);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
