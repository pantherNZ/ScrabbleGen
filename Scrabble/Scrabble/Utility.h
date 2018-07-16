#pragma once

#ifndef UTILITY_H
#define UTILITY_H

#define _USE_MATH_DEFINES
#define WIN32_LEAN_AND_MEAN

#pragma warning ( disable : 4005 )

// Includes
#include <Windows.h>
#include <WindowsX.h>
#include <d3dx10.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <vld.h>
#include <time.h>

// Definitions

// Enumerations

// Structs
struct Vertex
{
	D3DXVECTOR2 vecPosition;
	D3DXVECTOR2 vecTexCoord;
};

// Defined Functions
#define ReleaseCOM(x) { if(x) { x->Release(); x = nullptr; } }
#define DeleteOBJ(x) {if(x) { delete x; x = nullptr; } }
#define RandomFloat() (rand() / (RAND_MAX + 1.0f))

// Defined Values
#define CAMERA_ZOOM_SPEED 400 // Zoom speed of the camera
#define CAMERA_MOVE_SPEED 500 // Move speed of the camera
#define SCALE 0.15f
#define SQUARE_SIZE 105.0f * SCALE
#define SQUARE_DIST_DIAGONAL sqrt(2 * (SQUARE_SIZE * SQUARE_SIZE))

// Inline functions
inline float Round(float _fVal, float _fAccuracy)
{
	_fVal *= _fAccuracy;
	return(floor(_fVal + 0.5f) / _fAccuracy);
}

inline bool compareLen(const std::string& a, const std::string& b)
{
    return (a.size() > b.size()); 
}

#endif // UTILITY_H