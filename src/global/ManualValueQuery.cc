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

#include "ManualValueQuery.h"
#include <nlohmann/json.hpp>
#include "../messages/ChordMessage_m.h"
#include "../messages/CANMessage_m.h"
#include "../messages/UDPControlInfo_m.h"

using json = nlohmann::json;

Define_Module(ManualValueQuery);

ManualValueQuery::ManualValueQuery() {
    scan = NULL;
}

ManualValueQuery::~ManualValueQuery() {
    if (scan != NULL) {
        cancelAndDelete(scan);
    }
}

void ManualValueQuery::initialize() {
    scan_cycle = par("scan_cycle");
    scan = new cMessage(msg::MANUAL_QUERY);
    scheduleAt(simTime() + scan_cycle, scan);
    WATCH(id);
    WATCH(coordinate);
}

void ManualValueQuery::handleMessage(cMessage *msg) {
    if (msg->isName(msg::MANUAL_QUERY)) {
        if (!id.empty()) {
            EV << "id: " << endl;
            int m = GlobalParametersAccess().get()->getAddrSpaceSize();
//            unsigned long target = util::getSHA1(id, m);

            ChordInfo* sender = NULL;
            do {
                sender = GlobalNodeListAccess().get()->randChord();
            } while (sender == NULL
                    || !GlobalNodeListAccess().get()->isReady(
                            sender->getChordId()));
            ChordMessage* message = new ChordMessage(msg::CHORD_LOOK_UP);
            message->setSender(sender->getChordId());
            message->setHop(0);
            message->setType(ChordMsgType::CHORD_GET);
            message->setLabel(msg::LABEL_TEST);
            json content;
//            content["id"] = target;
            content["id"] = util::strToLong(id);
            message->setContent(content.dump().c_str());
            IPvXAddress destAddr = GlobalNodeListAccess().get()->getNodeAddr(
                    sender->getChordId());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(destAddr);
            message->setControlInfo(udpControlInfo);
            SimpleNodeEntry* destEntry = sender->getEntry();
            cSimpleModule::sendDirect(message, 0, 0,
                    destEntry->getTcpIPv4Gate());

            id.clear();
        } else if (!coordinate.empty()) {
            EV << "coordinate: " << endl;
            vector<string> parts;
            util::splitString(coordinate, ",", parts);
            int x = util::strToInt(parts[0]);
            int y = util::strToInt(parts[1]);

            CANInfo* sender = NULL;
            do {
                sender = GlobalNodeListAccess().get()->randCAN();
            } while (sender == NULL
                    || !GlobalNodeListAccess().get()->isReady(sender->getId()));
            CANMessage* message = new CANMessage(msg::CAN_LOOK_UP);
            message->setSender(sender->getId());
            message->setHop(0);
            message->setType(CANMsgType::CAN_GET);
            message->setLabel(msg::LABEL_TEST);
            json content;
            content["x"] = x;
            content["y"] = y;
            message->setContent(content.dump().c_str());
            IPvXAddress destAddr = GlobalNodeListAccess().get()->getNodeAddr(
                    sender->getId());
            UDPControlInfo* udpControlInfo = new UDPControlInfo();
            udpControlInfo->setDestAddr(destAddr);
            message->setControlInfo(udpControlInfo);
            SimpleNodeEntry* destEntry = sender->getEntry();
            cSimpleModule::sendDirect(message, 0, 0,
                    destEntry->getTcpIPv4Gate());

            coordinate.clear();
        }
        scheduleAt(simTime() + scan_cycle, scan);
    }
}
