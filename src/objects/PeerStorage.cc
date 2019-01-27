//
// Copyright (C) 2010 Karlsruhe Institute of Technology (KIT),
//                    Institute of Telematics
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
 * @file PeerStorage.cc
 * @author Ingmar Baumgart
 */

#include "PeerInfo.h"

#include "PeerStorage.h"

PeerStorage::~PeerStorage() {
    PeerHashMap::iterator it;
    for (it = peerHashMap.begin(); it != peerHashMap.end(); it++) {
        delete it->second.info;
    }
}

size_t PeerStorage::size() {
    return peerHashMap.size();
}

const PeerHashMap::iterator PeerStorage::find(const IPvXAddress& ip) {
    return peerHashMap.find(ip);
}

const PeerHashMap::iterator PeerStorage::begin() {
    return peerHashMap.begin();
}

const PeerHashMap::iterator PeerStorage::end() {
    return peerHashMap.end();
}

size_t PeerStorage::offsetSize() {
    return 1 << 2;
}

uint8_t PeerStorage::calcOffset(bool bootstrapped, bool malicious) {
    uint8_t offset = 0;
    if (bootstrapped)
        offset += 1 << 0;
    if (malicious)
        offset += 1 << 1;
    return offset;
}

std::pair<const PeerHashMap::iterator, bool> PeerStorage::insert(
        const std::pair<IPvXAddress, BootstrapEntry>& element) {
    std::pair<PeerHashMap::iterator, bool> ret;

    ret = peerHashMap.insert(element);

    return ret;
}

void PeerStorage::erase(const PeerHashMap::iterator it) {
    delete it->second.info;
    peerHashMap.erase(it);
}
