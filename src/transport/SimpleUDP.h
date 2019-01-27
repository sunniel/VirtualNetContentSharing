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

#ifndef __MULTINODEFASTSMRSIMULATION_SIMPLEUDP_H_
#define __MULTINODEFASTSMRSIMULATION_SIMPLEUDP_H_

#include <omnetpp.h>
#include "../objects/IPvXAddress.h"
#include "../global/GlobalNodeListAccess.h"
#include "../global/GlobalParametersAccess.h"
#include "../messages/UDPControlInfo_m.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/SimpleInfo.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class SimpleUDP: public cSimpleModule {
private:
    double Copt;
    double BW;
    double MTU;
    double mu;
    double sigma;
    double pktLossRate;
    double Dmin;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    /**
     * Handle and outgoing message
     */
    virtual void handleOutgoingMessage(cMessage* packet);
    /**
     * Handle an incoming message
     */
    virtual void handleIncomingMessage(cMessage* packet);
};

#endif
