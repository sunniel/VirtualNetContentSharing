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
 * @file PeerInfo.h
 * @author Helge Backhaus
 * @author Stephan Krause
 */

#ifndef __PEERINFO_H__
#define __PEERINFO_H__

#include <sstream>
#include <omnetpp.h>

using namespace std;

class PeerInfo;

/**
 * Base class for providing additional underlay specific
 * information associated with a certain transport address
 */
class PeerInfo {
    friend class PeerStorage;
public:

    /**
     * simple constructor
     */
    PeerInfo(int moduleId);

    PeerInfo(int moduleId, string moduleName);

    virtual ~PeerInfo() {
    }
    ;

    /**
     * returns the moduleId of the peer
     *
     * @return the moduleId
     */
    int getModuleID() {
        return moduleId;
    }
    ;

    /**
     * standard output stream for PeerInfo,
     * gives moduleID and true if peer has bootstrapped, false otherwise
     *
     * @param os the ostream
     * @param info the PeerInfo
     * @return the output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const PeerInfo info);

private:
    virtual void dummy(); /**< dummy-function to make PeerInfo polymorphic */
protected:
    int moduleId; /**< the moduleId of the host */
    string moduleName; /**< the moduleId of the host */
};

#endif
