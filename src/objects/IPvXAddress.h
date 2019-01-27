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

#ifndef __INET_IPVXADDRESS_H
#define __INET_IPVXADDRESS_H

#include <cstdint>
#include <omnetpp.h>
#include <string.h>
#include "IPAddress.h"

using namespace omnetpp;

/**
 * Stores an IPv4 address. This class should be used everywhere
 * in transport layer and up, to guarantee IPv4 transparence.
 */
class IPvXAddress {
protected:
    uint32_t d[4];

public:
    /** name Constructors, destructor */
    //@{
    /**
     * Constructor for IPv4 addresses.
     */
    IPvXAddress() {
        d[0] = 0;
    }

    /**
     * Constructor for IPv4 addresses.
     */
    IPvXAddress(const IPAddress& addr) {
        set(addr);
    }

    /**
     * Accepts string representations suuported by IPAddress (dotted decimal
     * notation) and IPv6Address (hex string with colons). Throws an error
     * if the format is not recognized.
     */
    IPvXAddress(const char *addr) {
        set(addr);
    }

    /**
     * Copy constructor.
     */
    IPvXAddress(const IPvXAddress& addr) {
        set(addr);
    }

    /**
     * Destructor
     */
    ~IPvXAddress() {
    }
    //@}

    /**
     * Get IPv4 address. Throws exception if this is an IPv6 address.
     */
    IPAddress get4() const {
        return IPAddress(d[0]);
    }

    /**
     * Set to an IPv4 address.
     */
    void set(const IPAddress& addr) {
        d[0] = addr.getInt();
    }

    /**
     * Assignment
     */
    void set(const IPvXAddress& addr) {
        d[0] = addr.d[0];
    }

    /**
     * Accepts string representations supported by IPAddress (dotted decimal
     * notation). Throws an error if the format is not recognized.
     */
    void set(const char *addr) {
        if (!tryParse(addr))
            throw cRuntimeError(
                    "IPvXAddress: cannot interpret address string `%s'", addr);
    }

    /**
     * Assignment
     */
    IPvXAddress& operator=(const IPAddress& addr) {
        set(addr);
        return *this;
    }

    /**
     * Assignment
     */
    IPvXAddress& operator=(const IPvXAddress& addr) {
        set(addr);
        return *this;
    }

    /**
     * Parses and assigns the given address and returns true if the string is
     * recognized by IPAddress or IPv6Address, otherwise just returns false.
     */
    bool tryParse(const char *addr);

    /**
     * Returns the string representation of the address (e.g. "152.66.86.92")
     */
    std::string str() const {
        return get4().str();
    }
    //@}

    /** name Comparison */
    //@{
    /**
     * True if the structure has not been assigned any address yet.
     */
    bool isUnspecified() const {
        return d[0] == 0;
    }

    /**
     * Returns length of internal binary representation of address,
     * (count of 32-bit unsigned integers.)
     */
    int wordCount() const {
        return 1;
    }

    /**
     * Returns pointer to internal binary representation of address,
     * four 32-bit unsigned integers.
     */
    const uint32_t *words() const {
        return d;
    }

    /**
     * Returns true if the two addresses are equal
     */
    bool equals(const IPAddress& addr) const {
        return d[0] == addr.getInt();
    }

    /**
     * Returns true if the two addresses are equal
     */
    bool equals(const IPvXAddress& addr) const {
        return d[0] == addr.d[0];
    }

    /**
     * Returns equals(addr).
     */
    bool operator==(const IPAddress& addr) const {
        return equals(addr);
    }

    /**
     * Returns !equals(addr).
     */
    bool operator!=(const IPAddress& addr) const {
        return !equals(addr);
    }

    /**
     * Returns equals(addr).
     */
    bool operator==(const IPvXAddress& addr) const {
        return equals(addr);
    }

    /**
     * Returns !equals(addr).
     */
    bool operator!=(const IPvXAddress& addr) const {
        return !equals(addr);
    }

    /**
     * Compares two addresses.
     */
    bool operator<(const IPvXAddress& addr) const {
        return d[0] < addr.d[0];
    }
    //@}
};

inline std::ostream& operator<<(std::ostream& os, const IPvXAddress& ip) {
    return os << ip.str();
}

//inline void doPacking(cCommBuffer *buf, const IPvXAddress& addr) {
//    doPacking(buf, addr.get4());
//}
//
//inline void doUnpacking(cCommBuffer *buf, IPvXAddress& addr) {
//    IPAddress tmp;
//    doUnpacking(buf, tmp);
//    addr.set(tmp);
//}

#endif

