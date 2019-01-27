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

#include "ContentDistributor.h"
#include "CoordinatorAccess.h"
#include "GlobalNodeListAccess.h"
#include "GlobalParametersAccess.h"
#include "../objects/ChordInfo.h"
#include "../objects/CANInfo.h"
#include "../common/Util.h"
#include "../common/Constants.h"
#include "../messages/ChordMessage_m.h"
#include "../messages/CANMessage_m.h"
#include "../messages/UDPControlInfo_m.h"

Define_Module(ContentDistributor);

ContentDistributor::ContentDistributor() {
    deployedInv = 0;
    deployedObj = 0;
    ready = false;
}

void ContentDistributor::initialize() {
    contentNum = par("content_num");
}

void ContentDistributor::handleMessage(cMessage *msg) {
    // TODO - Generated method body
}

void ContentDistributor::generate() {
    // add the macro for context switch
    Enter_Method_Silent();

    long regionSize = CoordinatorAccess().get()->regionSize;
    int regionsInRow = ceil((double) CoordinatorAccess().get()->XMax / regionSize);
    int regionsInColumn = ceil((double) CoordinatorAccess().get()->YMax / regionSize);

    cout << "regionsInRow: " << regionsInRow << endl;
    cout << "regionsInColumn: " << regionsInColumn << endl;

    // generate inventories
    Coordinate cursor(0, 0);
    for (int i = 0; i < regionsInRow; i++) {
        cursor.x = i * regionSize;
        for (int j = 0; j < regionsInColumn; j++) {
            cursor.y = j * regionSize;
            json inventory;
            inventory["rid"] = util::getSHA1(cursor.str(), GlobalParametersAccess().get()->getAddrSpaceSize());
            inventory["type"] = data::DATA_TYPE_INVENTORY;
            json coord;
            coord["x"] = cursor.x;
            coord["y"] = cursor.y;
            inventory["rCoord"] = coord;
            cout << "inventory: " << inventory << endl;
            inventories[cursor] = inventory;
        }
    }

    cout << "inventories size: " << inventories.size() << endl;

    // generate objects
    string namePrefix = "Object_";
    set<json> objects;

//    int count = 0;
//    long radius = 12727922;
//    Coordinate origin = CoordinatorAccess().get()->centerLocation();
//    Coordinate r = CoordinatorAccess().get()->getRegion(origin);
//    cout << "region: " << r << endl;
//    vector<Coordinate> regions = CoordinatorAccess().get()->neighborRegions(r);
//    cout << "regions: " << endl;
//    for(auto elem: regions) {
//        cout << elem << endl;
//    }

    set<unsigned long> idList;
    int multiplier = 0;
    for (int i = 0; i < contentNum; i++) {
        json obj;
        unsigned long oid = util::getSHA1(namePrefix + to_string(i + multiplier * contentNum), GlobalParametersAccess().get()->getAddrSpaceSize());
        while(idList.count(oid) > 0) {
            multiplier++;
            string seed = namePrefix + to_string(i + multiplier * contentNum);
            oid = util::getSHA1(seed, GlobalParametersAccess().get()->getAddrSpaceSize());
        }
        idList.insert(oid);
        multiplier = 0;
        obj["oid"] = oid;
        obj["name"] = namePrefix + to_string(i);
        obj["type"] = data::DATA_TYPE_OBJECT;
        Coordinate coord(CoordinatorAccess().get()->randomX(),
        CoordinatorAccess().get()->randomY());
        json pos;
        pos["x"] = coord.x;
        pos["y"] = coord.y;
        obj["oCoord"] = pos;
        Coordinate region = CoordinatorAccess().get()->getRegion(coord);

//        cout << " object " << obj << " at region: " << region << endl;
//        long dist = distance(coord, origin);
//        if (dist <= radius) {
//            cout << "object " << coord << " in region " << region << endl;
//            count++;
//        }

        objects.insert(obj);
        json inventory = inventories[region];
        inventories[region]["object"].push_back(obj);

        /*
         * display the object on the canvas with an icon
         */
        auto icon = new cImageFigure("object");
        icon->setImageName("customized/flag_small");
        icon->setVisible(true);
        Coordinate coordOnMap = CoordinatorAccess().get()->mapLocation(coord);

//        cout << "object position: " << coordOnMap << endl;

        cFigure::Point p((double)coordOnMap.x, (double)coordOnMap.y);
        icon->setPosition(p);
        getParentModule()->getCanvas()->addFigure(icon);
    }

//    cout << "number of objects within the center: " << count << endl;

    /*
     * distribute inventories and objects to overlay networks
     */
    // distribute objects into Chord
    ChordInfo* chord = NULL;
    do {
        chord = GlobalNodeListAccess().get()->randChord();
    }while (chord == NULL
    || !GlobalNodeListAccess().get()->isReady(chord->getChordId()));
    for (auto elem : objects) {
        ChordMessage* message = new ChordMessage(msg::CHORD_LOOK_UP);
        message->setSender(chord->getChordId());
        message->setHop(0);
        message->setType(ChordMsgType::CHORD_STORE);
        json content;
        content["id"] = elem["oid"];
        content["value"] = elem.dump();
        message->setContent(content.dump().c_str());
        message->setLabel(msg::LABEL_INIT);
        IPvXAddress destAddr = GlobalNodeListAccess().get()->getNodeAddr(
        chord->getChordId());
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(destAddr);
        message->setControlInfo(udpControlInfo);
        SimpleNodeEntry* destEntry = chord->getEntry();
        cSimpleModule::sendDirect(message, 0, 0, destEntry->getTcpIPv4Gate());
    }

    // distribute inventories into CAN
    CANInfo* can = NULL;
    do {
        can = GlobalNodeListAccess().get()->randCAN();
    }while (can == NULL || !GlobalNodeListAccess().get()->isReady(can->getId()));
    for (auto elem : inventories) {
        CANMessage* message = new CANMessage(msg::CAN_LOOK_UP);
        message->setSender(can->getId());
        message->setHop(0);
        message->setType(CANMsgType::CAN_STORE);
        json content;
        content["x"] = elem.first.x;
        content["y"] = elem.first.y;
        content["value"] = elem.second.dump();
        content["type"] = data::DATA_TYPE_INVENTORY;
        message->setContent(content.dump().c_str());
        message->setLabel(msg::LABEL_INIT);
        IPvXAddress destAddr = GlobalNodeListAccess().get()->getNodeAddr(can->getId());
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(destAddr);
        message->setControlInfo(udpControlInfo);
        SimpleNodeEntry* destEntry = can->getEntry();
        cSimpleModule::sendDirect(message, 0, 0, destEntry->getTcpIPv4Gate());
    }
}

void ContentDistributor::callback(string object, unsigned long addrBot) {
    // add the macro for context switch
    Enter_Method_Silent();

    mappings[object] = addrBot;
    if(mappings.size() == contentNum) {
        json mapping;

        CANInfo* sender = NULL;
        do {
            sender = GlobalNodeListAccess().get()->randCAN();
        }while (sender == NULL
        || !GlobalNodeListAccess().get()->isReady(sender->getId()));

        for(auto elem : mappings) {
            json obj = json::parse(elem.first);
            obj["mapping"] = elem.second;
            CANMessage* message = new CANMessage(msg::CAN_LOOK_UP);
            message->setSender(sender->getId());
            message->setHop(0);
            message->setType(CANMsgType::CAN_STORE);
            message->setLabel(msg::LABEL_INIT);
            json content;
            content["x"] = obj["oCoord"]["x"];
            content["y"] = obj["oCoord"]["y"];
            content["value"] = obj.dump();
            content["type"] = data::DATA_TYPE_OBJECT;
            message->setContent(content.dump().c_str());
            IPvXAddress destAddr = GlobalNodeListAccess().get()->getNodeAddr(
            sender->getId());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(destAddr);
            message->setControlInfo(udpControlInfo);
            SimpleNodeEntry* destEntry = sender->getEntry();
            cSimpleModule::sendDirect(message, 0, 0, destEntry->getTcpIPv4Gate());
        }
    }
}

void ContentDistributor::deployed(string type) {
    if (type == data::DATA_TYPE_INVENTORY) {

//        cout << "Inventory deployed" << endl;

        deployedInv++;
    } else if (type == data::DATA_TYPE_OBJECT) {

//        cout << "object mapping deployed" << endl;

        deployedObj++;
    }
    if (deployedInv + deployedObj >= inventories.size() + mappings.size()) {
        ready = true;
    }
}
