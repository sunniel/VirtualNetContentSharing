//
// Copyright (C) 2006 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @file Chord.h
 * @author Markus Mauch, Ingmar Baumgart
 */

#ifndef __CAN_H_
#define __CAN_H_

#include <climits>
#include <algorithm>
#include <omnetpp.h>
#include <nlohmann/json.hpp>
#include "../common/HostBase.h"
#include "../objects/CANInfo.h"
#include "../objects/CANProfile.h"
#include "../messages/CANMessage_m.h"
#include "../messages/ChordMessage_m.h"

using namespace std;
using namespace omnetpp;
using json = nlohmann::json;

/**
 * Chord overlay module
 *
 * Implementation of the Chord KBR overlay as described in
 * "Chord: A Scalable Peer-to-Peer Lookup Protocol for Inetnet
 * Applications" by I. Stoica et al. published in Transactions on Networking.
 *
 * @author Markus Mauch, Ingmar Baumgart
 * @see BaseOverlay, ChordFingerTable, ChordSuccessorList
 */
class CANCtrl: public HostBase {
    vector<CANProfile> neighbors;
    map<CANProfile, vector<CANProfile>> neighborsOfNeighbors;

    /*
     * Take-over related data structures
     */
    vector<CANProfile> takeovers;
    map<CANProfile, vector<CANProfile>> neighborsOftakeovers;
    // neighbors of neighbors of take-overs
    map<CANProfile, map<CANProfile, vector<CANProfile>>> nnTakeovers;

    // set of failed neighbor zones
    set<CANProfile> failed;
    set<CANProfile> failedHosts;

    // content storage
    map<position, string, compare> inventories;
    map<position, string, compare> data;

    /*
     * Expanding ring search data structures
     */
    // maximal time-to-live of ERS, at minimum of 1
    map<CANProfile, int> TTLs;
private:
    void onRoute(CANMessage* msg);
    void onJoinReply(CANMessage* msg);
    void onJoinReplyFail(CANMessage* msg);
    void onNeighborAdd(CANMessage* msg);
    void onNeighborRemove(CANMessage* msg);
    void onFinal(CANMessage* msg);
    // handle the neighbor area update event
    void onAreaUpdate(CANMessage* msg);
    // handle the neighbor ID update event
    void onNeighborUpdate(CANMessage* msg);
    void onNeighborTakeover(CANMessage* msg);
    void onNeighborExchange(CANMessage* msg);
    void onNeighborRequest(CANMessage* msg);
    void onNeighborReply(CANMessage* msg);
    void onReplicate(CANMessage* msg);
    void onFixReplicas(CANMessage* msg);
    void onFixLoad(CANMessage* msg);
    void onMappingReply(ChordMessage* msg);
    void onMappingQuery(CANMessage* msg);
    void exchangeNeighbors();
    void fixNeighbors();
    void fixReplicas();
    void fixLoad();
    void expandRingSearch();
    void updateMapping();
    void lookup(unsigned long nodeToAsk, string content, int type,
            string label);
    bool replyInventory(CANMessage* msg, position p);
    bool replyContent(CANMessage* msg, CANProfile info,
            vector<CANProfile> neighbors);
    // failures: next hop not found, data not found, node failed, max hop reached
    void handleFailure(CANMessage* msg, CANProfile info);
    /**
     * Gives the next peer for the routing of lookup and store messages.
     */
    CANProfile routeToNext(position id, CANProfile info,
            vector<CANProfile> neighbors);
public:
    CANProfile info;

    /*
     * for result recording
     */
    simtime_t birth;
    int load;

    CANCtrl();
    virtual ~CANCtrl();
    bool operator<(const CANCtrl& ctrl) const;
    void join(unsigned long bootstrap, string label = "");
    void initArea();
    void startMaint();
protected:
    // timer messages
    cMessage* maintainer;
    simtime_t maintain_cycle;

    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void final();
    virtual void dispatchHandler(cMessage *msg);
    virtual void r_transmit(CANMessage* msg, unsigned long destID);
    virtual void maintain(cMessage *msg);
};

#endif
