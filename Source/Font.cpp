#include "Font.h"

Font::Font(std::string path, int fontSize)
{
	mFontSize = fontSize;
	mpSDLFont = TTF_OpenFont(path.c_str(), mFontSize);

	if (!mpSDLFont)
		std::cout << "Could not create font " << path << ": " << SDL_GetError() << std::endl;
}