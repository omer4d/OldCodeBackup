#ifndef _Item_hpp_
#define _Item_hpp_

#include <list>

#include "Vec3f.hpp"
#include "Vec2f.hpp"

#include "Signal.hpp"
#include "Env.hpp"

#include "SourceState.hpp"
#include "HeadState.hpp"
#include "Event.hpp"
#include "Proxy.hpp"

#include <boost/shared_ptr.hpp>

namespace Audio {
    class SourceEvent;
    typedef Proxy<EventListener<SourceEvent> > ListenerProxy;

    enum Quality {
        AMBIENT_QUALITY, CHEAP_QUALITY, FULL_QUALITY
    };

    struct Item {
        boost::shared_ptr<bool> alive;
        int id;
        Signal::Handle signal;
        Quality quality;
        bool looped;
        double curr, step;

        Vec3f pos;
        double intens;
        Env::Handle env;
        SourceState lastState;

        EventDispatcher<SourceEvent> dispatcher;
       
        Item(double pStep, Env::Handle const& pEnv, Signal::Handle const& pSignal, int pId,
             Quality pQuality, double pIntens, bool pLooped, Vec3f const& pPos, HeadState const& head);
        ~Item();

        SourceState state(HeadState const& head) const;

        // **********
        // * Handle *
        // **********
        
        class Handle : public Proxy<Item> {
            Item* getObject()
            {
                return object;
            }

        public:
            Handle(Item* object, boost::shared_ptr<bool> isAlive): Proxy<Item>(object, isAlive)
            {
            }
            
            void addEventListener(ListenerProxy const& p)
            {
                getObject()->dispatcher.addListener(p);
            }

            void removeEventListener(ListenerProxy const& p)
            {
                getObject()->dispatcher.removeListener(p);
            }
        };
        
        Handle handle()
        {
            return Handle(this, alive);
        }
    };

}

#endif
