//
// Copyright (C) 2007 Institut fuer Telematik, Universitaet Karlsruhe (TH)
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
 * @file GlobalParameters.h
 * @author Ingmar Baumgart
 */

#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__

#include <omnetpp.h>
#include "../objects/IPvXAddress.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include "../common/MiscWatch.h"

using namespace std;
using namespace omnetpp;
using namespace boost;
using namespace boost::tuples;

/**
 * Modul for storing global simulation parameters
 *
 * @author Ingmar Baumgart
 */
class GlobalParameters: public cSimpleModule {
public:
    double getDistance(IPvXAddress host1, IPvXAddress host2);
    void remoteHost(IPvXAddress host);
    simtime_t getFixLatency();
    int getLatencyGenerationMode();
    int getAddrSpaceSize();

    int getSuccListSize();
    int getChordInitSize();
    int getChordReplicaSize();

    int getCANAreaSize();
    int getCANInitSize();
    int getMaxHop();
    int getERSThreshold();
protected:
    virtual void initialize();
private:
    double maxDist;
    map<boost::tuple<IPvXAddress, IPvXAddress>, double> distances;
    // for message transmission with fixed latency
    simtime_t fixedLatency;
    int latencyMode;
    int spaceSize;

    /*
     * chord parameters
     */
    int chord_init_size;
    int succSize;
    int replicaSize;

    /*
     * can parameters
     */
    int can_init_size;
    int canSize;
    int maxHop;
    // expanding ring search threshold
    int ttl;
};

#endif
