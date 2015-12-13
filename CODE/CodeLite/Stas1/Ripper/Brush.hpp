#ifndef BRUSH_HPP
#define BRUSH_HPP

#include <allegro.h>

enum BrushShape {
	BRUSH_CIRCLE, BRUSH_RECT
};

class Brush
{
	static const int MAX_SIZE = 201;
	
	BITMAP* mBmp;
	BrushShape mShape;
	float mSize;
	int mCol;
	
	void refresh()
	{
		int l = MAX_SIZE / 2;
		int r = int(mSize) / 2;
		
		clear_to_color(mBmp, makecol(255, 0, 255));
		
		switch(mShape)
		{
			case BRUSH_CIRCLE:
				circlefill(mBmp, l, l, r, mCol);
				break;
			
			case BRUSH_RECT:
				rectfill(mBmp, l - r, l - r, l + r, l + r, mCol);
				break;
		}
	}
	
	void clampSize()
	{
		if(mSize > MAX_SIZE) mSize = MAX_SIZE;
		else if(mSize < 1) mSize = 1;
	}
	
public:
	Brush(): mShape(BRUSH_CIRCLE), mSize(10), mCol(0x00FFFFFF)
	{
		mBmp = create_bitmap(MAX_SIZE, MAX_SIZE);
		refresh();
	}
	
	~Brush()
	{
		destroy_bitmap(mBmp);
	}
	
	void setShape(BrushShape shape)
	{
		mShape = shape;
		refresh();
	}
	
	float getMaxSize() const
	{
		return MAX_SIZE;
	}
	
	void setSize(float size)
	{
		mSize = size;
		clampSize();
		refresh();
	}
	
	float getSize() const
	{
		return mSize;
	}
	
	void grow(float k)
	{
		mSize += sqrt(mSize) * k;
		clampSize();
		refresh();
	}
	
	void setCol(int col)
	{
		mCol = col;
		refresh();
	}
	
	int getCol() const
	{
		return mCol;
	}
	
	BITMAP const* getBmp() const
	{
		return mBmp;
	}
	
	void draw(BITMAP* dest, int x, int y) const
	{
		draw_sprite(dest, mBmp, x - MAX_SIZE / 2, y - MAX_SIZE / 2);
	}
};

#endif // BRUSH_HPP
