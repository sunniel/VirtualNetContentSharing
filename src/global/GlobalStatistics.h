//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __MULTINODEFASTSMRSIMULATOIN_GLOBALSTATISTICS_H_
#define __MULTINODEFASTSMRSIMULATOIN_GLOBALSTATISTICS_H_

#include <omnetpp.h>
#include <climits>
#include <cfloat>
#include "GlobalNodeListAccess.h"

using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class GlobalStatistics: public cSimpleModule {
private:
    vector<int> hops;
    vector<int> contentLoadHops;
    vector<simtime_t> contentLoadDelays;
    vector<double> avgLoads;

    cMessage* sampler;
    simtime_t sample_cycle;

    simsignal_t loadSignal;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void sample(cMessage *msg);
public:
    int FAILS = 0;
    int SUCCESS = 0;
    int JOIN_FAILS = 0;
    int CHORD_ARRIVAL = 0;
    int CHORD_DEPARTURE = 0;
    int CAN_ARRIVAL = 0;
    int CAN_DEPARTURE = 0;
    GlobalStatistics();
    ~GlobalStatistics();
    void addHop(int hop);
    void addContentLoadHop(int hop);
    void addContentLoadDelay(simtime_t delay);
};

#endif
