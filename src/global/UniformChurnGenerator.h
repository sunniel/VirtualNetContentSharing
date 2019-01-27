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

#ifndef __MULTINODEFASTSMRSIMULATOIN_UNIFORMCHURNGENERATOR_H_
#define __MULTINODEFASTSMRSIMULATOIN_UNIFORMCHURNGENERATOR_H_

#include <omnetpp.h>
#include "GlobalNodeListAccess.h"
#include "UnderlayConfiguratorAccess.h"
#include "GlobalStatisticsAccess.h"

using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class UniformChurnGenerator: public cSimpleModule {
private:
    double churnChordCycle;
    cMessage* churnChord;
    double departChordRate;
    double arrivalChordRate;
    double churnCanCycle;
    cMessage* churnCan;
    double departCanRate;
    double arrivalCanRate;
    // max and min overlay network size
    int max;
    int min;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
public:
    UniformChurnGenerator();
    virtual ~UniformChurnGenerator();
};

#endif
