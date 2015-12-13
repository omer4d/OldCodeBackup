#include "HRTFDB.hpp"

namespace Audio {

int HRTFDB::wrap(int i)
{
    if(i >= 0) return i % 360;
    else return 360 - 1 - (abs(i + 1) % 360);
}

void HRTFDB::load(int id, int buffLen)
{
    char buff[300];
    int counter = 0;

    for(int j = 0; j < mSphere.size(); j++)
        for(int i = 0; i < mSphere[j].size(); i++)
        {
            sprintf(buff, "data/IRC_%d_C_R0195_T%03d_P%03d 1.wav", id, i * 360 / mSphere[j].size(), wrap(-45 + j * 15));
            SAMPLE* samp = load_sample(buff);
            
            if(samp == NULL)
                throw MissingFileException(buff);

            mSphere[j][i].processFir(samp, buffLen);
            destroy_sample(samp);

            counter++;
            printf("%.1f \n", counter / 187.0 * 100.0);
        }

    printf("HRTFs ready.\n");
}

void HRTFDB::_geti(HRTF& hrtf, double azim, double elev)
{
    if(elev < -45.0) elev = -45.0;
    if(elev > 90.0) elev = 90.0;
    if(azim < 0.0) azim = 0.0;
    if(azim > 359.9) azim = 359.9;

    int j = int((elev + 45.0) / 15.0);

    double fi = azim / 360.0 * mSphere[j].size();
    int i = int(fi);
    double k = fi - i;

    hrtf.blend(mSphere[j][i], mSphere[j][(i + 1) % mSphere[j].size()], k);
}

HRTFDB::HRTFDB(int id, int buffLen)
{
    for(int j = 0; j < 7; j++)
    {
        mSphere.push_back(Ring());
        for(int i = 0; i < 24; i++)
            mSphere[j].push_back(HRTF());
    }

    mSphere.push_back(Ring());
    for(int i = 0; i < 12; i++)
        mSphere[7].push_back(HRTF());

    mSphere.push_back(Ring());
    for(int i = 0; i < 6; i++)
        mSphere[8].push_back(HRTF());

    mSphere.push_back(Ring());
    mSphere[9].push_back(HRTF());

    load(id, buffLen);
    mInterp.init(mSphere[0][0].firLen(), buffLen);
    mInterp2.init(mSphere[0][0].firLen(), buffLen);
    mInterp3.init(mSphere[0][0].firLen(), buffLen);

    mFirLen = mSphere[0][0].firLen();
    mLogicLen = mSphere[0][0].logicLen();
    mRealLen = mSphere[0][0].realLen();
}

HRTF const& HRTFDB::get(double azim, double elev) const
{
    if(elev < -45.0) elev = -45.0;
    if(elev > 90.0) elev = 90.0;
    if(azim < 0.0) azim = 0.0;
    if(azim > 359.9) azim = 359.9;

    int j = int((elev + 45.0) / 15.0);
    int i = int(azim / 360.0 * mSphere[j].size());

    return mSphere[j][i];
}

HRTF const& HRTFDB::geti(double azim, double elev)
{
    _geti(mInterp, azim, elev);
    return mInterp;
}

HRTF const& HRTFDB::getex(double azim1, double azim2, double elev)
{
    _geti(mInterp, azim1, elev);
    _geti(mInterp2, azim2, elev);
    mInterp3.crossfade(mInterp, mInterp2);
    return mInterp3;
}

void HRTFDB::ild(double azim, double elev, double& left, double& right)
{
    if(elev < -45.0) elev = -45.0;
    if(elev > 90.0) elev = 90.0;
    if(azim < 0.0) azim = 0.0;
    if(azim > 359.9) azim = 359.9;

    int j = int((elev + 45.0) / 15.0);

    double fi = azim / 360.0 * mSphere[j].size();
    int i = int(fi);
    double k = fi - i;

    HRTF::blendILD(mSphere[j][i], mSphere[j][(i + 1) % mSphere[j].size()], k, left, right);
}

int HRTFDB::realLen() const
{
    return mRealLen;
}

int HRTFDB::logicLen() const
{
    return mLogicLen;
}

int HRTFDB::firLen() const
{
    return mFirLen;
}

}