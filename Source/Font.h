#ifndef FONT_H
#define FONT_H

#include <SDL_ttf.h>

#include <iostream>
#include <string>


class Font
{
	friend class GraphicsSystem;
public:
	Font(std::string path, int fontSize = 12);
	Font(){};
	~Font(){ }//TTF_CloseFont(mpSDLFont); };
	void cleanup(){ TTF_CloseFont(mpSDLFont); };

	inline int getFontSize(){ return mFontSize; };
	inline TTF_Font* getFont(){ return mpSDLFont; };

protected:
	TTF_Font* mpSDLFont;
	int mFontSize;
};

#endif
