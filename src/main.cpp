#include "SDL.h"
#include "SDL_syswm.h"

#include "SupaNES.h"

#include <iostream>
#include <commdlg.h>

SDL_Window* win;
SDL_Renderer* renderer;
SDL_Texture* texture;

SDL_Event event;

bool running = true;
bool snesRunning = false;

SupaNES* supaNES;

void handleInput();
HMENU CreateMenuBar();

int main(int argc, char* argv[]) {

    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_CreateWindowAndRenderer(256, 224, 0, &win, &renderer);
    SDL_SetWindowSize(win, 256 * 2, 224 * 2);
    SDL_RenderSetLogicalSize(renderer, 256, 224);
    SDL_SetWindowResizable(win, SDL_TRUE);

    //texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 224);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR555, SDL_TEXTUREACCESS_STREAMING, 256, 224);

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

    supaNES = new SupaNES();

    while (running) {

        // Main Loop
        if (snesRunning) {
            supaNES->Clock();
        }
        else {
            // Events management
            handleInput();
        }

        if (snesRunning && supaNES->ppu->IsFrameCompleted()) {

            // Events management
            handleInput();

            // Graphics
            SDL_RenderClear(renderer);
            uint16_t* frame = supaNES->ppu->GetFrame();
            SDL_UpdateTexture(texture, NULL, frame, supaNES->ppu->GetPitch());
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }
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
                    else if (LOWORD(event.syswm.msg->msg.win.wParam) == 7) {
                    if (!snesRunning) {
                        continue;
                    }
                    // TODO:
                    // Create new window
                    // Set texture dimensions as background size
                    // Update texture
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

    std::wstring sBGTilemap = L"Background Tilemap";
    LPCWSTR shBGTilemap = sBGTilemap.c_str();
    AppendMenu(hDebug, MF_POPUP, 7, shBGTilemap);

    return hMenuBar;
}