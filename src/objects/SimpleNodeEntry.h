//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004 Andras Varga
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
 * @file SimpleNodeEntry.h
 * @author Bernhard Heep
 * @date 2006/11/30
 */

#ifndef __SIMPLENODEENTRY_H
#define __SIMPLENODEENTRY_H

#include <omnetpp.h>

using namespace omnetpp;

/**
 * representation of a single node in the GlobalNodeList
 *
 * @author Bernhard Heep
 */
class SimpleNodeEntry: public cObject {
public:

    ~SimpleNodeEntry() {
    }

    /**
     * Simple constructor
     */
    SimpleNodeEntry(cModule* node);

    /**
     * Getter for SimpleUDP ingate
     *
     * @return the ingate
     */
    inline cGate* getUdpIPv4Gate() const {
        return UdpIPv4ingate;
    }
    ;

    /**
     * Getter for SimpleUDP ingate
     *
     * @return the ingate
     */
    inline cGate* getTcpIPv4Gate() const {
        return TcpIPv4ingate;
    }
    ;

    /**
     * OMNeT++ info method
     *
     * @return infostring
     */
    std::string info() const;

    /**
     * Stream output
     *
     * @param out output stream
     * @param entry the terminal
     * @return reference to stream out
     */
    friend std::ostream& operator<<(std::ostream& out,
            const SimpleNodeEntry& entry);

protected:
    cGate* UdpIPv4ingate; //!< IPv4 ingate of the SimpleUDP module of this terminal
    cGate* TcpIPv4ingate; //!< IPv4 ingate of the SimpleTCP module of this terminal
};

#endif // __SIMPLENODEENTRY_H
