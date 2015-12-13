#ifndef _SourceUpdateListener_hpp_
#define _SourceUpdateListener_hpp_

#include "Item.hpp"
#include "SafePtr.hpp"
#include "Signal.hpp"
#include "boost/shared_ptr.hpp"
#include "Vec3f.hpp"

#include "SourceUpdateEvent.hpp"

namespace Audio {

    class SourceUpdateListener {
        SafePtr<SourceUpdateListener> mThisPtr;

    public:

        SourceUpdateListener() : mThisPtr(this)
        {
        }

        SourceUpdateListener(SourceUpdateListener const& other) : mThisPtr(this)
        {
        }

        ~SourceUpdateListener()
        {
            mThisPtr.nullify();
        }

        SafePtr<SourceUpdateListener> ptr()
        {
            return mThisPtr;
        }

        virtual void onStop(Signal::Handle const& h, int id)
        {

        }

        virtual bool onFullCycle(Signal::Handle const& h, int id)
        {
            return true;
        }

        virtual bool onUpdate(Signal::Handle const& h, int id, double const& curr, Vec3f& pos, double& intens)
        {
            return true;
        }
    };

}

#endif
