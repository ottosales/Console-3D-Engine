#include <iostream>
#include <algorithm>
#include "olcConsoleGameEngine.h"

#define PI 3.14159f

struct vect3 {
    float x;
    float y;
    float z;
};

struct triangle {
    vect3 point[3];
    wchar_t sym;
    short col;
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

    vect3 camera;

    void MultiplyMatrixVector (vect3 &input, vect3 &output, matrix4x4 &mtx) {
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

    CHAR_INFO GetColour (float lum) {
        short bg_col, fg_col;
        wchar_t sym;
        int pixel_bw = (int)(13.0f * lum);
        switch (pixel_bw)
        {
        case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

        case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
        case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
        case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

        case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
        case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
        case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

        case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
        case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
        case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
        case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
        default:
            bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
        }

        CHAR_INFO c;
        c.Attributes = bg_col | fg_col;
        c.Char.UnicodeChar = sym;
        return c;
    }

public:
    gameEngine3D () {
        m_sAppName = L"3D testing";
    }

    bool OnUserCreate() override {
        cube.triangleMesh = {
            // south pannel
            { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

            // east pannel
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

            // north pannel
            { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

            // west pannel
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

            // top pannel
            { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

            // bottom pannel
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
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
        theta += 0.75f * fElapsedTime;
        matRotZ.m[0][0] = cosf(theta);
        matRotZ.m[0][1] = sinf(theta);
        matRotZ.m[1][0] = -sinf(theta);
        matRotZ.m[1][1] = cosf(theta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(theta * 0.5f);
        matRotX.m[1][2] = sinf(theta * 0.5f);
        matRotX.m[2][1] = -sinf(theta * 0.5f);
        matRotX.m[2][2] = cosf(theta * 0.5f);
        matRotX.m[3][3] = 1;

        std::vector<triangle> triangleVector;

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

            vect3 normal, line1, line2;
            line1.x = triangleTranslated.point[1].x - triangleTranslated.point[0].x;
            line1.y = triangleTranslated.point[1].y - triangleTranslated.point[0].y;
            line1.z = triangleTranslated.point[1].z - triangleTranslated.point[0].z;

            line2.x = triangleTranslated.point[2].x - triangleTranslated.point[0].x;
            line2.y = triangleTranslated.point[2].y - triangleTranslated.point[0].y;
            line2.z = triangleTranslated.point[2].z - triangleTranslated.point[0].z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            float normalLength = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= normalLength;
            normal.y /= normalLength;
            normal.z /= normalLength;

            if (normal.x * (triangleTranslated.point[0].x - camera.x) +
                normal.y * (triangleTranslated.point[0].y - camera.y) +
                normal.z * (triangleTranslated.point[0].z - camera.z) < 0.0f) {

                vect3 lightDirection = { 0.0f, 0.0f, -1.0f };
                float lightNormalized = sqrtf(lightDirection.x * lightDirection.x + lightDirection.y * lightDirection.y + lightDirection.z * lightDirection.z);
                lightDirection.x /= lightNormalized;
                lightDirection.y /= lightNormalized;
                lightDirection.z /= lightNormalized;

                float dotProduct = normal.x * lightDirection.x + normal.y * lightDirection.y + normal.z * lightDirection.z;
                CHAR_INFO c = GetColour(dotProduct);
                triangleTranslated.col = c.Attributes;
                triangleTranslated.sym = c.Char.UnicodeChar;

                MultiplyMatrixVector(triangleTranslated.point[0], triangleProjected.point[0], projectionMatrix);
                MultiplyMatrixVector(triangleTranslated.point[1], triangleProjected.point[1], projectionMatrix);
                MultiplyMatrixVector(triangleTranslated.point[2], triangleProjected.point[2], projectionMatrix);
                triangleProjected.col = triangleTranslated.col;
                triangleProjected.sym = triangleTranslated.sym;

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

                triangleVector.push_back(triangleProjected);
            }

            sort(triangleVector.begin(), triangleVector.end(), [](triangle& t1, triangle& t2) {
                float z1 = (t1.point[0].z + t1.point[1].z + t1.point[2].z) / 3.0f;
                float z2 = (t1.point[0].z + t1.point[1].z + t1.point[2].z) / 3.0f;
                return z1 > z2;
            });

            for (auto& triangleProjected : triangleVector) {

                FillTriangle(triangleProjected.point[0].x, triangleProjected.point[0].y,
                    triangleProjected.point[1].x, triangleProjected.point[1].y,
                    triangleProjected.point[2].x, triangleProjected.point[2].y,
                    triangleProjected.sym, triangleProjected.col);

               /* DrawTriangle(triangleProjected.point[0].x, triangleProjected.point[0].y,
                    triangleProjected.point[1].x, triangleProjected.point[1].y,
                    triangleProjected.point[2].x, triangleProjected.point[2].y,
                    PIXEL_SOLID, FG_WHITE);*/
            }
        }
        return true;
    }
};

int main() {
    gameEngine3D partTwo;

    if (partTwo.ConstructConsole(256, 240, 3, 3)) {
        std::cout << "everything went right :)\n";
        partTwo.Start();
    }
    else {
        std::cout << "something went wrong :(\n";
    }

    return 0;
}