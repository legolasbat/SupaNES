#include "SDL.h"
#include "SDL_syswm.h"

#include "Cartridge.h"
#include "Memory.h"

#include <iostream>
#include <commdlg.h>

SDL_Window* win;
SDL_Renderer* renderer;
SDL_Texture* texture;

SDL_Event event;

bool running = true;
bool snesRunning = false;

Memory* memory;
Cartridge* cart;
CPURicoh* cpu;

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

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 224);

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

    cpu = new CPURicoh();
    memory = new Memory(cpu);
    cart = new Cartridge(memory);

    while (running) {

        // Main Loop
        if (snesRunning) {
            cpu->Clock();
        }

        // Events management
        handleInput();

        if (snesRunning) {
            // Graphics
            SDL_RenderClear(renderer);
            // TODO:
            //  SDL_UpdateTexture
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
                            snesRunning = cart->LoadRom(ofn);
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

    return hMenuBar;
}