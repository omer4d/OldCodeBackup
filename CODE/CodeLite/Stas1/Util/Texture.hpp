/* 
 * File:   Texture.hpp
 * Author: Stas
 *
 * Created on June 14, 2012, 8:13 PM
 */

#ifndef TEXTURE_HPP
#define	TEXTURE_HPP

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glu.h>

#include <stack>
#include <string>
#include <stdexcept>
#include <sstream>

#include <Vec2f.hpp>

#include "Buffer.hpp"
#include "Col32.hpp"
#include "ImageLoader.hpp"

struct TextureSettings {
    unsigned int flags;
    Col32 maskColor;
    
    TextureSettings()
    {
        flags = 0;
        maskColor = 0;
    }
    
    TextureSettings(unsigned int flags, Col32 maskColor)
    {
        this->flags = flags;
        this->maskColor = maskColor;
    }
};

struct TextureUnitSettings {
    GLuint texId;
    bool enabled;
    
    TextureUnitSettings()
    {
        texId = 0;
        enabled = false;
    }
};

class Texture {
    static std::stack<TextureSettings> settings;
    static GLuint activeUnit;
    static std::vector<TextureUnitSettings> unitSettings;
    
    static Buffer<int32_t>* applyMask(Buffer<int32_t>* bmp)
    {
        int w = bmp->getWidth(), h = bmp->getHeight();
        Buffer<int32_t>* masked = new Buffer<int32_t>(w, h);
        
        for(int j = 0; j < h; j++)
            for(int i = 0; i < w; i++)
            {
                Col32 c = bmp->at(i, j);
                c.seta(255);
                
                if(c.rgbCmp(Texture::maskColor))
                    masked->at(i, j) = 0;
                else
                    masked->at(i, j) = c.value;
            }
        
        return masked;
    }
    
    
    
public:
    static const unsigned int LINEAR = 1, ALPHA = 2, MIPMAP = 4, ANISO = 8, WRAP = 16, MASKED = 32;
    static unsigned int flags;
    static Col32 maskColor;
    
    int w, h;
    GLuint id;
    
    void init(Buffer<int32_t>* bmp)
    {
        Buffer<int32_t>* temp = (Texture::flags & MASKED) ? applyMask(bmp) : bmp;
        this->w = bmp->getWidth();
        this->h = bmp->getHeight();
        GLint destFmt = Texture::flags & (ALPHA | MASKED) ? GL_RGBA : GL_RGB;
        
        this->id = Texture::create(bmp->getRawData(), w, h, GL_BGRA, destFmt);
        
        if(temp != bmp) // Delete the temporary masked bitmap.
            delete temp;
    }
    
    static void pushSettings()
    {
        settings.push(TextureSettings(flags, maskColor));
    }
    
    static void popSettings()
    {
        if(settings.empty())
            return;
        
        TextureSettings t = settings.top();
        flags = t.flags;
        maskColor = t.maskColor;
        settings.pop();
    }
    
    static void setActiveUnit(int unit)
    {
        //if(Texture::activeTexUnit != unit)
        //{
            glActiveTextureARB(GL_TEXTURE0_ARB + unit);
            //Texture::activeTexUnit = unit;
        //}
    }
    
    static void setEnabled(bool enabled, int unit=Texture::activeUnit)
    {
        setActiveUnit(unit);
        
        /*
        if(enabled && !texUnitSettings.at(unit).enabled)
            glEnable(GL_TEXTURE_2D);
        else if(!enabled && texUnitSettings.at(unit).enabled)
            glDisable(GL_TEXTURE_2D);*/
        
        //texUnitSettings.at(unit).enabled = enabled;
        
        if(enabled)
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);
    }
    
    static void bindId(GLuint id, int unit=Texture::activeUnit)
    {
        setActiveUnit(unit);
        
        //if(id != Texture::texUnitSettings.at(unit).texId)
        //{
            glBindTexture(GL_TEXTURE_2D, id);
            //Texture::texUnitSettings.at(unit).texId = id;
        //}
    }
    
    void bind(int unit=Texture::activeUnit) const
    {
        setActiveUnit(unit);
        
        //if(id != Texture::texUnitSettings.at(unit).texId)
        //{
            glBindTexture(GL_TEXTURE_2D, id);
            //Texture::texUnitSettings.at(unit).texId = id;
        //}
    }
    
    GLuint static create(void const* data, int w, int h, GLint srcFmt, GLint destFmt)
    {
        GLuint id;
        
        glGenTextures(1, &id);
        Texture::bindId(id);
        
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        if(Texture::flags & WRAP)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        else
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        }
        
        if(Texture::flags & LINEAR)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            (Texture::flags & MIPMAP) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        
        else
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            (Texture::flags & MIPMAP) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR);
            
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        
        if(Texture::flags & ANISO)
        {
            GLfloat level;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &level);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
        }
        
        else
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
        }
        
        if(Texture::flags & MIPMAP)
            gluBuild2DMipmaps(GL_TEXTURE_2D, destFmt, w, h, srcFmt, GL_UNSIGNED_BYTE, data);
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, destFmt, w, h, 0, srcFmt, GL_UNSIGNED_BYTE, data);
        }
        
        return id;
    }
    
    Texture(GLuint id, int w, int h)
    {
        this->id = id;
        this->w = w;
        this->h = h;
    }
    
    Texture()
    {
        int w = 16, h = 16;
        Buffer<int32_t> bmp(w, h);
        bmp.fill(Col32(0, 255, 0, 255).value);
        
        this->id = Texture::create(bmp.getRawData(), w, h, GL_RGBA, GL_RGB);
        this->w = w;
        this->h = h;
    }
    
    Texture(Buffer<int32_t>* bmp)
    {
        init(bmp);
    }
    
    Texture(char const* path)
    {
        Buffer<int32_t>* bmp = ImageLoader::load(path);
        
        if(bmp)
        {
            init(bmp);
            delete bmp;
        }
            
        else
        {
            std::string error = "Could not load file: '";
            error += path;
            error += "'";
            
            throw std::runtime_error(error);
        }
    }
    
    Vec2f getTexCoords(int x, int y) const
    {
        Vec2f v(float(x) / float(w), float(y) / float(h));
        return v;
    }

    int getHeight() const
    {
        return h;
    }

    int getWidth() const
    {
        return w;
    }

    GLuint getId() const
    {
        return id;
    }
};

unsigned int Texture::flags = Texture::LINEAR;
Col32 Texture::maskColor;
std::stack<TextureSettings> Texture::settings;
GLuint Texture::activeUnit = 0;
std::vector<TextureUnitSettings> Texture::unitSettings(GL_MAX_TEXTURE_UNITS_ARB);

#endif	/* TEXTURE_HPP */

