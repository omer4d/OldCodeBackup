#include "Engine.hpp"

namespace Audio {

    void Engine::spatialized(Item* event)
    {
        Signal::Handle& sig = event->signal;

        SourceState currState = event->state(mHeadState);
        SourceState& lastState = event->lastState;
        SourceState step = SourceState::calcStep(lastState, currState, mBuffLen);

        double* leftSigTime = mSpatializer->left();
        double* rightSigTime = mSpatializer->right();
        double lastYaw = lastState.relYaw;

        mWorkload += 20;

        if(event->looped)
            for(int i = 0; i < mBuffLen; i++)
            {
                leftSigTime[i] = (sig.loopedSample(event->curr - lastState.leftShift) - 32767) * lastState.intens;
                rightSigTime[i] = (sig.loopedSample(event->curr - lastState.rightShift) - 32767) * lastState.intens;
                lastState.takeStep(step);
                event->curr += event->step;
            }
        else
            for(int i = 0; i < mBuffLen; i++)
            {
                leftSigTime[i] = (sig.sample(event->curr - lastState.leftShift) - 32767) * lastState.intens;
                rightSigTime[i] = (sig.sample(event->curr - lastState.rightShift) - 32767) * lastState.intens;
                lastState.takeStep(step);
                event->curr += event->step;
            }

        lastState.relYaw = currState.relYaw;
        mSpatializer->render(mBuff, currState.relYaw / M_PI * 180.0, 0.0);
    }

    void Engine::cheap(Item* event)
    {
        Signal::Handle& sig = event->signal;

        SourceState currState = event->state(mHeadState);
        SourceState& lastState = event->lastState;
        SourceState step = SourceState::calcStep(lastState, currState, mBuffLen);
        double a = 0.3 + ((sin(currState.relYaw) + 1.0) * 0.5) * 0.7, b = 0.3 + (1.0 - a) * 0.7;

        mWorkload += 2;

        // TODO: Enable blending of a and b to avoid clicking!
        if(event->looped)
            for(int i = 0; i < mBuffLen; i++)
            {
                mBuff[i * 2 + 1] += int16((sig.loopedSample(event->curr - lastState.leftShift) - 32767) * b * lastState.intens);
                mBuff[i * 2] += int16((sig.loopedSample(event->curr - lastState.rightShift) - 32767) * a * lastState.intens);

                lastState.takeStep(step);
                event->curr += event->step;
            }

        else
            for(int i = 0; i < mBuffLen; i++)
            {
                mBuff[i * 2 + 1] += int16((sig.sample(event->curr - lastState.leftShift) - 32767) * b * lastState.intens);
                mBuff[i * 2] += int16((sig.sample(event->curr - lastState.rightShift) - 32767) * a * lastState.intens);

                lastState.takeStep(step);
                event->curr += event->step;
            }
    }

    void Engine::ambient(Item* event)
    {
        Signal::Handle& sig = event->signal;
        Signal::Handle::getSampi getSampiLeft = NULL;
        Signal::Handle::getSampi getSampiRight = NULL;
        
        if(event->looped)
        {
            getSampiLeft = &Signal::Handle::loopedSampleLeft;
            getSampiRight = &Signal::Handle::loopedSampleRight;
        }
        
        else
        {
            getSampiLeft = &Signal::Handle::sampleLeft;
            getSampiRight = &Signal::Handle::sampleRight;
        }
            
        SourceState currState = event->state(mHeadState);
        SourceState& lastState = event->lastState;
        SourceState step = SourceState::calcStep(lastState, currState, mBuffLen);

        mWorkload += 1;

        if(event->signal.stereo())
            for(int i = 0; i < mBuffLen; i++)
            {
                int s1 = CALL_METHOD(sig, getSampiLeft)(int(event->curr)) - 32767;
                int s2 = CALL_METHOD(sig, getSampiRight)(int(event->curr)) - 32767;

                mBuff[i * 2 + 1] += int16(s1 * lastState.intens);
                mBuff[i * 2] += int16(s2 * lastState.intens);

                lastState.takeStep(step);
                event->curr += event->step;
            }
        else
            for(int i = 0; i < mBuffLen; i++)
            {
                int s = CALL_METHOD(sig, getSampiLeft)(int(event->curr)) - 32767;
                
                mBuff[i * 2 + 1] += int16(s * lastState.intens);
                mBuff[i * 2] += int16(s * lastState.intens);

                lastState.takeStep(step);
                event->curr += event->step;
            }
    }

    Engine::Engine(HRTFDB& hrtfdb, int pBuffLen): mDefaultEnv(new Env())
    {
        mBuff = new int16[pBuffLen * 2];
        mBuffLen = pBuffLen;
        mOutFreq = 44100;
        mCycleNum = 0;

        mHeadState.yaw = -M_PI * 0.5;
        mSpatializer = new Spatializer(hrtfdb, pBuffLen);
        mWorkload = 0;
    }

    Engine::~Engine()
    {
        for(ItemList::iterator iter = mPlayList.begin(); iter != mPlayList.end(); ++iter)
            delete *iter;
        delete mSpatializer;
        delete[] mBuff;
    }
    
    void Engine::setDefaultEnv(Env::Handle const& env)
    {
        this->mDefaultEnv = env;
    }

    int16 const* Engine::getBuffer() const
    {
        return mBuff;
    }

    void Engine::resetSpatializer(HRTFDB& hrtfdb)
    {
        delete mSpatializer;
        mSpatializer = new Spatializer(hrtfdb, mBuffLen);
    }

    Item::Handle Engine::play(Env::Handle const& env, Signal::Handle const& signal, int id, Quality quality,
                      double intens, bool looped, Vec3f const& pos)
    {
        Item* e = new Item(double(signal.freq()) / double(mOutFreq), env, signal, id,
                             quality, intens, looped, pos, mHeadState);
        
        mPlayList.push_back(e);

        if(e->quality == FULL_QUALITY) mWorkload += 10;
        else mWorkload++;
        
        return e->handle();
    }

    Item::Handle Engine::play(Signal::Handle const& signal, int id, Quality quality, double intens,
                      bool looped, Vec3f const& pos)
    {
        return play(mDefaultEnv, signal, id, quality, intens, looped, pos);
    }


    void Engine::stop(Signal::Handle const& signal)
    {
        for(ItemList::iterator iter = mPlayList.begin(); iter != mPlayList.end();)
            if((*iter)->signal == signal)
            {
                delete *iter;
                iter = mPlayList.erase(iter);
            }
            else
                ++iter;
    }

    void Engine::stop(Signal::Handle const& signal, int id)
    {
        for(ItemList::iterator iter = mPlayList.begin(); iter != mPlayList.end();)
            if((*iter)->signal == signal && (*iter)->id == id)
            {
                delete *iter;
                iter = mPlayList.erase(iter);
            }

            else
                ++iter;
    }

    void Engine::stopAll()
    {
        for(ItemList::iterator iter = mPlayList.begin(); iter != mPlayList.end(); ++iter)
            delete *iter;
        mPlayList.clear();
    }

    void Engine::stopAll(int id)
    {
        for(ItemList::iterator iter = mPlayList.begin(); iter != mPlayList.end();)
            if((*iter)->id == id)
            {
                delete *iter;
                iter = mPlayList.erase(iter);
            }

            else
                ++iter;
    }
    
    void Engine::addEventListener(ListenerProxy p, int id)
    {
        for(ItemList::iterator iter = mPlayList.begin(); iter != mPlayList.end(); ++iter)
            if((*iter)->id == id)
                (*iter)->dispatcher.addListener(p);
    }
    
    void Engine::removeEventListener(ListenerProxy p, int id)
    {
        for(ItemList::iterator iter = mPlayList.begin(); iter != mPlayList.end(); ++iter)
            if((*iter)->id == id)
                (*iter)->dispatcher.removeListener(p);
    }

    void Engine::setHeadState(Vec3f const& pos, double const& yaw, double const& pitch)
    {
        mHeadState.pos = pos;
        mHeadState.yaw = yaw;
        mHeadState.pitch = pitch;
    }

    Vec3f Engine::getHeadPos() const
    {
        return mHeadState.pos;
    }

    double Engine::getHeadYaw() const
    {
        return mHeadState.yaw;
    }

    double Engine::getHeadPitch() const
    {
        return mHeadState.pitch;
    }

    void Engine::logic()
    {
        mSpatializer->renderTail(mBuff);
        mWorkload = 0;

        ItemList::iterator iter = mPlayList.begin();

        while(iter != mPlayList.end())
        {
            Item* item = *iter;
            SourceEvent event(*item, SourceEvent::UPDATE);
            item->dispatcher.dispatch(event);
            bool stop = event.stop;
            
            switch(item->quality)
            {
                case FULL_QUALITY:
                    spatialized(item);
                    break;
                case CHEAP_QUALITY:
                    cheap(item);
                    break;
                case AMBIENT_QUALITY:
                    ambient(item);
                    break;
            }

            if(item->curr >= item->signal.len())
                if(item->looped)
                {
                    item->curr -= item->signal.len();
                    SourceEvent event(*item, SourceEvent::FULL_CYCLE);
                    item->dispatcher.dispatch(event);
                    
                    stop = stop || event.stop;
                }
                else
                    stop = true;

            if(stop)
            {
                SourceEvent event(*item, SourceEvent::STOP);
                item->dispatcher.dispatch(event);
                delete item;
                iter = mPlayList.erase(iter);
            }

            else
                ++iter;

        }

        mCycleNum++;
    }

    double Engine::time() const
    {
        return mCycleNum * mBuffLen / double(mOutFreq);
    }

    long Engine::workload() const
    {
        return mWorkload;
    }

    void Engine::visualize(BITMAP* buffer) const
    {
        HeadState const& head = mHeadState;
        double r = 6.0, v = 340.0;

        Vec2f dir(cos(head.yaw), sin(head.yaw)), norm = dir.normal();
        Vec3f left(head.pos.x - r * norm.x, head.pos.y, head.pos.z - r * norm.y);
        Vec3f right(head.pos.x + r * norm.x, head.pos.y, head.pos.z + r * norm.y);

        circlefill(buffer, SCREEN_W / 2 + int(left.x), SCREEN_H / 2 + int(left.z), 3, makecol(0, 255, 0));
        circlefill(buffer, SCREEN_W / 2 + int(right.x), SCREEN_H / 2 + int(right.z), 3, makecol(0, 0, 255));
        circlefill(buffer, SCREEN_W / 2 + int(head.pos.x), SCREEN_H / 2 + int(head.pos.z), 6, makecol(255, 0, 0));
        line(buffer, SCREEN_W / 2 + int(head.pos.x), SCREEN_H / 2 + int(head.pos.z),
                SCREEN_W / 2 + int(head.pos.x + dir.x * 10.0), SCREEN_H / 2 + int(head.pos.z + dir.y * 10.0), 0);

        for(ItemList::const_iterator iter = mPlayList.begin(); iter != mPlayList.end(); ++iter)
            circlefill(buffer, SCREEN_W / 2 + int((*iter)->pos.x), SCREEN_H / 2 + int((*iter)->pos.z), 3, makecol(255, 0, 0));
    }

}