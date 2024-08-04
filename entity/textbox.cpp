#include "textbox.h"
#include "raylib.h"

TextBox::TextBox() : promptMessage("Test")
{
	x = GetScreenWidth() / 4;
	y = GetScreenHeight() / 4;
	width = GetScreenWidth() / 2;
	height = GetScreenHeight() / 2;
}

TextBox::TextBox(std::string message) : promptMessage(message)
{
	x = GetScreenWidth() / 4;
	y = GetScreenHeight() / 4;
	width = GetScreenWidth() / 2;
	height = GetScreenHeight() / 2;
}

void TextBox::SetDimensions(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

void TextBox::Draw()
{
	BeginBlendMode(BLEND_ALPHA);
	DrawRectangle(x, y, width, height, Fade(DARKGRAY, fadeValue));
	DrawRectangle(x + 10, y + 10, width - 20, height - 20, Fade(WHITE, fadeValue));
	DrawText(promptMessage.c_str(), x + (width / 2) - (MeasureText(promptMessage.c_str(), 24) / 2), y + (height / 2) - 12, 24, Fade(BLACK, fadeValue));
	EndBlendMode();
}