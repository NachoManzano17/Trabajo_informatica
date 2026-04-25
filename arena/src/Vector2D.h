// Vector2D.h
#pragma once

class Vector2D {
public:
    float x;
    float y;

    Vector2D(float xv = 0.0f, float yv = 0.0f);
    virtual ~Vector2D();


    float modulo();
    float argumento();
    Vector2D unitario();
};