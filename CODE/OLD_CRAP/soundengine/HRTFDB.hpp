#ifndef _HRTFDB_hpp_
#define _HRTFDB_hpp_

#include <stdio.h>
#include <allegro.h>

#include "HRTF.hpp"
#include <vector>

#include "MissingFileException.hpp"

namespace Audio {

class HRTFDB {
    typedef std::vector<HRTF> Ring;
    std::vector<Ring> mSphere;
    HRTF mInterp, mInterp2, mInterp3;
    int mFirLen, mLogicLen, mRealLen;

    static int wrap(int i);
    void load(int id, int buffLen);

    void _geti(HRTF& hrtf, double azim, double elev);

public:

    HRTFDB(int id, int buffLen);

    HRTF const& get(double azim, double elev) const;
    HRTF const& geti(double azim, double elev);
    HRTF const& getex(double azim1, double azim2, double elev);

    void ild(double azim, double elev, double& left, double& right);
    int realLen() const;
    int logicLen() const;
    int firLen() const;
};

}

#endif
