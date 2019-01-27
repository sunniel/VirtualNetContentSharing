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

#ifndef CLIENTLOADTEST_H_
#define CLIENTLOADTEST_H_

#include <functional>
#include <map>
#include <fstream>
#include "../common/HostBase.h"
#include "../messages/CANMessage_m.h"
#include "../messages/ChordMessage_m.h"
#include "../objects/Coordinate.h"
#include "../objects/CANProfile.h"

using namespace std;
using namespace omnetpp;
using namespace boost;
using namespace boost::tuples;

class ClientLoadTest: public HostBase {
private:
    unsigned long id;
    Coordinate location;
    Coordinate origin;
    // perception range
    long radius;

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
    bool trailFromFile;
    ifstream fin;

    /*
     * data structures for data retrieval
     */
    // number of inventories to combine
    int coverage;
    int inventories;
    // cross-region content inventory;
    map<Coordinate, string, CoordntCompare> crc;
    map<Coordinate, string, CoordntCompare> mappings;
    map<Coordinate, string, CoordntCompare> data;
    map<Coordinate, string, CoordntCompare> history;
    set<CANProfile> toVisit;
    set<CANProfile> visiting;
    set<CANProfile> visited;

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

    void refreshContent(cMessage* msg);
    void clearCache();
    void loadContent();
    void move(cMessage* msg);
    void changePosition();
    void onInventory(CANMessage* msg);
    void onContent(CANMessage* msg);
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
    ClientLoadTest();
    virtual ~ClientLoadTest();
};

#endif /* CLIENT_H_ */
