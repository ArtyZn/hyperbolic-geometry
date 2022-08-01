#define print(v) for (auto i : v) cout << i << " "; cout << '\n';
#ifndef OPENGL_VEC3D_H
#define OPENGL_VEC3D_H
#include <cmath>
#include <vector>
using ld = long double;

struct Matrix {
    std::vector<std::vector<double>> m;

    explicit Matrix(std::vector<std::vector<double>> m_) {
        this->m = m_;
    }

    Matrix operator*(const Matrix &m) {
        int h1 = this->m.size(), w1 = this->m[0].size();
        int h2 = m.m.size(), w2 = m.m[0].size();

        if (w1 != h2) throw std::invalid_argument("Matrix sizes differ");
        std::vector<std::vector<double>> out(h1, std::vector<double>(w2, 0));
        for (int x = 0; x < w2; ++x)
            for (int y = 0; y < h1; ++y)
                for (int i = 0; i < w1; ++i) {
                    out[y][x] += this->m[y][i] * m.m[i][x];
                }
        return Matrix(out);
    }

    static Matrix rotX(double a) {
        return Matrix({
                              {1, 0, 0},
                              {0, cos(a), -sin(a)},
                              {0, sin(a), cos(a)}
                      });
    }

    static Matrix hrotX(double a) {
        return Matrix({
                              {1, 0, 0},
                              {0, cosh(a), sinh(a)},
                              {0, sinh(a), cosh(a)}
                      });
    }

    static Matrix rotY(double a) {
        return Matrix({
                              {cos(a), 0, sin(a)},
                              {0, 1, 0},
                              {-sin(a), 0, cos(a)}
                      });
    }

    static Matrix hrotY(double a) {
        return Matrix({
                              {cosh(a), 0, sinh(a)},
                              {0, 1, 0},
                              {sinh(a), 0, cosh(a)}
                      });
    }

    Matrix transpose() {
        std::vector<std::vector<double>> nm(this->m[0].size(), std::vector<double>(this->m.size(), 0));
        for (int i = 0; i < this->m.size(); ++i) {
            for (int j = 0; j < this->m[0].size(); ++j) {
                nm[j][i] = this->m[i][j];
            }
        }
        return Matrix(nm);
    }
};

struct Vec3D {
    double x = 0, y = 0, z = 0;
    Matrix asMatrix() {
        return Matrix({{x, y, z}});
    }

    Vec3D(Matrix m) {
        fillWithMatrix(m);
    }

    void fillWithMatrix(Matrix m) {
        x = m.m[0][0];
        y = m.m[0][1];
        z = m.m[0][2];
    }

    Vec3D(double x_, double y_, double z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    void rotateX(double a, bool reversed=false) {
        Matrix out = asMatrix() * Matrix::rotX(-a);
        fillWithMatrix(out);
    }

    void hrotateX(double a, bool reversed=false) {
        Matrix out = asMatrix() * Matrix::hrotX(-a);
        fillWithMatrix(out);
    }

    void rotateY(double a, bool reversed=false) {
        Matrix out = asMatrix() * Matrix::rotY(a);
        fillWithMatrix(out);
    }

    void hrotateY(double a, bool reversed=false) {
        Matrix out = asMatrix() * Matrix::hrotY(a);
        fillWithMatrix(out);
    }

    Vec3D copy() {
        return Vec3D(x, y, z);
    }

    double len() const {
        return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    }

    Vec3D rotated(double x_, double y_, bool reversed=false) {
        auto v = copy();
        v.rotateX(x_, reversed);
        v.rotateY(y_, reversed);
        return v;
    }

    Vec3D hrotated(double x_, double y_, bool reversed=false) {
        auto v = copy();
        v.hrotateX(x_, reversed);
        v.hrotateY(y_, reversed);
        return v;
    }

    Vec3D operator* (ld a) const {
        return Vec3D(x * a, y * a, z * a);
    }

    Vec3D operator/ (ld a) const {
        return Vec3D(x / a, y / a, z / a);
    }

    Vec3D operator+ (ld a) const {
        double d = this->len();
        return *this / d * (d + a);
    }

    Vec3D operator+ (Vec3D a) const {
        return {x + a.x, y + a.y, z + a.z};
    }

    Vec3D operator- (Vec3D a) const {
        return {x - a.x, y - a.y, z - a.z};
    }

    Vec3D operator- (ld a) const {
        double d = this->len();
        return *this / d * (d - a);
    }

    void operator+= (ld a) {
        double d = this->len();
        x = x / d * (d + a);
        y = y / d * (d + a);
        z = z / d * (d + a);
    }

    void operator-= (ld a) {
        double d = this->len();
        x = x / d * (d - a);
        y = y / d * (d - a);
        z = z / d * (d - a);
    }



    Vec3D renderPrep() const;
};

double dist3d(Vec3D a, Vec3D b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

double dist2d(Vec3D a, Vec3D b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

#endif //OPENGL_VEC3D_H
