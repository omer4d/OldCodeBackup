/* 
 * File:   SpriteSheet.hpp
 * Author: Stas
 *
 * Created on June 19, 2012, 5:37 AM
 */

#ifndef SPRITESHEET_HPP
#define	SPRITESHEET_HPP

#include <vector>
#include <string>
#include <fstream>

#include "Texture.hpp"
#include "Frame.hpp"

class SpriteSheet {
    Texture tex;
    std::vector<Frame> frameVec;
    
public:
    SpriteSheet()
    {
        frameVec.push_back(Frame(Vec2f(0, 0), Vec2f(1, 1), Vec2f(0, 0), Vec2f(16, 16)));
    }
    
    SpriteSheet(char const* path): tex(path)
    {
        char buffer[500];
        
        // Try to load frame map:
        replace_extension(buffer, path, "smp", 500);
        std::fstream file;
        file.open(buffer, std::fstream::in);
        
        if(!file.is_open()) // Assume the whole image is a single frame.
        {
            Frame frame(tex.getTexCoords(0, 0),
                        tex.getTexCoords(tex.getWidth(), tex.getHeight()),
                        Vec2f(0, 0), Vec2f(tex.getWidth(), tex.getHeight()));
            frameVec.push_back(frame);
        }
        
        else
        {
            int fieldsRead = 0;
            int minx, miny, maxx, maxy;

            while(file.good())
            {
                int t = 0;
                t += (file >> minx) != 0;
                t += (file >> miny) != 0;
                t += (file >> maxx) != 0;
                t += (file >> maxy) != 0;
                fieldsRead += t;

                if(t == 4)
                {
                    Frame frame(tex.getTexCoords(minx, miny),
                                tex.getTexCoords(maxx + 1, maxy + 1),
                                Vec2f(minx, miny), Vec2f(maxx, maxy));
                    frameVec.push_back(frame);
                }
            }

            file.close();
            if(fieldsRead > 0 && fieldsRead % 4)
                throw std::runtime_error(std::string("Could not parse spritesheet map file: '") + buffer + "'");
        }
        
        // Try to load pivot file:
        
        replace_extension(buffer, path, "pvt", 500);
        file.open(buffer, std::fstream::in);

        int fieldsRead = 0;
        std::string name;
        int px, py;
        
        while(file.good())
        {
            int t = 0;
            
            t += (file >> name) != 0;
            t += (file >> px) != 0;
            t += (file >> py) != 0;
            fieldsRead += t;
            
            if(t == 3)
            {
                Vec2f v = Vec2f(px, py);
                
                for(int i = 0; i < frameVec.size(); i++)
                    if(frameVec[i].contains(v))
                    {
                        frameVec[i].addPivot(name, Pivot(v - frameVec[i].getMin()));
                        break;
                    }
            }
        }
        
        file.close();
        if(fieldsRead > 0 && fieldsRead % 3)
            throw std::runtime_error(std::string("Could not parse spritesheet pivot file: '") + buffer + "'");
    }
    
    Texture const& getTexture() const
    {
        return tex;
    }
    
    Frame const& getFrame(float n) const
    {
        return frameVec[int(n) % frameVec.size()];
    }
    
    int getFrameNum() const
    {
        return frameVec.size();
    }
};

#endif	/* SPRITESHEET_HPP */

