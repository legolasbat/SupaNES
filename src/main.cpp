#include "SDL.h"
#include "SDL_syswm.h"

#include "SupaNES.h"

#include <iostream>
#include <commdlg.h>
#include <chrono>

SDL_Window* win;
SDL_Renderer* renderer;
SDL_Texture* texture;
SDL_Joystick* gameController;

SDL_Event event;

bool running = true;
bool snesRunning = false;

int cycles = 0;
std::chrono::steady_clock::time_point start;

SupaNES* supaNES;

void handleInput();
HMENU CreateMenuBar();

int main(int argc, char* argv[]) {

    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_CreateWindowAndRenderer(256, 224, 0, &win, &renderer);
    SDL_SetWindowSize(win, 256 * 2, 224 * 2);
    SDL_RenderSetLogicalSize(renderer, 256, 224);
    SDL_SetWindowResizable(win, SDL_TRUE);

    //texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 224);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_STREAMING, 256, 224);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_SetWindowTitle(win, "SupaNES");
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(win, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    HMENU hMenuBar = CreateMenuBar();
    SetMenu(hwnd, hMenuBar);

    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    //Check for joysticks
    if (SDL_NumJoysticks() < 1)
    {
        printf("Warning: No joysticks connected!\n");
    }
    else
    {
        //Load joystick
        gameController = SDL_JoystickOpen(0);
        if (gameController == NULL)
        {
            printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
        }
    }

    supaNES = new SupaNES();

    while (running) {

        // Main Loop
        if (snesRunning) {
            cycles += supaNES->Clock();
        }
        else {
            // Events management
            handleInput();
        }

        if (snesRunning && supaNES->ppu->IsFrameCompleted()) {

            //while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 16.64) {
            //    
            //}
            cycles = 0;// -= 8853;     // CPU Speed / Framerate / 4 (idk why)

            // Events management
            handleInput();

            // Graphics
            SDL_RenderClear(renderer);
            uint16_t* frame = supaNES->ppu->GetFrame();
            SDL_UpdateTexture(texture, NULL, frame, supaNES->ppu->GetPitch());
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            start = std::chrono::high_resolution_clock::now();
        }
    }

    //Close game controller
    SDL_JoystickClose(gameController);
    gameController = NULL;

    //Destroy window
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    win = NULL;
    renderer = NULL;
    texture = NULL;

    return 0;
}

void handleInput() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {

            case SDL_QUIT: {
                // handling of close button
                running = false;
                break;
            }
            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                if (!snesRunning) {
                    continue;
                }
                switch (event.key.keysym.sym) {
                // B Button (PS: cross / XBOX: A)
                case SDLK_k: {
                    supaNES->cpu->joypad1Hi |= 0x80;
                    break;
                }
                // A Button (PS: circle / XBOX: B)
                case SDLK_j: {
                    supaNES->cpu->joypad1Lo |= 0x80;
                    break;
                }
                // Y Button (PS: square / XBOX: X)
                case SDLK_i: {
                    supaNES->cpu->joypad1Hi |= 0x40;
                    break;
                }
                // X Button (PS: triangle / XBOX: Y)
                case SDLK_u: {
                    supaNES->cpu->joypad1Lo |= 0x40;
                    break;
                }
                // Select Button
                case SDLK_q: {
                    supaNES->cpu->joypad1Hi |= 0x20;
                    break;
                }
                // Start Button
                case SDLK_e: {
                    supaNES->cpu->joypad1Hi |= 0x10;
                    break;
                }
                // L Button (PS: L1 / XBOX: LB)
                case SDLK_o: {
                    supaNES->cpu->joypad1Lo |= 0x20;
                    break;
                }
                // R Button (PS: R1 / XBOX: RB)
                case SDLK_p: {
                    supaNES->cpu->joypad1Lo |= 0x10;
                    break;
                }
                // Up Button
                case SDLK_w: {
                    supaNES->cpu->joypad1Hi |= 0x08;
                    break;
                }
                // Down Button
                case SDLK_s: {
                    supaNES->cpu->joypad1Hi |= 0x04;
                    break;
                }
                // Left Button
                case SDLK_a: {
                    supaNES->cpu->joypad1Hi |= 0x02;
                    break;
                }
                // Right Button
                case SDLK_d: {
                    supaNES->cpu->joypad1Hi |= 0x01;
                    break;
                }
                }
                break;
            }
            case SDL_KEYUP: {
                if (!snesRunning) {
                    continue;
                }
                switch (event.key.keysym.sym) {
                // B Button (PS: cross / XBOX: A)
                case SDLK_k: {
                    supaNES->cpu->joypad1Hi &= ~0x80;
                    break;
                }
                // A Button (PS: circle / XBOX: B)
                case SDLK_j: {
                    supaNES->cpu->joypad1Lo &= ~0x80;
                    break;
                }
                // Y Button (PS: square / XBOX: X)
                case SDLK_i: {
                    supaNES->cpu->joypad1Hi &= ~0x40;
                    break;
                }
                // X Button (PS: triangle / XBOX: Y)
                case SDLK_u: {
                    supaNES->cpu->joypad1Lo &= ~0x40;
                    break;
                }
                // Select Button
                case SDLK_q: {
                    supaNES->cpu->joypad1Hi &= ~0x20;
                    break;
                }
                // Start Button
                case SDLK_e: {
                    supaNES->cpu->joypad1Hi &= ~0x10;
                    break;
                }
                // L Button (PS: L1 / XBOX: LB)
                case SDLK_o: {
                    supaNES->cpu->joypad1Lo &= ~0x20;
                    break;
                }
                // R Button (PS: R1 / XBOX: RB)
                case SDLK_p: {
                    supaNES->cpu->joypad1Lo &= ~0x10;
                    break;
                }
                // Up Button
                case SDLK_w: {
                    supaNES->cpu->joypad1Hi &= ~0x08;
                    break;
                }
                // Down Button
                case SDLK_s: {
                    supaNES->cpu->joypad1Hi &= ~0x04;
                    break;
                }
                // Left Button
                case SDLK_a: {
                    supaNES->cpu->joypad1Hi &= ~0x02;
                    break;
                }
                // Right Button
                case SDLK_d: {
                    supaNES->cpu->joypad1Hi &= ~0x01;
                    break;
                }
                }
                break;
            }
            case SDL_JOYBUTTONDOWN: {
                if (!snesRunning) {
                    continue;
                }
                switch (event.jbutton.button) {
                // B Button (PS: cross / XBOX: A)
                case 0: {
                    supaNES->cpu->joypad1Hi |= 0x80;
                    break;
                }
                // A Button (PS: circle / XBOX: B)
                case 1: {
                    supaNES->cpu->joypad1Lo |= 0x80;
                    break;
                }
                // Y Button (PS: square / XBOX: X)
                case 2: {
                    supaNES->cpu->joypad1Hi |= 0x40;
                    break;
                }
                // X Button (PS: triangle / XBOX: Y)
                case 3: {
                    supaNES->cpu->joypad1Lo |= 0x40;
                    break;
                }
                // Select Button
                case 4: {
                    supaNES->cpu->joypad1Hi |= 0x20;
                    break;
                }
                // Start Button
                case 6: {
                    supaNES->cpu->joypad1Hi |= 0x10;
                    break;
                }
                // L Button (PS: L1 / XBOX: LB)
                case 9: {
                    supaNES->cpu->joypad1Lo |= 0x20;
                    break;
                }
                // R Button (PS: R1 / XBOX: RB)
                case 10: {
                    supaNES->cpu->joypad1Lo |= 0x10;
                    break;
                }
                // Up Button
                case 11: {
                    supaNES->cpu->joypad1Hi |= 0x08;
                    break;
                }
                // Down Button
                case 12: {
                    supaNES->cpu->joypad1Hi |= 0x04;
                    break;
                }
                // Left Button
                case 13: {
                    supaNES->cpu->joypad1Hi |= 0x02;
                    break;
                }
                // Right Button
                case 14: {
                    supaNES->cpu->joypad1Hi |= 0x01;
                    break;
                }
                }
                break;
            }
            case SDL_JOYBUTTONUP: {
                if (!snesRunning) {
                    continue;
                }
                switch (event.jbutton.button) {
                    // B Button (PS: cross / XBOX: A)
                case 0: {
                    supaNES->cpu->joypad1Hi &= ~0x80;
                    break;
                }
                      // A Button (PS: circle / XBOX: B)
                case 1: {
                    supaNES->cpu->joypad1Lo &= ~0x80;
                    break;
                }
                      // Y Button (PS: square / XBOX: X)
                case 2: {
                    supaNES->cpu->joypad1Hi &= ~0x40;
                    break;
                }
                      // X Button (PS: triangle / XBOX: Y)
                case 3: {
                    supaNES->cpu->joypad1Lo &= ~0x40;
                    break;
                }
                      // Select Button
                case 4: {
                    supaNES->cpu->joypad1Hi &= ~0x20;
                    break;
                }
                      // Start Button
                case 6: {
                    supaNES->cpu->joypad1Hi &= ~0x10;
                    break;
                }
                      // L Button (PS: L1 / XBOX: LB)
                case 9: {
                    supaNES->cpu->joypad1Lo &= ~0x20;
                    break;
                }
                      // R Button (PS: R1 / XBOX: RB)
                case 10: {
                    supaNES->cpu->joypad1Lo &= ~0x10;
                    break;
                }
                       // Up Button
                case 11: {
                    supaNES->cpu->joypad1Hi &= ~0x08;
                    break;
                }
                       // Down Button
                case 12: {
                    supaNES->cpu->joypad1Hi &= ~0x04;
                    break;
                }
                       // Left Button
                case 13: {
                    supaNES->cpu->joypad1Hi &= ~0x02;
                    break;
                }
                       // Right Button
                case 14: {
                    supaNES->cpu->joypad1Hi &= ~0x01;
                    break;
                }
                }
                break;
            }
            case SDL_SYSWMEVENT: {
                if (event.syswm.msg->msg.win.msg == WM_COMMAND) {
                    // Exit
                    if (LOWORD(event.syswm.msg->msg.win.wParam) == 1) {
                        running = false;
                    }
                    // Load ROM
                    else if (LOWORD(event.syswm.msg->msg.win.wParam) == 2) {
                        char szFile[260];
                        OPENFILENAME ofn;

                        ZeroMemory(&ofn, sizeof(ofn));

                        ofn.lStructSize = sizeof(ofn);
                        ofn.lpstrFile = (LPWSTR)szFile;
                        ofn.lpstrFile[0] = '\0';
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = L"*.smc\0";
                        ofn.lpstrTitle = L"ROM Selection";
                        ofn.Flags = OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                        if (GetOpenFileName(&ofn) == TRUE) {
                            snesRunning = supaNES->cart->LoadRom(ofn);
                            start = std::chrono::high_resolution_clock::now();
                            cycles = 0;
                        }
                    }
                    // Toggle backgrounds
                    else if (LOWORD(event.syswm.msg->msg.win.wParam) == 3) {
                        if (!snesRunning) {
                            continue;
                        }

                        SDL_SysWMinfo wmInfo;
                        SDL_VERSION(&wmInfo.version);
                        SDL_GetWindowWMInfo(win, &wmInfo);
                        HWND hwnd = wmInfo.info.win.window;
                        HMENU menu = GetMenu(wmInfo.info.win.window);

                        UINT state = GetMenuState(menu, 3, MF_BYCOMMAND);

                        if (state == MF_CHECKED) {
                            CheckMenuItem(menu, 3, MF_UNCHECKED);
                            supaNES->ppu->ToggleBG1(false);
                        }
                        else {
                            CheckMenuItem(menu, 3, MF_CHECKED);
                            supaNES->ppu->ToggleBG1(true);
                        }
                    }
                    else if (LOWORD(event.syswm.msg->msg.win.wParam) == 4) {
                        if (!snesRunning) {
                            continue;
                        }

                        SDL_SysWMinfo wmInfo;
                        SDL_VERSION(&wmInfo.version);
                        SDL_GetWindowWMInfo(win, &wmInfo);
                        HWND hwnd = wmInfo.info.win.window;
                        HMENU menu = GetMenu(wmInfo.info.win.window);

                        UINT state = GetMenuState(menu, 4, MF_BYCOMMAND);

                        if (state == MF_CHECKED) {
                            CheckMenuItem(menu, 4, MF_UNCHECKED);
                            supaNES->ppu->ToggleBG2(false);
                        }
                        else {
                            CheckMenuItem(menu, 4, MF_CHECKED);
                            supaNES->ppu->ToggleBG2(true);
                        }
                    }
                    else if (LOWORD(event.syswm.msg->msg.win.wParam) == 5) {
                        if (!snesRunning) {
                            continue;
                        }

                        SDL_SysWMinfo wmInfo;
                        SDL_VERSION(&wmInfo.version);
                        SDL_GetWindowWMInfo(win, &wmInfo);
                        HWND hwnd = wmInfo.info.win.window;
                        HMENU menu = GetMenu(wmInfo.info.win.window);

                        UINT state = GetMenuState(menu, 5, MF_BYCOMMAND);

                        if (state == MF_CHECKED) {
                            CheckMenuItem(menu, 5, MF_UNCHECKED);
                            supaNES->ppu->ToggleBG3(false);
                        }
                        else {
                            CheckMenuItem(menu, 5, MF_CHECKED);
                            supaNES->ppu->ToggleBG3(true);
                        }
                    }
                    else if (LOWORD(event.syswm.msg->msg.win.wParam) == 6) {
                        if (!snesRunning) {
                            continue;
                        }

                        SDL_SysWMinfo wmInfo;
                        SDL_VERSION(&wmInfo.version);
                        SDL_GetWindowWMInfo(win, &wmInfo);
                        HWND hwnd = wmInfo.info.win.window;
                        HMENU menu = GetMenu(wmInfo.info.win.window);

                        UINT state = GetMenuState(menu, 6, MF_BYCOMMAND);

                        if (state == MF_CHECKED) {
                            CheckMenuItem(menu, 6, MF_UNCHECKED);
                            supaNES->ppu->ToggleBG4(false);
                        }
                        else {
                            CheckMenuItem(menu, 6, MF_CHECKED);
                            supaNES->ppu->ToggleBG4(true);
                        }
                    }
                }

                break;
            }
        }
    }
}

HMENU CreateMenuBar() {
    HMENU hMenuBar = CreateMenu();

    // File menu
    HMENU hFile = CreateMenu();
    std::wstring sFile = L"File";
    LPCWSTR shFile = sFile.c_str();
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, shFile);

    std::wstring sLoad = L"Load ROM";
    LPCWSTR shLoad = sLoad.c_str();
    AppendMenu(hFile, MF_STRING, 2, shLoad);

    std::wstring sExit = L"Exit";
    LPCWSTR shExit = sExit.c_str();
    AppendMenu(hFile, MF_STRING, 1, shExit);

    // Debug menu
    HMENU hDebug = CreateMenu();
    std::wstring sDebug = L"Debug";
    LPCWSTR shDebug = sDebug.c_str();
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hDebug, shDebug);

    HMENU hDBG = CreateMenu();
    std::wstring sDBG = L"Display BG";
    LPCWSTR shDBG = sDBG.c_str();
    AppendMenu(hDebug, MF_POPUP, (UINT_PTR)hDBG, shDBG);

    std::wstring sBG1 = L"BG1";
    LPCWSTR shBG1 = sBG1.c_str();
    AppendMenu(hDBG, MF_STRING | MF_CHECKED, 3, shBG1);

    std::wstring sBG2 = L"BG2";
    LPCWSTR shBG2 = sBG2.c_str();
    AppendMenu(hDBG, MF_STRING | MF_CHECKED, 4, shBG2);

    std::wstring sBG3 = L"BG3";
    LPCWSTR shBG3 = sBG3.c_str();
    AppendMenu(hDBG, MF_STRING | MF_CHECKED, 5, shBG3);

    std::wstring sBG4 = L"BG4";
    LPCWSTR shBG4 = sBG4.c_str();
    AppendMenu(hDBG, MF_STRING | MF_CHECKED, 6, shBG4);

    return hMenuBar;
}