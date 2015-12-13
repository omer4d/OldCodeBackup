/* 
 * File:   SourceUpdateEvent.hpp
 * Author: Stas
 *
 * Created on February 13, 2012, 8:25 PM
 */

#ifndef SOURCEUPDATEEVENT_HPP
#define	SOURCEUPDATEEVENT_HPP

#include "Item.hpp"

namespace Audio {

    class SourceEvent {
    public:
        enum Type {
            UPDATE, FULL_CYCLE, STOP
        };

    private:
        Type mType;
        Item& mItem;
        
    public:
        bool stop;

        SourceEvent(Item& item, Type type): mItem(item)
        {
            mType = type;
            stop = false;
        }

        Signal::Handle const& signal() const
        {
            return mItem.signal;
        }
        
        Type type() const
        {
            return mType;
        }

        int id() const
        {
            return mItem.id;
        }

        double curr() const
        {
            return mItem.curr;
        }

        Vec3f& pos()
        {
            return mItem.pos;
        }

        double& intens()
        {
            return mItem.intens;
        }
    };
}


#endif	/* SOURCEUPDATEEVENT_HPP */

