#include "SDL.h"
#include "SDL_syswm.h"

#include <iostream>

int main(int argc, char* argv[]) {

    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    SDL_CreateWindowAndRenderer(256, 224, 0, &win, &renderer);
    SDL_SetWindowSize(win, 256 * 2, 224 * 2);
    SDL_RenderSetLogicalSize(renderer, 256, 224);
    SDL_SetWindowResizable(win, SDL_TRUE);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 224);

    SDL_SetWindowTitle(win, "SupaNES");
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(win, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    HMENU hMenuBar = CreateMenu();
    HMENU hFile = CreateMenu();
    std::wstring sFile = L"File";
    LPCWSTR shFile = sFile.c_str();
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, shFile);
    std::wstring sExit = L"Exit";
    LPCWSTR shExit = sExit.c_str();
    AppendMenu(hFile, MF_STRING, 1, shExit);
    SetMenu(hwnd, hMenuBar);

    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    bool running = true;

    while (running) {

        SDL_Event event;

        // Events management
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_QUIT: {
                    // handling of close button
                    running = false;
                    break;
                }
                case SDL_SYSWMEVENT: {
                    if(event.syswm.msg->msg.win.msg == WM_COMMAND){
                        if (LOWORD(event.syswm.msg->msg.win.wParam) == 1) {
                            running = false;
                        }
                    }
                    
                    break;
                }
            }
        }
    }
    return 0;
}