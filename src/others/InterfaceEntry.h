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

#ifndef __INTERFACEENTRY_H
#define __INTERFACEENTRY_H

#include <vector>
#include <omnetpp.h>
#include "../objects/IPv4InterfaceData.h"

using namespace omnetpp;

/**
 * Interface entry for the interface table in IInterfaceTable.
 *
 * @see IInterfaceTable
 */
class InterfaceEntry: public cNamedObject {
private:
    // copying not supported: following are private and also left undefined
    InterfaceEntry(const InterfaceEntry& obj);
    InterfaceEntry& operator=(const InterfaceEntry& obj);

protected:
    int interfaceId; ///< identifies the interface in the IInterfaceTable

    IPv4InterfaceData *ipv4data; ///< IPv4-specific interface info (IP address, etc)

public:
    InterfaceEntry();
    virtual ~InterfaceEntry() {
    }
    virtual void setInterfaceId(int id) {
        interfaceId = id;
    }
    virtual std::string info() const;
    virtual std::string detailedInfo() const;

    /** @name Field getters. Note they are non-virtual and inline, for performance reasons. */
    //@{
    int getInterfaceId() const {
        return interfaceId;
    }
    //@}

    /** @name Field setters */
    //@{
    virtual void setName(const char *s) {
        cNamedObject::setName(s);
    }
    //@}

    /** @name Accessing protocol-specific interface data. Note methods are non-virtual, for performance reasons. */
    //@{
    IPv4InterfaceData *ipv4Data() {
        return ipv4data;
    }
    //@}

    /** @name Installing protocol-specific interface data */
    //@{
    virtual void setIPv4Data(IPv4InterfaceData *p);
    //@}
};

#endif

