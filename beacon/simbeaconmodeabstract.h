#ifndef SIMBEACONMODEABSTRACT_H
#define SIMBEACONMODEABSTRACT_H

namespace sim
{
    namespace beacon
    {

class mode_abstract
{
public:
    mode_abstract() {}
    virtual ~mode_abstract() {}

    virtual bool run() = 0;
};


    } // namespace beacon
} // namespace sim

#endif // SIMBEACONMODEABSTRACT_H
