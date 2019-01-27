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

#include "InterfaceTable.h"

Define_Module(InterfaceTable);

#define INTERFACEIDS_START  100

std::ostream& operator<<(std::ostream& os, const InterfaceEntry& e) {
    os << e.info();
    return os;
}

InterfaceTable::InterfaceTable() {
    tmpNumInterfaces = -1;
    tmpInterfaceList = NULL;
}

InterfaceTable::~InterfaceTable() {
    for (int i = 0; i < (int) idToInterface.size(); i++)
        delete idToInterface[i];
    delete[] tmpInterfaceList;
}

void InterfaceTable::initialize(int stage) {
    WATCH_PTRVECTOR(idToInterface);
    updateDisplayString();
}

void InterfaceTable::updateDisplayString() {
    if (!cComponent::hasGUI())
        return;

    char buf[80];
    sprintf(buf, "%d interfaces", getNumInterfaces());
    getDisplayString().setTagArg("t", 0, buf);
}

void InterfaceTable::handleMessage(cMessage *msg) {
    error("This module doesn't process messages");
}

//---

int InterfaceTable::getNumInterfaces() {
    if (tmpNumInterfaces == -1) {
        // count non-NULL elements
        int n = 0;
        int maxId = idToInterface.size();
        for (int i = 0; i < maxId; i++)
            if (idToInterface[i])
                n++;
        tmpNumInterfaces = n;
    }

    return tmpNumInterfaces;
}

void InterfaceTable::addInterface(InterfaceEntry *entry) {
    // check name is unique
    if (getInterfaceByName(entry->getName()) != NULL)
        error("addInterface(): interface '%s' already registered",
                entry->getName());

    // insert
    entry->setInterfaceId(INTERFACEIDS_START + idToInterface.size());
    idToInterface.push_back(entry);
    invalidateTmpInterfaceList();
    updateDisplayString();
}

void InterfaceTable::invalidateTmpInterfaceList() {
    tmpNumInterfaces = -1;
    delete[] tmpInterfaceList;
    tmpInterfaceList = NULL;
}

InterfaceEntry *InterfaceTable::getInterfaceByName(const char *name) {
    Enter_Method_Silent();
    if (!name)
        return NULL;
    int n = idToInterface.size();
    for (int i = 0; i < n; i++)
        if (idToInterface[i] && !strcmp(name, idToInterface[i]->getName()))
            return idToInterface[i];
    return NULL;
}

