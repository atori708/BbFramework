#pragma once

struct RECT_SIZE
{
	unsigned int width, height;
};

enum class WINDOW_SIZE_TYPE
{
	SIZE_FREE,		// 未設定(サイズ自由)
	SIZE_800x600,
	SIZE_640x480,
	SIZE_1280x720,
	SIZE_1980x1080,
};