// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once

namespace Tmpl8 {

	// basic sprite class
	class Sprite {
	public:
		// structors
		Sprite(Surface* surface, unsigned int frameCount);
		Sprite(Surface* surface, unsigned int rows, unsigned int columns);
		~Sprite();
		// methods
		void Draw(Surface* target, int x, int y);
		void Draw(Surface* target, int x, int y, int frame);
		void DrawScaled(Surface* target, int x, int y, int width, int height);
		void SetFlags(unsigned int f) { flags = f; }
		void SetFrame(unsigned int i) { currentFrame = i; }
		unsigned int GetFlags() const { return flags; }
		int GetWidth() { return width; }
		int GetHeight() { return height; }
		uint* GetBuffer() { return surface->pixels; }
		unsigned int Frames() { return numFrames; }
		Surface* GetSurface() { return surface; }
		void InitializeStartData();
	private:
		// attributes
		int width, height;
		const bool frames2D;
		unsigned int numColumns, numRows;
		unsigned int numFrames;
		unsigned int currentFrame;
		unsigned int flags;
		unsigned int** start;
		Surface* surface;
	};
}