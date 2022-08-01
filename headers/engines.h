//
// Created by artem on 16.07.2022.
//

#ifndef OPENGL_ENGINES_H
#define OPENGL_ENGINES_H
#include <random>
#include <list>
#include "vec3d.h"

enum GeomType {
    EUCLIDEAN,
    RIEMANNIAN,
    HYPERBOLIC
};

Vec3D poincareToHyperbolic(Vec3D v) {
    double x = (-2 * v.x) / (v.x * v.x + v.y * v.y - 1);
    double y = (-2 * v.y) / (v.x * v.x + v.y * v.y - 1);
    double z = (-pow(v.x, 2) - pow(v.y, 2) - 1) / (pow(v.x, 2) + pow(v.y,2) - 1);
    return {x, y, z};
}

Vec3D hyperbolicToPoincare(Vec3D v) {
    return Vec3D(v.x / (1 + v.z), v.y / (1 + v.z), 0);
}

class IGeometry {
public:
    GeomType type;
    std::list<Vec3D> snake;
    Matrix rrot = Matrix({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    Matrix rot = Matrix({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    Vec3D apple = {0, 0, 0};
    virtual void moveX(double a) = 0;
    virtual void moveY(double a) = 0;
    virtual void addPoint() = 0;
    virtual bool canMove(double dx, double dy) = 0;
    virtual void spawnApple() = 0;
    virtual Vec3D toEuclidean(Vec3D p) = 0;
};

class EuclideanGeometry: public IGeometry {
public:
    EuclideanGeometry() {
        type = EUCLIDEAN;
        addPoint();
    }

    void moveX(double a) override {
        snake.push_front(snake.front());
        snake.front().x -= a;
        snake.pop_back();
    }

    void moveY(double a) override {
        snake.push_front(snake.front());
        snake.front().y -= a;
        snake.pop_back();
    }

    void addPoint() override {
        if (snake.empty()) snake.push_back({0, 0, 0});
        else snake.push_back(snake.back());
    }

    bool canMove(double dx, double dy) override {
        return true;
    };

    void spawnApple() override {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> randomDeg(0,200);
        double phi = (double)randomDeg(rng);
        double theta = (double)randomDeg(rng);
        apple = {phi - 100, theta - 100, 0};
    }

    Vec3D toEuclidean(Vec3D p) {
        return p;
    }
};

class RiemannianGeometry: public IGeometry {
public:
    RiemannianGeometry() {
        type = RIEMANNIAN;
        addPoint();
    }

    void moveX(double a) override {
        snake.push_front(Vec3D(0, 0, 1).asMatrix() * rrot);
        //snake.front().rotateY(a / 100);

        rrot = Matrix::rotY(a / 100) * rrot;
        rot = rot * Matrix::rotY(-a / 100);

        snake.pop_back();
    }

    void moveY(double a) override {
        snake.push_front(Vec3D(0, 0, 1).asMatrix() * rrot);
        //snake.front().rotateX(a / 100);

        rrot = Matrix::rotX(-a / 100) * rrot;
        rot = rot * Matrix::rotX(a / 100);

        snake.pop_back();
    }

    void addPoint() override {
        if (snake.empty()) snake.push_back({0, 0, 1});
        else snake.push_back(snake.back());
    }

    bool canMove(double dx, double dy) override {
        return true;
    };

    void spawnApple() override {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> randomDeg(0,360);
        double a = (double)randomDeg(rng);
        double b = (double)randomDeg(rng);
        apple = Vec3D({0, 0, 1}).rotated(a, b);
    }

    Vec3D toEuclidean(Vec3D p) {
        return p;
    }
};

class PoincareDiskGeometry: public IGeometry {
public:
    PoincareDiskGeometry() {
        type = HYPERBOLIC;
        addPoint();
    }

    void moveX(double a) override {
        snake.push_front(Vec3D(0, 0, 1).asMatrix() * rrot);
        //snake.front().rotateY(a / 100);

        rrot = Matrix::hrotY(-a / 100) * rrot;
        rot = rot * Matrix::hrotY(a / 100);

        snake.pop_back();
    }

    void moveY(double a) override {
        snake.push_front(Vec3D(0, 0, 1).asMatrix() * rrot);
        //snake.front().rotateX(a / 100);

        rrot = Matrix::hrotX(-a / 100) * rrot;
        rot = rot * Matrix::hrotX(a / 100);

        snake.pop_back();
    }

    void addPoint() override {
        if (snake.empty()) snake.emplace_back(Vec3D(0, 0, 1));
        else snake.push_back(snake.back());
    }

    bool canMove(double dx, double dy) override {
        return true;
    };

    void spawnApple() override {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> randomDeg(0,360);
        double a = (double)randomDeg(rng);
        double b = (double)randomDeg(rng);
        apple = Vec3D({0, 0, 1}).hrotated(a * M_PI / 180, b * M_PI / 180);
    }

    Vec3D toEuclidean(Vec3D p) {
        return hyperbolicToPoincare(p);
    }
};

#endif //OPENGL_ENGINES_H
