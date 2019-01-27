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

#ifndef __VIRTUALNETCONTENTSHARING_CONTENTDISTRIBUTOR_H_
#define __VIRTUALNETCONTENTSHARING_CONTENTDISTRIBUTOR_H_

#include <omnetpp.h>
#include <nlohmann/json.hpp>
#include "../objects/Coordinate.h"

using namespace omnetpp;
using namespace std;
using json = nlohmann::json;

/**
 * TODO - Generated class
 */
class ContentDistributor: public cSimpleModule {
    int contentNum;
    map<Coordinate, json, CoordntCompare> inventories;
    // object-address (i.e., chord host Id) mappings
    map<string, unsigned long> mappings;
    int deployedInv;
    int deployedObj;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
public:
    bool ready;

    ContentDistributor();
    void generate();
    void callback(string object, unsigned long addrBot);
    void deployed(string type);
};

#endif
