//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INTERFACETABLE_H
#define __INTERFACETABLE_H

#include <vector>
#include <omnetpp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <sstream>
#include "InterfaceEntry.h"

using namespace omnetpp;

/**
 * Represents the interface table. This object has one instance per host
 * or router. It has methods to manage the interface table,
 * so one can access functionality similar to the "ifconfig" command.
 *
 * See the NED documentation for general overview.
 *
 * This is a simple module without gates, it requires function calls to it
 * (message handling does nothing). Methods are provided for reading and
 * updating the interface table.
 *
 * Interfaces are dynamically registered: at the start of the simulation,
 * every L2 module adds its own InterfaceEntry to the table; after that,
 * IPv4's IRoutingTable and IPv6's RoutingTable6 (an possibly, further
 * L3 protocols) add protocol-specific data on each InterfaceEntry
 * (see IPv4InterfaceData, IPv6InterfaceData, and InterfaceEntry::setIPv4Data(),
 * InterfaceEntry::setIPv6Data())
 *
 * Interfaces are represented by InterfaceEntry objects.
 *
 * When interfaces need to be reliably and efficiently identified from other
 * modules, interfaceIds should be used. They are better suited than pointers
 * because when an interface gets removed (see deleteInterface()), it is
 * often impossible/impractical to invalidate all pointers to it, and also
 * because pointers are not necessarily unique (a new InterfaceEntry may get
 * allocated exactly at the address of a previously deleted one).
 * Interface Ids are unique (Ids of removed interfaces are not issued again),
 * stale Ids can be detected, and they are also invariant to insertion/deletion.
 *
 * Clients can get notified about interface changes by subscribing to
 * the following notifications in NotificationBoard: NF_INTERFACE_CREATED,
 * NF_INTERFACE_DELETED, NF_INTERFACE_STATE_CHANGED, NF_INTERFACE_CONFIG_CHANGED.
 * State change gets fired for up/down events; all other changes fire as
 * config change.
 *
 * @see InterfaceEntry
 */
class InterfaceTable: public cSimpleModule {
protected:
    // primary storage for interfaces: vector indexed by id; may contain NULLs;
    // slots are never reused to ensure id uniqueness
    typedef std::vector<InterfaceEntry *> InterfaceVector;
    InterfaceVector idToInterface;

    // fields to support getNumInterfaces() and getInterface(pos)
    int tmpNumInterfaces; // caches number of non-NULL elements of idToInterface; -1 if invalid
    InterfaceEntry **tmpInterfaceList; // caches non-NULL elements of idToInterface; NULL if invalid

    // displays summary above the icon
    virtual void updateDisplayString();

    // internal
    virtual void invalidateTmpInterfaceList();

    virtual int numInitStages() const {
        return 1;
    }
    virtual void initialize(int stage);

    /**
     * Raises an error.
     */
    virtual void handleMessage(cMessage *);

public:
    InterfaceTable();
    virtual ~InterfaceTable();
    /**
     * Adds an interface. The second argument should be a module which belongs
     * to the physical interface (e.g. PPP or EtherMac) -- it will be used
     * to discover and fill in getNetworkLayerGateIndex(), getNodeOutputGateId(),
     * and getNodeInputGateId() in InterfaceEntry. It should be NULL if this is
     * a virtual interface (e.g. loopback).
     */
    virtual void addInterface(InterfaceEntry *entry);

    /**
     * Returns the number of interfaces.
     */
    virtual int getNumInterfaces();

    /**
     * Returns an interface given by its name. Returns NULL if not found.
     */
    virtual InterfaceEntry *getInterfaceByName(const char *name);
};

#endif

