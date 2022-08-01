#ifndef OPENGL_TILINGS_H
#define OPENGL_TILINGS_H
#include <vector>
#include <map>
#include <SDL.h>
#include "vec3d.h"
#include "renderers.h"

struct Tile {
    std::vector<Vec3D> vertices;
};

struct TesselationTree {
    int parent;
    std::vector<int> children;
};

class ITiling {
public:
    int _p = 0, _q = 0;
    std::vector<Tile> tiles;

    virtual void addTile (int index, int side) = 0;
    virtual void draw(SDL_Renderer * renderer, Matrix rot) = 0;

};

class Poincare73Tiling : public ITiling {
public:
    int _p = 7, _q = 3;
    Poincare73Tiling() {
        double d = sqrt((tan(M_PI / 2 - M_PI / _q) - tan(M_PI / _p)) / (tan(M_PI / 2 - M_PI / _q) + tan(M_PI / _p)));
        Vec3D start = {0, 0, 0};
        Tile t;
        double angle = M_PI - (_p - 2) * M_PI / _p;
        for (int i = 0; i < _p; ++i) {
            start.x = d * cos(i * angle);
            start.y = d * sin(i * angle);
            t.vertices.emplace_back(poincareToHyperbolic(start));
        }

        tiles.emplace_back(t);
        for (int i = 0; i < _p; ++i) addTile(0, i);
    }

    /* Yeah I'm kinda too lazy to look for a better way, so I convert Minkowski to Poincare,
       make a tile using inversions and convert it back to Minkowski space

       If you're brave enough to make a single formula for inversions on hyperbolic model, please do so. */
    void addTile(int index, int side) {
        Tile t = tiles[index];
        auto p1 = hyperbolicToPoincare(t.vertices[side]);
        auto p2 = hyperbolicToPoincare(t.vertices[(side + 1) % _p]);
        Vec3D O = find_center(p1, p2);
        double R = dist2d(O, p1);

        for (auto &v : t.vertices) {
            auto A = hyperbolicToPoincare(v);
            auto r = (A - O).len();
            auto r_ = R * R / r;
            auto A_ = (A - O) / (A - O).len() * r_ + O;
            v = poincareToHyperbolic(A_);
        }
        tiles.emplace_back(t);
    }

    void draw(SDL_Renderer * renderer, Matrix rot) {
        for (auto t : tiles) {
            for (int i = 0; i < t.vertices.size(); ++i) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                drawGeodesic(renderer,
                             hyperbolicToPoincare(Vec3D(t.vertices[i].asMatrix() * rot)),
                             hyperbolicToPoincare(Vec3D(t.vertices[(i + 1) % t.vertices.size()].asMatrix() * rot)));
            }
        }
    }
};

#endif //OPENGL_TILINGS_H
