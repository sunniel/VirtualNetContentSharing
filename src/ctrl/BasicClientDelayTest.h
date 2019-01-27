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

#ifndef CTRL_BASICCLIENTDELAYTEST_H_
#define CTRL_BASICCLIENTDELAYTEST_H_

#include <functional>
#include <map>
#include <queue>
#include "../common/HostBase.h"
#include "../global/CoordinatorAccess.h"
#include "../messages/CANMessage_m.h"
#include "../messages/ChordMessage_m.h"

using namespace std;
using namespace omnetpp;
using namespace boost;
using namespace boost::tuples;

class BasicClientDelayTest: public HostBase {
private:
    unsigned long id;
    Coordinate location;
    Coordinate origin;
    // perception range
    int radius;
    long perception;

    /*
     * Random walk variables
     */
    // movement direction
    double alpha;
    long speed;
    int steps;
    // movement distance
    int moveDist;
    // oval-shape for search range
    cOvalFigure* circle;
    // oval-shape for perception range
    cOvalFigure* p_circle;

    /*
     * data structures for data retrieval
     */
    // number of inventories to combine
    int coverage;
    int inventories;
    // cross-region content inventory;
    map<Coordinate, string, CoordntCompare> crc;
    map<Coordinate, string, CoordntCompare> data;
    queue<string> toDownloads;

    // check whether a content download process has been triggered
    bool triggered;
    bool loaded;

    // performance measures
    int hops;
    simtime_t start;
    simtime_t end;

    simtime_t loadCycle;
    simtime_t walkCycle;
    simtime_t startCycle;
    cMessage* refreshTimer;
    cMessage* walkTimer;
    cMessage* startTimer;

    simsignal_t delaySignal;

    void loadContent(cMessage* msg);
    void move(cMessage* msg);
    void changePosition();
    void onInventory(CANMessage* msg);
    void onDataReply(ChordMessage* msg);
    void displayPosition();
    void startPlay(cMessage* msg);
protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void dispatchHandler(cMessage *msg);
    virtual void finish();
    virtual bool r_transmit(cMessage* msg, unsigned long destID);
public:
    BasicClientDelayTest();
    virtual ~BasicClientDelayTest();
};

#endif /* CTRL_BASICCLIENTDELAYTEST_H_ */
