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

#ifndef _UNDERLAYCONFIGURATOR_H_
#define _UNDERLAYCONFIGURATOR_H_

#include <cstdint>
#include <omnetpp.h>
#include "../common/HostBase.h"
#include "../common/Util.h"
#include "GlobalNodeListAccess.h"
#include "GlobalParametersAccess.h"
#include "ContentDistributorAccess.h"
#include "../objects/IPAddress.h"
#include "../objects/IPvXAddress.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/SimpleInfo.h"
#include "../others/InterfaceTableAccess.h"

using namespace std;
using namespace omnetpp;

/**
 * Base class for configurators of different underlay models
 *
 * @author Stephan Krause, Bernhard Heep
 */
class UnderlayConfigurator: public cSimpleModule {
public:
    UnderlayConfigurator();
    virtual ~UnderlayConfigurator();
    void removeNode(IPvXAddress& nodeAddr);
    void addChord();
    void addCAN(bool init = false);
    void initCANAdded();
    uint32_t freeAddress();
protected:
    /**
     * OMNeT number of init stages
     */
    int numInitStages() const;
    /**
     * OMNeT init methods
     */
    virtual void initialize(int stage);
    /**
     * Node mobility simulation
     *
     * @param msg timer-message
     */
    virtual void handleMessage(cMessage* msg);

private:
    // chord node creation counter
    int chord_counter;
    // chord node creation counter
    int can_counter;

    uint32_t nextFreeAddress;
    GlobalNodeList* globalNodeList;
    GlobalParameters* parameterList;
    ContentDistributor* contentDist;

    // record the time of death of each node, in (node_address, node_death_time)
    map<string, simtime_t> death_schedule;

    void handleChordInit();
    void handleCANInit();
    void disposeFailures();
};

#endif
