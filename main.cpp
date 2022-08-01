#include <SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <iostream>
#include "headers/vec3d.h"
#include "headers/engines.h"
#include "headers/renderers.h"
#include "headers/tilings.h"

using namespace std;

IGeometry* geom;
Poincare73Tiling tiling = Poincare73Tiling();

void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    circleColor(renderer, SCREENMID.x, SCREENMID.y, 200, 0xff000000);
    auto apple = Vec3D(geom->apple.asMatrix() * geom->rot);

    if (apple.z < 0) {
        drawCircle(renderer, geom->toEuclidean(apple), 0xff008000);
    }

    for (auto s : geom->snake) {
        auto v = Vec3D(s.asMatrix() * geom->rot);
        if (v.z < 0) drawCircle(renderer, geom->toEuclidean(v), 0xff808080);
    }

    for (auto s : geom->snake) {
        auto v = Vec3D(s.asMatrix() * geom->rot);
        if (v.z >= 0) drawCircle(renderer, geom->toEuclidean(v), 0xff000000);
    }

    if (apple.z >= 0) {
        drawCircle(renderer, geom->toEuclidean(apple), 0xff00ff00);
    }

    if (geom->type == HYPERBOLIC) {
        tiling.draw(renderer, geom->rot);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
    geom = new PoincareDiskGeometry();
    geom->spawnApple();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Window* gWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    if (!(gWindow || gRenderer)) return -1;

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    geom->addPoint();
                }
            }
        }
        if (!running) break;

        const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
        if (keystate[SDL_SCANCODE_UP])
            geom->moveY(1);
        if (keystate[SDL_SCANCODE_DOWN])
            geom->moveY(-1);
        if (keystate[SDL_SCANCODE_LEFT])
            geom->moveX(1);
        if (keystate[SDL_SCANCODE_RIGHT])
            geom->moveX(-1);
        if ((keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_RIGHT]) && keystate[SDL_SCANCODE_SPACE])
            geom->addPoint();

        if (keystate[SDL_SCANCODE_0]) {
            geom->snake.clear();
            geom->addPoint();
        }
        if (keystate[SDL_SCANCODE_9]) {
            cout << dist3d(geom->toEuclidean(geom->snake.front().asMatrix() * geom->rot), geom->toEuclidean(geom->apple.asMatrix() * geom->rot)) << endl;
        }
        if (keystate[SDL_SCANCODE_1]) {
            geom = new EuclideanGeometry();
            geom->spawnApple();
        }
        if (keystate[SDL_SCANCODE_2]) {
            geom = new RiemannianGeometry();
            geom->spawnApple();
        }
        if (keystate[SDL_SCANCODE_3]) {
            geom = new PoincareDiskGeometry();
            geom->spawnApple();
        }

        render(gRenderer);
        if (dist3d(geom->toEuclidean(geom->snake.front().asMatrix() * geom->rot).renderPrep(), geom->toEuclidean(geom->apple.asMatrix() * geom->rot).renderPrep()) <= 10) {
            geom->spawnApple();
            geom->addPoint();
        }
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}