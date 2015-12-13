#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <vector>

#include <allegro.h>

#include "Vec2f.hpp"
#include "Mat9f.hpp"
#include "Buffer.hpp"
#include "Keyboard.hpp"

using namespace Util;

BITMAP* buffer = NULL;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     install_timer();
     
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     srand(time(NULL));
     
     //set_alpha_blender();
     //drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     //srand(time(NULL));
     //install_int(tick, 17);
}

void deinit()
{
    destroy_bitmap(buffer);
}

int mulCol(int col, float x)
{
    return makecol(clamp255(int(getr(col) * x)),
                   clamp255(int(getg(col) * x)),
                   clamp255(int(getb(col) * x)));
}

struct Block {
   int mColor;
   
   Block(int color=0): mColor(color)
   {
   }
};

struct Shape {
       std::vector<Buffer<Block>* > mBuffVec;
       
       Shape(int w, int h, int n)
       {
           for(int i = 0; i < n; ++i)
           {
              Buffer<Block>* t = new Buffer<Block>(w, h);
              t->fill(0);
              mBuffVec.push_back(t);
           }
       }
       
       ~Shape()
       {
           for(int i = 0; i < mBuffVec.size(); ++i)
              delete mBuffVec[i];
       }
       
       Buffer<Block> const& getBuff(int i) const
       {
           i = wrapi(i, mBuffVec.size());
           return *mBuffVec[i];
       }
       
       Buffer<Block>& getBuff(int i)
       {
           i = wrapi(i, mBuffVec.size());
           return *mBuffVec[i];
       }
};

struct Row {
       bool unbroken;
       float countdown;
       
       Row(): unbroken(false), countdown(3)
       {
       }
};

struct Game {
       static const int SHAPE_NUM = 7;
       static const int BLOCK_SIZE = 20;
       
       //std::vector<
       Keyboard& mKeyboard;
       
       Buffer<Block> mField;
       Shape* mShapeArr[SHAPE_NUM];
       
       Shape* mCurrShape;
       float mPosX, mPosY;
       int mAng;
       
       // This doesn't do any bounds checking!
       static void putRotatedBlockBuffer(Buffer<Block>& dest, Buffer<Block> const& src, int dx, int dy, float ang)
       {
            bool swapXy[4] = {0, 1, 0, 1};
            bool flipX[4] = {0, 0, 1, 1};
            bool flipY[4] = {0, 1, 1, 0};
            int i = wrapi(int(ang / 90), 4);
            
            int tw = swapXy[i] ? src.getHeight() : src.getWidth();
            int th = swapXy[i] ? src.getWidth() : src.getHeight();
            Mat9f mat;
            
            mat.translate(flipX[i] ? (src.getWidth() - 1) : 0, flipY[i] ? (src.getHeight() - 1) : 0);
            mat.scale(flipX[i] ? -1 : 1, flipY[i] ? -1 : 1);
            
            if(swapXy[i])
               mat.swapXy();
            
            int imat[6] = {Util::round(mat(0, 0)), Util::round(mat(1, 0)), Util::round(mat(2, 0)),
                           Util::round(mat(0, 1)), Util::round(mat(1, 1)), Util::round(mat(2, 1))};
            
            for(int y = 0; y < th; ++y)
               for(int x = 0; x < tw; ++x)
               {
                  Block b = src.at(x * imat[0] + y * imat[1] + imat[2],
                                   x * imat[3] + y * imat[4] + imat[5]);
                  
                  if(b.mColor)
                     dest.at(dx + x, dy + y) = b;
               }
       }
       
       static bool overlapping(Buffer<Block> const& dest, Buffer<Block> const& src, int dx0, int dy0)
       {
         int sx0 = 0, sy0 = 0;
         int sx1 = src.getWidth() - 1, sy1 = src.getHeight() - 1;
         int dx1 = dx0 + sx1 - sx0;
         int dy1 = dy0 + sy1 - sy0;
         
         int tx = sx0 - dx0;
         int ty = sy0 - dy0;
         
         if(dx1 < 0 || dy1 < 0 || dx0 > dest.getWidth() - 1 || dy0 > dest.getHeight() - 1)
                return false;
         
         dx0 = dest.clampX(dx0);
         dx1 = dest.clampX(dx1);
         dy0 = dest.clampY(dy0);
         dy1 = dest.clampY(dy1);
         
         for(int y = dy0; y <= dy1; ++y)
            for(int x = dx0; x <= dx1; ++x)
               if(dest.at(x, y).mColor && src.at(x + tx, y + ty).mColor)
                  return true;
         
         return false;
       }
       
       void initShapes()
       {
            Buffer<Block> temp;
            
            mShapeArr[0] = new Shape(2, 2, 1);
            temp.reset(2, 2);
            temp.fill(makecol(255, 50, 50));
            putRotatedBlockBuffer(*mShapeArr[0]->mBuffVec[0], temp, 0, 0, 0);
            
            mShapeArr[1] = new Shape(3, 3, 4);
            temp.reset(3, 2);
            temp.fill(makecol(50, 255, 50));
            temp.at(0, 0) = Block();
            temp.at(2, 0) = Block();
            putRotatedBlockBuffer(mShapeArr[1]->getBuff(0), temp, 0, 0, 0);
            putRotatedBlockBuffer(mShapeArr[1]->getBuff(1), temp, 1, 0, 90);
            putRotatedBlockBuffer(mShapeArr[1]->getBuff(2), temp, 0, 1, 180);
            putRotatedBlockBuffer(mShapeArr[1]->getBuff(3), temp, 0, 0, 270);
            
            mShapeArr[2] = new Shape(4, 4, 2);
            temp.reset(4, 1);
            temp.fill(makecol(100, 100, 255));
            putRotatedBlockBuffer(mShapeArr[2]->getBuff(0), temp, 0, 0, 0);
            putRotatedBlockBuffer(mShapeArr[2]->getBuff(1), temp, 1, 0, 90);
            
            mShapeArr[3] = new Shape(3, 3, 4);
            temp.reset(3, 2);
            temp.fill(makecol(255, 255, 50));
            temp.at(0, 1) = Block();
            temp.at(1, 1) = Block();
            putRotatedBlockBuffer(mShapeArr[3]->getBuff(0), temp, 0, 1, 0);
            putRotatedBlockBuffer(mShapeArr[3]->getBuff(1), temp, 0, 0, 90);
            putRotatedBlockBuffer(mShapeArr[3]->getBuff(2), temp, 0, 0, 180);
            putRotatedBlockBuffer(mShapeArr[3]->getBuff(3), temp, 1, 0, 270);
            
            mShapeArr[4] = new Shape(3, 3, 4);
            temp.reset(3, 2);
            temp.fill(makecol(100, 200, 255));
            temp.at(1, 1) = Block();
            temp.at(2, 1) = Block();
            putRotatedBlockBuffer(mShapeArr[4]->getBuff(0), temp, 0, 1, 0);
            putRotatedBlockBuffer(mShapeArr[4]->getBuff(1), temp, 0, 0, 90);
            putRotatedBlockBuffer(mShapeArr[4]->getBuff(2), temp, 0, 0, 180);
            putRotatedBlockBuffer(mShapeArr[4]->getBuff(3), temp, 1, 0, 270);
            
            mShapeArr[5] = new Shape(3, 3, 2);
            temp.reset(3, 2);
            temp.fill(makecol(255, 125, 255));
            temp.at(0, 0) = Block();
            temp.at(2, 1) = Block();
            putRotatedBlockBuffer(mShapeArr[5]->getBuff(0), temp, 0, 0, 0);
            putRotatedBlockBuffer(mShapeArr[5]->getBuff(1), temp, 1, 0, 90);
            
            mShapeArr[6] = new Shape(3, 3, 2);
            temp.reset(3, 2);
            temp.fill(makecol(255, 128, 20));
            temp.at(0, 1) = Block();
            temp.at(2, 0) = Block();
            putRotatedBlockBuffer(mShapeArr[6]->getBuff(0), temp, 0, 0, 0);
            putRotatedBlockBuffer(mShapeArr[6]->getBuff(1), temp, 1, 0, 90);
       }
       
       void clearField()
       {
            mField.fill(0);
            
            for(int i = 0; i < mField.getWidth(); ++i)
               mField.at(i, mField.getHeight() - 1) = makecol(128, 128, 128);
               
            for(int i = 0; i < mField.getHeight(); ++i)
            {
               mField.at(0, i) = makecol(128, 128, 128);
               mField.at(mField.getWidth() - 1, i) = makecol(128, 128, 128);
            }
       }
       
       void dropShape()
       {
            int i = int(rand() / float(RAND_MAX) * SHAPE_NUM);
            mAng = 0;
            mCurrShape = mShapeArr[i];
            mPosX = int((mField.getWidth() - mCurrShape->getBuff(mAng).getWidth()) / 2);
            mPosY = (i == 3 || i == 4) ? -1.1 : 0;
       }
       
       Game(Keyboard& keyboard, int w, int h): mKeyboard(keyboard), mField(w, h)
       {
           clearField();
           initShapes();
           dropShape();
       }
       
       static void drawBlock(BITMAP* dest, int x0, int y0, int w, int h, int s, int col)
       {    
            int r = getr(col), g = getg(col), b = getb(col);
     
            rectfill(buffer, x0, y0, x0 + w - 1, y0 + h - 1, col);
     
            for(int i = 0; i < s; ++i)
            {
                hline(buffer, x0 + i, y0 + i, x0 + w - 1 - i, mulCol(col, 0.9));
                hline(buffer, x0 + i, y0 + h - 1 - i, x0 + w - 1 - i, mulCol(col, 0.65));
                vline(buffer, x0 + i, y0 + i, y0 + h - 1 - i, mulCol(col, 0.8));
                vline(buffer, x0 + w - 1 - i, y0 + i, y0 + h - 1 - i, mulCol(col, 0.8));
            }
       }
       
       static void drawBlockBuffer(BITMAP* dest, Buffer<Block> const& bb, int x, int y)
       {
            int s = BLOCK_SIZE;
        
            for(int j = 0; j < bb.getHeight(); ++j)
               for(int i = 0; i < bb.getWidth(); ++i)
               {
                   if(bb.at(i, j).mColor)
                   {
                      drawBlock(dest, x + i * s, y + j * s, s, s, 3, bb.at(i, j).mColor);
                      
                      rect(dest,   x + i * s, y + j * s,
                                   x + (i + 1) * s, y + (j + 1) * s, 0);
                   }
               }
       }
       
       void removeRow(int y)
       {
            for(int j = y; j > 0; --j)
              for(int i = 1; i < mField.getWidth() - 1; ++i)
               mField.at(i, j) = mField.at(i, j - 1);
            
            for(int i = 1; i < mField.getWidth() - 1; ++i)
               mField.at(i, 0) = 0;
       }
       
       void removeUnbrokenRows()
       {
            for(int j = mField.getHeight() - 2; j > -1; --j)
            {
              bool unbroken = true;
             
              for(int i = 1; i < mField.getWidth() - 1 && unbroken; ++i)
                 if(!mField.at(i, j).mColor)
                   unbroken = false;
              
              if(unbroken)
              {
                 removeRow(j);
                 ++j;
              }
            }
       }
       
       void freezeShape()
       {
            putRotatedBlockBuffer(mField, mCurrShape->getBuff(mAng), int(mPosX), int(mPosY - 1), 0);
            dropShape();
            removeUnbrokenRows();
       }
       
       void logic(float dt)
       {
            int px = int(mPosX), py = int(mPosY);
            Buffer<Block>* sbuff = &mCurrShape->getBuff(mAng);
            
            if(mKeyboard.pressed(KEY_SPACE))
              removeRow(mField.getHeight() - 2);
            
            if(mKeyboard.pressed(KEY_LEFT) && !overlapping(mField, *sbuff, px - 1, py))
            {
              px = int(--mPosX);
            }
            
            if(mKeyboard.pressed(KEY_RIGHT) && !overlapping(mField, *sbuff, px + 1, py))
            {
              px = int(++mPosX);
            }
            
            if(mKeyboard.held(KEY_DOWN))
            {
               py = int(mPosY += 3 * dt);
               if(overlapping(mField, *sbuff, px, py))
                  freezeShape();
            }
            
            if(mKeyboard.pressed(KEY_UP) && !overlapping(mField, mCurrShape->getBuff(mAng + 1), px, py + 1))
            {
               sbuff = &mCurrShape->getBuff(mAng + 1);
               ++mAng;
            }
            
            py = int(mPosY += 0.25 * dt);
            
            if(overlapping(mField, *sbuff, px, py))
            {
                freezeShape();
            }
       }
       
       void draw(BITMAP* dest)
       {
            int x = 10, y = 10;
            
            drawBlockBuffer(dest, mCurrShape->getBuff(mAng), x + int(mPosX) * BLOCK_SIZE, y + int(mPosY) * BLOCK_SIZE); 
            drawBlockBuffer(dest, mField, x, y);
            //rect(dest, x, y, x + BLOCK_SIZE * mField.getWidth(), y + BLOCK_SIZE * mField.getHeight(), 0);
       }
};


int main()
{   
    bool exit = false;
    Keyboard keyboard;
    
    init();
    
    Game game(keyboard, 12, 22);
    BITMAP* bitmap = load_bitmap("test.bmp", NULL);

    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     game.logic(0.01);
     keyboard.update();
     
     clear_to_color(buffer, 0);
     game.draw(buffer);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    return 0;
}END_OF_MAIN()
