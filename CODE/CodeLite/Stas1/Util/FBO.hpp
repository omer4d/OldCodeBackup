/* 
 * File:   FBO.hpp
 * Author: Stas
 *
 * Created on November 9, 2012, 12:13 PM
 */

#ifndef FBO_HPP
#define	FBO_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include "Texture.hpp"

class FBO {
    GLuint fboId;
    Texture colorBuffer;
    
    friend class SubFBO;
    
public:
    FBO(int w, int h): colorBuffer(Texture::create(NULL, w, h, GL_RGBA, GL_RGBA), w, h)
    {
        glGenFramebuffersEXT(1, &fboId);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, colorBuffer.getId(), 0);
        
        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
            throw std::runtime_error("Unable to create FBO!");
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
    
    void bind() const
    {
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, colorBuffer.getWidth(), colorBuffer.getHeight()); 
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
    }
    
    void unbind() const
    {
        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
    
    Texture const& getColorBuffer() const
    {
        return colorBuffer;
    }
};

class SubFBO {
    int x, y, w, h;
    FBO const& fbo;

public:

    SubFBO(FBO const& pFbo, int pX, int pY, int pW, int pH) : fbo(pFbo), x(pX), y(pY), w(pW), h(pH)
    {
    }
    
    void bind()
    {
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(x, y, w, h); 
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo.fboId);
    }
    
    void unbind()
    {
        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);   
    }
    
    Texture const& getColorBuffer() const
    {
        return fbo.colorBuffer;
    }
    
    void texCoordTransform()
    {
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        
        glTranslatef(float(x) / float(fbo.colorBuffer.getWidth()),
                     float(y) / float(fbo.colorBuffer.getWidth()),
                     0.0);
        
        glScalef(float(w) / float(fbo.colorBuffer.getWidth()),
                 float(h) / float(fbo.colorBuffer.getHeight()),
                 1.0);
    }
    
    void texCoordUntransform()
    {
        glPopMatrix();
    }
};

#endif	/* FBO_HPP */

