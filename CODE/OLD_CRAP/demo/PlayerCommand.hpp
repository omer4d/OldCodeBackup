/* 
 * File:   PlayerCommand.hpp
 * Author: Stas
 *
 * Created on February 8, 2012, 5:40 PM
 */

#ifndef PLAYERCOMMAND_HPP
#define	PLAYERCOMMAND_HPP

#include "Vec3f.hpp"

class PlayerCommand {
    bool distant;
    Vec3f targetPos;
    
protected:

    void setTargetPos(Vec3f targetPos)
    {
        this->targetPos = targetPos;
    }

    void setDistant(bool distant)
    {
        this->distant = distant;
    }
    
public:
    virtual bool validate() = 0;
    virtual void execute() = 0;
    
    Vec3f getTargetPos() const
    {
        return targetPos;
    }

    bool isDistant() const
    {
        return distant;
    }
};

#endif	/* PLAYERCOMMAND_HPP */

