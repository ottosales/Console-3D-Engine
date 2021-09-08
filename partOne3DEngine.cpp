#include <iostream>
#include "olcConsoleGameEngine.h"

#define PI 3.14159f

struct vect3 {
    float x;
    float y;
    float z;
};

struct triangle {
    vect3 point[3];
};

struct mesh {
    std::vector<triangle> triangleMesh;
};

struct matrix4x4 {
    float m[4][4] = { 0 };
};

class gameEngine3D : public olcConsoleGameEngine {
private:
    mesh cube;
    matrix4x4 projectionMatrix;
    float theta = 0;

    void MultiplyMatrixVector(vect3 &input, vect3 &output, matrix4x4 &mtx) {
        output.x = input.x * mtx.m[0][0] + input.y * mtx.m[1][0] + input.z * mtx.m[2][0] + mtx.m[3][0];
        output.y = input.x * mtx.m[0][1] + input.y * mtx.m[1][1] + input.z * mtx.m[2][1] + mtx.m[3][1];
        output.z = input.x * mtx.m[0][2] + input.y * mtx.m[1][2] + input.z * mtx.m[2][2] + mtx.m[3][2];
        float w =  input.x * mtx.m[0][3] + input.y * mtx.m[1][3] + input.z * mtx.m[2][3] + mtx.m[3][3];

        if (w != 0.0f) {
            output.x /= w;
            output.y /= w;
            output.z /= w;
        }
    }

public:
    gameEngine3D () {
        m_sAppName = L"3D testing";
    }

    bool OnUserCreate() override {
        cube.triangleMesh = {
            // south pannel
            {0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f},

            // east pannel
            {1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f},

            // north pannel
            {1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f},

            // west pannel
            {0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f},

            // top pannel
            {0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f},

            // bottom pannel
            {0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f}
        };

        float zFar = 1000.0f;
        float zNear = 0.1f;
        float fov = 90.0f;
        float aspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        float invTan = 1.0f / tanf(fov * 0.5f / 180.0f * PI);

        projectionMatrix.m[0][0] = aspectRatio * invTan;
        projectionMatrix.m[1][1] = invTan;
        projectionMatrix.m[2][2] = zFar / (zFar - zNear);
        projectionMatrix.m[3][2] = (-zFar * zNear) / (zFar - zNear);
        projectionMatrix.m[2][3] = 1.0f;
        // projectionMatrix.m[3][3] = aspectRatio * invTan;

        return true;
    }

    bool OnUserUpdate (float fElapsedTime) override {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        matrix4x4 matRotZ, matRotX;
        theta += 1.0f * fElapsedTime;
        matRotZ.m[0][0] = cosf(theta);
        matRotZ.m[0][1] = sinf(theta);
        matRotZ.m[1][0] = -sinf(theta);
        matRotZ.m[1][1] = cosf(theta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(theta);
        matRotX.m[1][2] = sinf(theta);
        matRotX.m[2][1] = -sinf(theta);
        matRotX.m[2][2] = cosf(theta);
        matRotX.m[3][3] = 1;

        for (auto triLoop : cube.triangleMesh) {
            triangle triangleTranslated;
            triangle triangleProjected;
            triangle triangleRotatedZ, triangleRotatedX;

            MultiplyMatrixVector(triLoop.point[0], triangleRotatedZ.point[0], matRotZ);
            MultiplyMatrixVector(triLoop.point[1], triangleRotatedZ.point[1], matRotZ);
            MultiplyMatrixVector(triLoop.point[2], triangleRotatedZ.point[2], matRotZ);

            MultiplyMatrixVector(triangleRotatedZ.point[0], triangleRotatedX.point[0], matRotX);
            MultiplyMatrixVector(triangleRotatedZ.point[1], triangleRotatedX.point[1], matRotX);
            MultiplyMatrixVector(triangleRotatedZ.point[2], triangleRotatedX.point[2], matRotX);


            triangleTranslated = triangleRotatedX;
            triangleTranslated.point[0].z += 3.0f;
            triangleTranslated.point[1].z += 3.0f;
            triangleTranslated.point[2].z += 3.0f;

            MultiplyMatrixVector(triangleTranslated.point[0], triangleProjected.point[0], projectionMatrix);
            MultiplyMatrixVector(triangleTranslated.point[1], triangleProjected.point[1], projectionMatrix);
            MultiplyMatrixVector(triangleTranslated.point[2], triangleProjected.point[2], projectionMatrix);

            // forgive me for these warcrimes
            triangleProjected.point[0].x += 1.0f;
            triangleProjected.point[0].y += 1.0f;
            triangleProjected.point[1].x += 1.0f;
            triangleProjected.point[1].y += 1.0f;
            triangleProjected.point[2].x += 1.0f;
            triangleProjected.point[2].y += 1.0f;

            triangleProjected.point[0].x *= 0.5f * (float)ScreenWidth();
            triangleProjected.point[0].y *= 0.5f * (float)ScreenHeight();
            triangleProjected.point[1].x *= 0.5f * (float)ScreenWidth();
            triangleProjected.point[1].y *= 0.5f * (float)ScreenHeight();
            triangleProjected.point[2].x *= 0.5f * (float)ScreenWidth();
            triangleProjected.point[2].y *= 0.5f * (float)ScreenHeight();


            DrawTriangle(triangleProjected.point[0].x, triangleProjected.point[0].y,
                triangleProjected.point[1].x, triangleProjected.point[1].y,
                triangleProjected.point[2].x, triangleProjected.point[2].y,
                PIXEL_SOLID, FG_WHITE);
        }

        return true;
    }
};

int main() {
    gameEngine3D partOne;

    if (partOne.ConstructConsole(256, 240, 3, 3)) {
        std::cout << "everything went right :)\n";
        partOne.Start();
    }
    else {
        std::cout << "careful, lad! something went wrong :(\n";
    }

    return 0;
}