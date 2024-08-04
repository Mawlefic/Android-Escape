#pragma once
#include <string>

class TextBox {
public:
	// attributes
	std::string promptMessage;
	int x, y, width, height;
	float fadeValue = 0.0f;
	bool enabled = false;

	// functions
	TextBox();
	TextBox(std::string message);
	void SetDimensions(int x, int y, int width, int height);
	void Draw();
};