#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <functional>
#include <allegro.h>

#include "Buffer.hpp"
#include "Cell.hpp"

class Document {
	Buffer<Cell> mCellBuff;
	int mCellSize;
	int mWidth, mHeight;
	
	// Returns the quotient of a and b, rounded upwards.
	static int iceil(int a, int b)
	{
		return a / b + bool(a % b);
	}
	
	int toI(int x) const
	{
		return x / mCellSize;
	}
	
	int toJ(int y) const
	{
		return y / mCellSize;
	}
	
	void modify(int x0, int y0, int x1, int y1, std::function<void(Cell&, int, int)> const& f)
	{
		int i0 = toI(x0), j0 = toJ(y0),
			i1 = toI(x1), j1 = toJ(y1);
		
		if(i1 < 0 || j1 < 0 || i0 >= mCellBuff.getWidth() || j0 >= mCellBuff.getHeight())
			return;
			
		i0 = mCellBuff.clampX(i0);
		j0 = mCellBuff.clampY(j0);
		i1 = mCellBuff.clampX(i1);
		j1 = mCellBuff.clampY(j1);
		
		for(int j = j0; j <= j1; ++j)
			for(int i = i0; i <= i1; ++i)
			{
				auto& cell = mCellBuff.at(i, j);
				cell.setModFlag();
				f(cell, i * mCellSize, j * mCellSize);
			}
	}
	
public:
	Document(BITMAP* src, int cellSize): mCellBuff(iceil(src->w, cellSize), iceil(src->h, cellSize)),
	mCellSize(cellSize), mWidth(src->w), mHeight(src->h)
	{
		int dw = src->w - cellSize * (mCellBuff.getWidth() - 1);
		int dh = src->h - cellSize * (mCellBuff.getHeight() - 1);
		
		for(int j = 0; j < mCellBuff.getHeight() - 1; ++j)
			for(int i = 0; i < mCellBuff.getWidth() - 1; ++i)
				mCellBuff.at(i, j).init(src, i * cellSize, j * cellSize, cellSize, cellSize);
				
		for(int i = 0, j = mCellBuff.getHeight() - 1; i < mCellBuff.getWidth() - 1; ++i)
			mCellBuff.at(i, j).init(src, i * cellSize, j * cellSize, cellSize, dh);
		
		for(int j = 0, i = mCellBuff.getWidth() - 1; j < mCellBuff.getHeight() - 1; ++j)
			mCellBuff.at(i, j).init(src, i * cellSize, j * cellSize, dw, cellSize);
		
		int i = mCellBuff.getWidth() - 1, j = mCellBuff.getHeight() - 1;
		mCellBuff.at(i, j).init(src, i * cellSize, j * cellSize, dw, dh);
	}
	
	int getWidth() const
	{
		return mWidth;
	}
	
	int getHeight() const
	{
		return mHeight;
	}
	
	void draw(BITMAP* dest, int x, int y) const
	{
		for(int j = 0; j < mCellBuff.getHeight(); ++j)
			for(int i = 0; i < mCellBuff.getWidth(); ++i)
				mCellBuff.at(i, j).draw(dest, x + i * mCellSize, y + j * mCellSize);
	}
	
	void beginWrite()
	{
		
	}
	
	void endWrite()
	{
		for(int j = 0; j < mCellBuff.getHeight(); ++j)
			for(int i = 0; i < mCellBuff.getWidth(); ++i)
				mCellBuff.at(i, j).clearModFlag();
	}
	
	int getPixel(int x, int y) const
	{
		int i = x / mCellSize;
		int j = y / mCellSize;
		
		return getpixel((BITMAP*)mCellBuff.at(i, j).getBitmap(), x - i * mCellSize, y - j * mCellSize);
	}
	
	void drawLine(int x1, int y1, int x2, int y2, int col)
	{
		modify(x1, y1, x2, y2, [=](Cell& cell, int ox, int oy)
		{
			::line(cell.getBitmap(), x1 - ox, y1 - oy, x2 - ox, y2 - oy, col);
		});
	}
	
	void drawCircle(int x, int y, int r, int col)
	{
		modify(x - r, y - r, x + r, y + r, [=](Cell& cell, int ox, int oy)
		{
			::circle(cell.getBitmap(), x - ox, y - oy, r, col);
		});
	}
	
	void fillCircle(int x, int y, int r, int col)
	{
		modify(x - r, y - r, x + r, y + r, [=](Cell& cell, int ox, int oy)
		{
			::circlefill(cell.getBitmap(), x - ox, y - oy, r, col);
		});
	}
	
	void drawRect(int x1, int y1, int x2, int y2, int col)
	{
		modify(x1, y1, x2, y2, [=](Cell& cell, int ox, int oy)
		{
			::rect(cell.getBitmap(), x1 - ox, y1 - oy, x2 - ox, y2 - oy, col);
		});
	}
	
	void fillRect(int x1, int y1, int x2, int y2, int col)
	{
		modify(x1, y1, x2, y2, [=](Cell& cell, int ox, int oy)
		{
			::rectfill(cell.getBitmap(), x1 - ox, y1 - oy, x2 - ox, y2 - oy, col);
		});
	}
	
	void drawSprite(BITMAP const* src, int x, int y)
	{
		modify(x, y, x + src->w - 1, y + src->h - 1, [=](Cell& cell, int ox, int oy)
		{
			::draw_sprite(cell.getBitmap(), (BITMAP*)src, x - ox, y - oy);
		});
	}
	
	void clear(int col=0)
	{
		modify(0, 0, mWidth - 1, mHeight - 1, [=](Cell& cell, int ox, int oy)
		{
			::clear_to_color(cell.getBitmap(), col);
		});
	}
	
	void blit(BITMAP const* src, int sx=0, int sy=0, int dx=0, int dy=0, int w=0, int h=0)
	{
		if(!w) w = src->w;
		if(!h) h = src->h;
		
		modify(dx, dy, dx + w - 1, dy + h - 1, [=](Cell& cell, int ox, int oy)
		{
			::blit((BITMAP*)src, cell.getBitmap(), sx, sy, dx - ox, dy - oy, w, h);
		});
	}
	
	void maskedBlit(BITMAP const* src, int sx=0, int sy=0, int dx=0, int dy=0, int w=0, int h=0)
	{
		if(!w) w = src->w;
		if(!h) h = src->h;
		
		modify(dx, dy, dx + w - 1, dy + h - 1, [=](Cell& cell, int ox, int oy)
		{
			::masked_blit((BITMAP*)src, cell.getBitmap(), sx, sy, dx - ox, dy - oy, w, h);
		});
	}
};

#endif // DOCUMENT_HPP
