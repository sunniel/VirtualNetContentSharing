//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __INTERFACETABLEACCESS_H_
#define __INTERFACETABLEACCESS_H_

#include <omnetpp.h>
#include "InterfaceTable.h"

/**
 * Gives access to the InterfaceTable module in the given parent module.
 *
 */
class InterfaceTableAccess {
public:
    /**
     * returns the InterfaceTable module in the given parent module
     *
     * @return the InterfaceTable module
     */
    InterfaceTable* get(cModule* parent) {
        return (InterfaceTable*) parent->getSubmodule("interfaceTable");
    }
};

#endif /* INTERFACETABLEACCESS_H_ */
