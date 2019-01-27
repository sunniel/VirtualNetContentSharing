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
 * @file GlobalNodeList.cc
 * @author Markus Mauch, Robert Palmer, Ingmar Baumgart
 */

#include <iostream>
#include "GlobalNodeList.h"
#include "../common/MiscWatch.h"

Define_Module(GlobalNodeList);

std::ostream& operator<<(std::ostream& os, const BootstrapEntry& entry) {
    os << " " << *(entry.info);

    return os;
}

void GlobalNodeList::initialize() {
    WATCH_UNORDERED_MAP(peerStorage.getPeerHashMap());
    WATCH_MAP(chords);
    WATCH_MAP(cans);
    WATCH_MAP(hosts);
    WATCH_MAP(canProfiles);
    WATCH_SET(zoneInCharges);
    WATCH_MAP(states);
}

void GlobalNodeList::handleMessage(cMessage* msg) {

}

void GlobalNodeList::addPeer(const IPvXAddress& ip, PeerInfo* info) {
    BootstrapEntry temp;
    temp.info = info;

    peerStorage.insert(std::make_pair(ip, temp));

    if (dynamic_cast<ChordInfo*>(info) != nullptr) {
        chords.insert( { dynamic_cast<ChordInfo*>(info)->getChordId(), ip });
        states.insert( { dynamic_cast<ChordInfo*>(info)->getChordId(), false });
    } else if (dynamic_cast<CANInfo*>(info) != nullptr) {
        cans.insert( { dynamic_cast<CANInfo*>(info)->getId(), ip });
        states.insert( { dynamic_cast<CANInfo*>(info)->getId(), false });
    } else {
        hosts.insert( { dynamic_cast<HostInfo*>(info)->getHostId(), ip });
    }
}

void GlobalNodeList::updateCANProfile(CANProfile profile) {
    unsigned long id = profile.getId();
    canProfiles[id] = profile;
}

CANProfile GlobalNodeList::getCANProfile(unsigned long canId) {
    if (canProfiles.count(canId) > 0) {
        return canProfiles[canId];
    } else {
        return CANProfile();
    }
}

void GlobalNodeList::addZone(CANProfile zone) {
    zoneInCharges.insert(zone);
}

void GlobalNodeList::delZone(CANProfile zone) {
    zoneInCharges.erase(zone);
}

int GlobalNodeList::zonesInCharge(unsigned long canId) {
    int zones = 0;
    for (auto elem : zoneInCharges) {
        if (elem.getId() == canId) {
            zones++;
        }
    }
    return zones;
}

map<unsigned long, IPvXAddress>& GlobalNodeList::getAllCANs() {
    return cans;
}

bool GlobalNodeList::validZone(CANProfile zone) {
    return zoneInCharges.count(zone) > 0;
}

void GlobalNodeList::killPeer(const IPvXAddress& ip) {
    PeerHashMap::iterator it = peerStorage.find(ip);
    if (it != peerStorage.end()) {
        if (dynamic_cast<ChordInfo*>(it->second.info) != nullptr) {
            unsigned long idToKill =
                    dynamic_cast<ChordInfo*>(it->second.info)->getChordId();
            chords.erase(idToKill);
            states.erase(idToKill);
        } else if (dynamic_cast<CANInfo*>(it->second.info) != nullptr) {
            unsigned long idToKill =
                    dynamic_cast<CANInfo*>(it->second.info)->getId();
            // remove CAN profile
            cans.erase(idToKill);
            canProfiles.erase(idToKill);
            states.erase(idToKill);

            // remove the zone(s) managed by the node
            for (set<CANProfile>::iterator it = zoneInCharges.begin();
                    it != zoneInCharges.end();) {
                CANProfile element = (*it);
                if (element.getId() == idToKill) {
                    it = zoneInCharges.erase(it);
                } else {
                    it++;
                }
            }
        } else {
            unsigned long idToKill =
                    dynamic_cast<HostInfo*>(it->second.info)->getHostId();
            hosts.erase(idToKill);
        }

        peerStorage.erase(it);
    }
}

PeerInfo* GlobalNodeList::getPeerInfo(const TransportAddress& peer) {
    return getPeerInfo(peer.getIp());
}

PeerInfo* GlobalNodeList::getPeerInfo(const IPvXAddress& ip) {
    PeerHashMap::iterator it = peerStorage.find(ip);

    if (it == peerStorage.end())
        return NULL;
    else
        return it->second.info;
}

std::vector<IPvXAddress>* GlobalNodeList::getAllIps() {
    std::vector<IPvXAddress>* ips = new std::vector<IPvXAddress>;

    const PeerHashMap::iterator it = peerStorage.begin();

    while (it != peerStorage.end()) {
        ips->push_back(it->first);
    }

    return ips;
}

bool GlobalNodeList::isUp(unsigned long id) {
    if (chords.count(id) > 0 || cans.count(id) > 0 || hosts.count(id) > 0) {
        return true;
    }
    return false;
}

bool GlobalNodeList::isReady(unsigned long id) {
    if ((chords.count(id) > 0 || cans.count(id) > 0) && states[id]) {
        return true;
    }
    return false;
}

void GlobalNodeList::ready(unsigned long id) {
    states[id] = true;
}

IPvXAddress GlobalNodeList::getNodeAddr(unsigned long id) {
    IPvXAddress addr;
    if (chords.count(id) > 0) {
        addr = chords[id];
    } else if (cans.count(id) > 0) {
        addr = cans[id];
    } else if (hosts.count(id) > 0) {
        addr = hosts[id];
    }
    return addr;
}

int GlobalNodeList::canSize() {
    return cans.size();
}

CANInfo* GlobalNodeList::randCAN() {
    // Retrieve all keys
    vector<unsigned long> keys;
    for (auto elem : cans)
        keys.push_back(elem.first);
    int random = (int) uniform(0, keys.size());
    unsigned long key = keys[random];
    IPvXAddress addr = cans[key];
    CANInfo* info = dynamic_cast<CANInfo*>(getPeerInfo(addr));
    return info;
}

int GlobalNodeList::chordSize() {
    return chords.size();
}

ChordInfo* GlobalNodeList::randChord() {
    // Retrieve all keys
    vector<unsigned long> keys;
    for (auto elem : chords)
        keys.push_back(elem.first);
    int random = (int) uniform(0, keys.size());
    unsigned long key = keys[random];
    IPvXAddress addr = chords[key];
    ChordInfo* info = dynamic_cast<ChordInfo*>(getPeerInfo(addr));
    return info;
}

