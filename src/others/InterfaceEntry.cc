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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <sstream>
#include "InterfaceEntry.h"
#include "../objects/IPv4InterfaceData.h"

InterfaceEntry::InterfaceEntry() {
    ipv4data = NULL;
}

std::string InterfaceEntry::info() const {
    std::stringstream out;
    out << (getName()[0] ? getName() : "*");
    out << "\n";
    if (ipv4data)
        out << " " << ((cObject*) ipv4data)->info(); // Khmm...
    return out.str();
}

std::string InterfaceEntry::detailedInfo() const {
    std::stringstream out;
    out << "name:" << (getName()[0] ? getName() : "*");
    out << "\n";
    if (ipv4data)
        out << " " << ((cObject*) ipv4data)->info() << "\n"; // Khmm...

    return out.str();
}

void InterfaceEntry::setIPv4Data(IPv4InterfaceData *p) {
    ipv4data = p;
}

