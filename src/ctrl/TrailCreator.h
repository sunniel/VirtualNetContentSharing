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

#ifndef CTRL_TRAILCREATOR_H_
#define CTRL_TRAILCREATOR_H_

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

class TrailCreator: public HostBase {
private:
    unsigned long id;
    Coordinate location;
    Coordinate origin;
    // perception range
    long radius;

    // file pointer
    ofstream outfile;

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

    simtime_t walkCycle;
    simtime_t startCycle;
    simtime_t timeoutLength;

    cMessage* walkTimer;
    cMessage* startTimer;

    void move(cMessage* msg);
    void changePosition();
    void displayPosition();
    void startPlay(cMessage* msg);
protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void dispatchHandler(cMessage *msg);
    virtual void finish();
public:
    TrailCreator();
    virtual ~TrailCreator();
};

#endif /* CTRL_TRAILCREATOR_H_ */
