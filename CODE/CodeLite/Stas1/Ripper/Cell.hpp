#ifndef CELL_HPP
#define CELL_HPP

#include <memory>
#include <allegro.h>

#include "Bitmap.hpp"

class Cell {
	std::shared_ptr<Bitmap> mData;
	bool mModFlag;
	
public:
	Cell(): mModFlag(false)
	{
	}
	
	BITMAP* getBitmap()
	{
		return mData->bmp;
	}
	
	BITMAP* getBitmap() const
	{
		return mData->bmp;
	}
	
	void setModFlag()
	{
		if(!mModFlag) // Copy on first write.
			mData.reset(new Bitmap(*mData));
		mModFlag = true;
	}
	
	void clearModFlag()
	{
		mModFlag = false;
	}
	
	void init(BITMAP* src, int sx, int sy, int w, int h)
	{
		mModFlag = false;
		mData.reset(new Bitmap(w, h));
		::blit(src, mData->bmp, sx, sy, 0, 0, w, h);
	}
	
	void draw(BITMAP* dest, int x, int y) const
	{
		::draw_sprite(dest, mData->bmp, x, y);
	}
};

#endif // CELL_HPP
