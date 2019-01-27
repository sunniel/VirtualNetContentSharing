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

#ifndef __CHORD_H_
#define __CHORD_H_

#include <climits>
#include <algorithm>
#include <omnetpp.h>
#include <nlohmann/json.hpp>
#include "../common/HostBase.h"
#include "../common/Util.h"
#include "../common/Constants.h"
#include "../global/GlobalStatisticsAccess.h"
#include "../global/GlobalNodeListAccess.h"
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
class ChordCtrl: public HostBase {
    struct Comparator {
        Comparator(ChordCtrl& c) :
                ctrl(c) {
        }
        unsigned long distance(unsigned long a, unsigned long b) {
            if (b >= a)
                return b - a;
            return b + pow(2, ctrl.M) - a;
        }
        bool operator ()(unsigned long a, unsigned long b) {
            unsigned long dist1 = distance(ctrl.getId(), a);
            unsigned long dist2 = distance(ctrl.getId(), b);
            return dist1 < dist2;
        }
        ChordCtrl& ctrl;
    };
private:
    int succSize;
    int fingerToFix;
    int objectSize;

    void onRoute(ChordMessage* msg);
    void onFinal(ChordMessage* msg);
    void onSuccessorFound(ChordMessage* msg);
    void onQuerySuccessor(ChordMessage* msg);
    void onNotify(ChordMessage* msg);
    void onStore(ChordMessage* msg);
    void onGet(ChordMessage* msg);
    void onReplicate(ChordMessage* msg);
    void onFixReplicas(ChordMessage* msg);
    void findSuccessor(unsigned long nodeToAsk, unsigned long id, string label);
    // return the node first preceding the given ID
    unsigned long closestPrecedingNode(unsigned long id);
    unsigned long distance(unsigned long a, unsigned long b);
    bool compare(unsigned long a, unsigned long b);
    vector<unsigned long> getFullTable();
    void notify(unsigned long nodeId);
    void stabilize();
    void fixFingers();
    void fixReplicas();
    bool inAB(unsigned long bid, unsigned long ba, unsigned long bb);
    json profile();
    // failures: next hop not found, data not found, node failed, max hop reached
    void handleFailure(ChordMessage* msg);
public:
    // chord IDs for overlay routing
    unsigned long chordId;
    unsigned long predecessor;
    vector<unsigned long> fingerTable;
    vector<unsigned long> successorList;
    // address space size
    int M;
    int MAXid;
    // content storage
    map<unsigned long, string> data;

    ChordCtrl();
    virtual ~ChordCtrl();
    bool operator<(const ChordCtrl& ctrl) const;
    void join(unsigned long bootstrap);
    void startMaint();
protected:
    // timer messages
    cMessage* maintainer;
    simtime_t maintain_cycle;

    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void final();
    virtual void dispatchHandler(cMessage *msg);
    virtual void r_transmit(ChordMessage* msg, unsigned long destID);
    virtual void maintain(cMessage *msg);
};

#endif
