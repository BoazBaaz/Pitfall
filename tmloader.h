#pragma once

#include <iostream>

class TMLoader {
public:
	// struct
    struct Tilemap {
        Tilemap(const char* filename) : file(filename) {}

        const char* file;
        const static int rows = 24, columns = 80;
        int map[rows * columns];
    };

	// special opperations
	void LoadTilemap(Tilemap& tilemap, char* delimiter);
	void DrawTilemap(Tilemap& tilemap, Sprite& tilesheet, Surface* screen, int x, int y);
};