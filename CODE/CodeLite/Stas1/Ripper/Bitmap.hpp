#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <stdexcept>
#include <utility>
#include <allegro.h>

struct Bitmap {
	BITMAP* bmp;
	
	Bitmap(int w, int h)
	{
		bmp = create_bitmap(w, h);
	}
	
	Bitmap(BITMAP* src)
	{
		bmp = src;
	}
	
	Bitmap(Bitmap const& other)
	{
		bmp = create_bitmap(other.getWidth(), other.getHeight());
		blit(other.bmp, bmp, 0, 0, 0, 0, other.getWidth(), other.getHeight());
		//printf("Copy constructor!");
	}
	
	Bitmap(Bitmap&& other)
	{
		bmp = other.bmp;
		other.bmp = nullptr;
		//printf("Move constructor!");
	}
	
	Bitmap& operator=(Bitmap&& other)
	{
		destroy_bitmap(bmp);
		bmp = other.bmp;
		other.bmp = nullptr;
		//printf("Move assignment!");
		return *this;
	}
	
	Bitmap& operator=(Bitmap const& other)
	{
		Bitmap tmp(other);
		*this = std::move(tmp);
		//printf("Copy assignment!");
		return *this;
	}
	
	~Bitmap()
	{
		//printf("Destroyed!");
		destroy_bitmap(bmp);
	}
	
	int getWidth() const
	{
		return bmp ? bmp->w : 0;
	}
	
	int getHeight() const
	{
		return bmp ? bmp->h : 0;
	}
};

#endif // BITMAP_HPP
