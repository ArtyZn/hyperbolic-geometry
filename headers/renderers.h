//
// Created by artem on 17.07.2022.
//

#ifndef OPENGL_RENDERERS_H
#define OPENGL_RENDERERS_H
#include <SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "vec3d.h"

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const double RADIUS = 200;
const Vec3D SCREENMID = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0};

Vec3D Vec3D::renderPrep() const {
    return Vec3D(SCREENMID.x + this->x * RADIUS, SCREENMID.y + this->y * RADIUS, this->z);
}

void drawCircle(SDL_Renderer *renderer, Vec3D p, Uint64 color) {
    auto q = p.renderPrep();
    filledCircleColor(renderer, q.x, q.y, 5, color);
}

bool drawArcShouldRender(double x, double y, int32_t radius, double start, double end) {
    if (end < start) return (atan2(x, y) > start || atan2(x, y) < end);
    return (atan2(x, y) > start && atan2(x, y) < end);
}

/* Works slow as hell, but at least it works for r > 1024 unlike SDL2_gfx */
void drawArc(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius, double start, double end) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
        if (drawArcShouldRender(x, y, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        if (drawArcShouldRender(y, x, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        if (drawArcShouldRender(-y, x, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);
        if (drawArcShouldRender(-x, y, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        if (drawArcShouldRender(-x, -y, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        if (drawArcShouldRender(-y, -x, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        if (drawArcShouldRender(y, -x, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        if (drawArcShouldRender(x, -y, radius, start, end))
            SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

Vec3D find_center(Vec3D p, Vec3D q) {
    double psum = p.x * p.x + p.y * p.y;
    double qsum = q.x * q.x + q.y * q.y;
    double denom = (2 * (p.x * q.y - p.y * q.x));

    double x0 = (q.y * (psum + 1) - p.y * (qsum + 1)) / denom;
    double y0 = (-q.x * (psum + 1) + p.x * (qsum + 1)) / denom;
    return {x0, y0, 0};
}

void drawGeodesic(SDL_Renderer * renderer, const Vec3D p, const Vec3D q) {
    //SDL_RenderDrawLine(renderer, p.renderPrep().x, p.renderPrep().y, q.renderPrep().x, q.renderPrep().y);
    //return;
    auto t = find_center(p, q);
    double x0 = t.x, y0 = t.y;
    auto c = t.renderPrep();

    auto start = atan2(p.x - x0, p.y - y0);
    auto end = atan2(q.x - x0, q.y - y0);

    if (start > end) std::swap(start, end);
    if (end - start > M_PI) std::swap(start, end);
    if (abs(2 * (p.x * q.y - p.y * q.x)) > 0.01) {
        drawArc(renderer, c.x, c.y, sqrt(x0 * x0 + y0 * y0 - 1) * RADIUS, start, end);
    } else {
        auto p1 = p.renderPrep();
        auto q1 = q.renderPrep();
        SDL_RenderDrawLine(renderer, p1.x, p1.y, q1.x, q1.y);
    }
}

#endif //OPENGL_RENDERERS_H
