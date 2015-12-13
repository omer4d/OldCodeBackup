#include "EngineManager.hpp"

namespace Audio {

    EngineManager::EngineManager(int pBuffLen) : mLogger("EngineManager")
    {
        mHrtfdb = new HRTFDB(1059, pBuffLen);
        mBuffLen = pBuffLen;
        mEngineNum = omp_get_num_procs();
        mEngineArr = new Engine*[mEngineNum];

        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i] = new Engine(*mHrtfdb, pBuffLen);

        mStream = play_audio_stream(mBuffLen, 16, 1, 44100, 255, 128);
        mLogger.log("%d Engines initiated!\n", mEngineNum);
    }

    EngineManager::~EngineManager()
    {
        delete mHrtfdb;
        stop_audio_stream(mStream);
        for(int i = 0; i < mEngineNum; i++)
            delete mEngineArr[i];
        delete[] mEngineArr;
    }

    void EngineManager::setDefaultEnv(Env::Handle const& env)
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->setDefaultEnv(env);
    }

    void EngineManager::resetSpatializer(int id)
    {
        delete mHrtfdb;
        mHrtfdb = new HRTFDB(id, mBuffLen);

        for(int i = 0; i < mEngineNum; i++)
        {
            mEngineArr[i]->resetSpatializer(*mHrtfdb);
        }
    }

    Item::Handle EngineManager::play(Env::Handle const& env, Signal::Handle const& signal, int id,
                                     Quality quality, double intens, bool looped, Vec3f const& pos)
    {
        int min = mEngineArr[0]->workload(), mini = 0;

        for(int i = 1; i < mEngineNum; i++)
            if(mEngineArr[i]->workload() < min)
            {
                min = mEngineArr[i]->workload();
                mini = i;
            }

        return mEngineArr[mini]->play(env, signal, id, quality, intens, looped, pos);
    }

    Item::Handle EngineManager::play(Signal::Handle const& signal, int id, Quality quality, double intens,
                                     bool looped, Vec3f const& pos)
    {
        int min = mEngineArr[0]->workload(), mini = 0;

        for(int i = 1; i < mEngineNum; i++)
            if(mEngineArr[i]->workload() < min)
            {
                min = mEngineArr[i]->workload();
                mini = i;
            }

        return mEngineArr[mini]->play(signal, id, quality, intens, looped, pos);
    }

    void EngineManager::stop(Signal::Handle const& signal)
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->stop(signal);
    }

    void EngineManager::stop(Signal::Handle const& signal, int id)
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->stop(signal, id);
    }

    void EngineManager::stopAll()
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->stopAll();
    }

    void EngineManager::stopAll(int id)
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->stopAll(id);
    }
    
    void EngineManager::addEventListener(ListenerProxy p, int id)
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->addEventListener(p, id);
    }
    
    void EngineManager::removeEventListener(ListenerProxy p, int id)
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->removeEventListener(p, id);
    }

    void EngineManager::setHeadState(Vec3f const& pos, double const& yaw, double const& pitch)
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->setHeadState(pos, yaw, pitch);
    }

    Vec3f EngineManager::getHeadPos() const
    {
        return mEngineArr[0]->getHeadPos();
    }

    double EngineManager::getHeadYaw() const
    {
        return mEngineArr[0]->getHeadYaw();
    }

    double EngineManager::getHeadPitch() const
    {
        return mEngineArr[0]->getHeadPitch();
    }

    double EngineManager::time() const
    {
        return mEngineArr[0]->time();
    }

    bool EngineManager::logic()
    {
        uint16* buff = (uint16*) get_audio_stream_buffer(mStream);
        int i;

        if(!buff) return false;

#pragma omp parallel for
        for(i = 0; i < mEngineNum; i++)
            mEngineArr[i]->logic();

        for(i = 0; i < mBuffLen * 2; i++)
            buff[i] = 32767;

        for(i = 0; i < mEngineNum; i++)
        {
            int16 const* data = mEngineArr[i]->getBuffer();
            for(int j = 0; j < mBuffLen * 2; j++)
                buff[j] += data[j];
        }

        free_audio_stream_buffer(mStream);
        return true;
    }

    void EngineManager::visualize(BITMAP* buffer) const
    {
        for(int i = 0; i < mEngineNum; i++)
            mEngineArr[i]->visualize(buffer);
    }

}