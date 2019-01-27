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
 * @file SimpleNodeEntry.cc
 * @author Bernhard Heep
 */

#include <sstream>

#include "SimpleNodeEntry.h"

// end of behaviour code

SimpleNodeEntry::SimpleNodeEntry(cModule* node) {

    UdpIPv4ingate = node->gateHalf("udpgate", cGate::INPUT);
    TcpIPv4ingate = node->gateHalf("tcpgate", cGate::INPUT);
}

std::string SimpleNodeEntry::info() const {
    std::ostringstream str;
    str << *this;
    return str.str();
}

std::ostream& operator<<(std::ostream& out, const SimpleNodeEntry& entry) {
    // TODO something to do later
    return out;
}
