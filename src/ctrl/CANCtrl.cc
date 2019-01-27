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

#include "CANCtrl.h"
#include "../common/Util.h"
#include "../common/Constants.h"
#include "../global/GlobalStatisticsAccess.h"
#include "../global/GlobalNodeListAccess.h"
#include "../global/UnderlayConfiguratorAccess.h"
#include "../global/CoordinatorAccess.h"

Define_Module(CANCtrl);

CANCtrl::CANCtrl() {
    maintainer = new cMessage(msg::CAN_MAINT);
    load = 0;
    birth = simtime_t::ZERO;
}

void CANCtrl::final() {
    ;
}

CANCtrl::~CANCtrl() {
    // destroy self timer messages
    cancelAndDelete(maintainer);
}

int CANCtrl::numInitStages() const {
    return 2;
}

void CANCtrl::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();

        maintain_cycle = par("maintain_cycle");

        WATCH(info);
        WATCH_SET(failedHosts);
        WATCH_VECTOR(neighbors);
        WATCH_VECTOR(takeovers);
        WATCH_MAP(data);

    } else if (stage == 1) {
        getParentModule()->getDisplayString().setTagArg("t", 0,
                to_string(info.getId()).c_str());
    }
}

void CANCtrl::r_transmit(CANMessage* msg, unsigned long destID) {
    if (GlobalNodeListAccess().get()->isUp(destID)) {
        // in case of message forwarding
        msg->removeControlInfo();
        IPvXAddress srcAddr = GlobalNodeListAccess().get()->getNodeAddr(
                info.getId());
        IPvXAddress destAddr = GlobalNodeListAccess().get()->getNodeAddr(
                destID);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(destAddr);
        udpControlInfo->setSrcAddr(srcAddr);
        msg->setControlInfo(udpControlInfo);
        string label = msg->getLabel();
        if (label.find(msg::LABEL_INIT) != string::npos) { // for overlay initialization
            sendIdeal(msg);
        } else {
            HostBase::r_transmit(msg, destAddr);
        }
    } else {

        // failed to find the next hop
        if (msg->getType() == CANMsgType::JOIN) {

            cout << simTime() << " join failed at hop " << msg->getHop()
                    << ", due to destination " << destID << " failed" << endl;

            GlobalStatisticsAccess().get()->JOIN_FAILS++;
        } else if (msg->getType() == CANMsgType::CAN_LOOK_UP
                || msg->getType() == CANMsgType::CAN_STORE
                || msg->getType() == CANMsgType::CAN_GET) {

            cout << simTime() << " lookup failed at hop " << msg->getHop()
                    << ", due to destination " << destID << " failed" << endl;

            GlobalStatisticsAccess().get()->FAILS++;
            handleFailure(msg, info);
        } else {
            cout << simTime() << " route failed, due to destination " << destID
                    << " failed" << endl;
        }

        delete msg;
    }
}

void CANCtrl::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::CAN_LOOK_UP)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onRoute(canMsg);
    } else if (msg->isName(msg::CAN_FINAL)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onFinal(canMsg);
    } else if (msg->isName(msg::CAN_JOIN_REPLY)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onJoinReply(canMsg);
    } else if (msg->isName(msg::CAN_ADD_NEIGHBOR)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onNeighborAdd(canMsg);
    } else if (msg->isName(msg::CAN_RM_NEIGHBOR)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onNeighborRemove(canMsg);
    } else if (msg->isName(msg::CAN_JOIN_REPLY_FAIL)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onJoinReplyFail(canMsg);
    } else if (msg->isName(msg::CAN_UPDATE_AREA)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onAreaUpdate(canMsg);
    } else if (msg->isName(msg::CAN_NEIGHBOR_UPDATE)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onNeighborUpdate(canMsg);
    } else if (msg->isName(msg::CAN_NEIGHBOR_TAKEOVER)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onNeighborTakeover(canMsg);
    } else if (msg->isName(msg::CAN_NEIGHBOR_EXCHANGE)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onNeighborExchange(canMsg);
    } else if (msg->isName(msg::CAN_ERS)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onNeighborRequest(canMsg);
    } else if (msg->isName(msg::CAN_ERS_REPLY)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onNeighborReply(canMsg);
    } else if (msg->isName(msg::CAN_MAINT)) {
        maintain(msg);
    } else if (msg->isName(msg::CAN_REPLICATE)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onReplicate(canMsg);
    } else if (msg->isName(msg::CAN_FIX_REPLICA)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onFixReplicas(canMsg);
    } else if (msg->isName(msg::CAN_FIX_LOAD)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onFixLoad(canMsg);
    } else if (msg->isName(msg::CHORD_REPLY)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onMappingReply(chordMsg);
    } else if (msg->isName(msg::CAN_MAPPING_QUERY)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onMappingQuery(canMsg);
    }
}

void CANCtrl::onRoute(CANMessage* msg) {
    if(msg->getType() == CANMsgType::CAN_GET){
        load++;
    }

    string content = msg->getContent();
    json lookupInfo = json::parse(content);
    position p;
    p.x = lookupInfo["x"];
    p.y = lookupInfo["y"];

//    cout << simTime() << " " << info << " received request from "
//            << msg->getSender() << " for " << content << endl;

    vector<int> nextHopArea;
    if (lookupInfo.find("area") != lookupInfo.end()) {
        nextHopArea = lookupInfo["area"].get<vector<int>>();
    } else {
        nextHopArea = info.getArea();
    }
    CANProfile targetProfile(info.getId(), nextHopArea);

    if (!includedInArea(p, nextHopArea)
            || (targetProfile != info
                    && std::find(takeovers.begin(), takeovers.end(),
                            targetProfile) == takeovers.end())) { // zone in charge changed
        if (msg->getHop() < GlobalParametersAccess().get()->getMaxHop()) {
            CANProfile next;
            if (info == targetProfile) {
                next = routeToNext(p, targetProfile, neighbors);
            } else if (neighborsOftakeovers.count(targetProfile) > 0) {
                next = routeToNext(p, targetProfile,
                        neighborsOftakeovers[targetProfile]);
            }

//            cout << simTime() << " lookup " << msg->getContent() << " on "
//                    << targetProfile << " next: " << next << endl;

            if (next.isEmpty()) {
                // failed to find the next hop
                if (msg->getType() == CANMsgType::JOIN) {

                    cout << simTime() << " " << msg->getSender()
                            << " join failed on " << targetProfile
                            << ", due to route to next failed" << endl;

                    GlobalStatisticsAccess().get()->JOIN_FAILS++;
                    IPvXAddress senderAddr =
                            GlobalNodeListAccess().get()->getNodeAddr(
                                    msg->getSender());
                    UnderlayConfiguratorAccess().get()->removeNode(senderAddr);
                } else {

                    cout << simTime()
                            << " lookup failed, due to route to next for "
                            << targetProfile << " failed" << endl;

                    GlobalStatisticsAccess().get()->FAILS++;
                    handleFailure(msg, info);
                }
                delete msg;
            } else {
                msg->setHop(msg->getHop() + 1);
                lookupInfo["area"] = next.getArea();
                msg->setContent(lookupInfo.dump().c_str());
                r_transmit(msg, next.getId());
            }
        } else {
            if (msg->getType() == CANMsgType::JOIN) {

                cout << simTime() << " " << msg->getSender()
                        << " join failed, due to max hop reached" << endl;

                GlobalStatisticsAccess().get()->JOIN_FAILS++;
                IPvXAddress senderAddr =
                        GlobalNodeListAccess().get()->getNodeAddr(
                                msg->getSender());
                UnderlayConfiguratorAccess().get()->removeNode(senderAddr);
            } else {

                cout << simTime()
                        << " lookup failed, due to max hop reached for "
                        << targetProfile << endl;

                GlobalStatisticsAccess().get()->FAILS++;
                handleFailure(msg, info);
            }
            delete msg;
        }

    } else {
        // context merge
        CANProfile info = targetProfile;
        CANProfile oldInfo = info;
        vector<CANProfile> neighbors;
        if (info == this->info) {
            neighbors = this->neighbors;
        } else {
            neighbors = neighborsOftakeovers[info];
        }

        if (msg->getType() == CANMsgType::CAN_LOOK_UP) {
            CANMessage* finalmsg = new CANMessage(msg::CAN_FINAL);
            finalmsg->setType(CANMsgType::CAN_FINAL);
//            finalmsg->setContent(profile.dump().c_str());
            finalmsg->setHop(msg->getHop() + 1);
            finalmsg->setLabel(msg->getLabel());
            finalmsg->setSender(info.getId());
            finalmsg->setLabel(msg->getLabel());
            r_transmit(finalmsg, msg->getSender());
        } else if (msg->getType() == CANMsgType::CAN_STORE) {
            string value = lookupInfo["value"];
            string type = lookupInfo["type"];
            if (type == data::DATA_TYPE_INVENTORY) {
                inventories[p] = value;
            } else if (type == data::DATA_TYPE_OBJECT) {
                data[p] = value;
            }
//            GlobalStatisticsAccess().get()->addHop(msg->getHop());
            // replicate value to successors
            for (int i = 0; i < neighbors.size(); i++) {
                CANMessage* replicate = new CANMessage(msg::CAN_REPLICATE);
                replicate->setSender(info.getId());
                replicate->setContent(msg->getContent());
                replicate->setLabel(msg->getLabel());
                r_transmit(replicate, neighbors[i].getId());
            }

            ContentDistributorAccess().get()->deployed(type);

//            cout << simTime() << " Store value " << value << " on " << info
//                    << endl;
        } else if (msg->getType() == CANMsgType::CAN_GET) {
//            GlobalStatisticsAccess().get()->addHop(msg->getHop());
            string label = msg->getLabel();
            if (label.find(msg::LABEL_TEST) != string::npos) {
                if (data.count(p) > 0) {
                    cout << "Return GET result: " << data[p] << " from " << info
                            << endl;
                    GlobalStatisticsAccess().get()->SUCCESS++;
                } else {
                    cout << "Return GET result: " << data::DATA_EMPTY
                            << " from " << info << endl;
                    GlobalStatisticsAccess().get()->FAILS++;
                }
            } else if (label.find(msg::LABEL_INVENTORY) != string::npos) {
                if (!replyInventory(msg, p)) {
                    handleFailure(msg, info);
                }
            } else if (label.find(msg::LABEL_CONTENT) != string::npos) {
                if (!replyContent(msg, info, neighbors)) {
                    handleFailure(msg, info);
                }
            }
        } else if (msg->getType() == CANMsgType::JOIN) {
            if (!takeovers.empty()) {
                CANProfile toTakeover = takeovers[takeovers.size() - 1];
                json joinInfo;

                // notify neighbors to update the neighbor
                vector<CANProfile> neighborsToNotify =
                        neighborsOftakeovers[toTakeover];
                for (auto elem : neighborsToNotify) {
                    json update;
                    update["oldId"] = info.getId();
                    update["newId"] = msg->getSender();
                    update["area"] = toTakeover.getArea();
                    update["neighborId"] = elem.getId();
                    update["neighborArea"] = elem.getArea();
                    CANMessage* updateNeighbor = new CANMessage(
                            msg::CAN_NEIGHBOR_UPDATE);
                    updateNeighbor->setType(CANMsgType::NEIGHBOR_UPDATE);
                    updateNeighbor->setContent(update.dump().c_str());
                    updateNeighbor->setLabel(msg->getLabel());
                    r_transmit(updateNeighbor, elem.getId());

                    json neighbor;
                    neighbor["id"] = elem.getId();
                    neighbor["area"] = elem.getArea();
                    joinInfo["neighbor"].push_back(neighbor);
                }

                // hand over the zone to the join node
                CANMessage* joinReply = new CANMessage(msg::CAN_JOIN_REPLY);
                joinReply->setType(CANMsgType::JOIN_REPLY);
                joinInfo["area"] = toTakeover.getArea();
                joinReply->setContent(joinInfo.dump().c_str());
                joinReply->setSender(info.getId());
                joinReply->setLabel(msg->getLabel());
                r_transmit(joinReply, msg->getSender());

                // remove the zone in charge
                takeovers.pop_back();
                neighborsOftakeovers.erase(toTakeover);
                nnTakeovers.erase(toTakeover);
                TTLs.erase(toTakeover);
                GlobalNodeListAccess().get()->delZone(toTakeover);
            } else {
                // split zone
                vector<vector<int>> subzones = splitZone(info.getArea());
                if (subzones.size() > 0) {
                    info.setArea(subzones[0]);
                    GlobalNodeListAccess().get()->updateCANProfile(info);
                    GlobalNodeListAccess().get()->delZone(
                            CANProfile(info.getId(), oldInfo.getArea()));
                    GlobalNodeListAccess().get()->addZone(info);

                    // TODO update Virtual ID in zone re-assignment, if implemented
                    // TODO update zone overload, if implemented

                    // update neighbors
                    vector<CANProfile> neighborToSend;
                    vector<CANProfile> neighborToRemove;
                    for (auto elem : neighbors) {
                        if (commonEdge(elem.getArea(), subzones[1])) {
                            neighborToSend.push_back(elem);
                        }
                        if (!commonEdge(elem.getArea(), subzones[0])) {
                            neighborToRemove.push_back(elem);
                        }
                    }

                    // notify neighbors to remove the host node
                    for (auto elem : neighborToRemove) {
                        json remove;
                        remove["id"] = info.getId();
                        remove["area"] = oldInfo.getArea();
                        remove["neighborId"] = elem.getId();
                        remove["neighborArea"] = elem.getArea();
                        CANMessage* rmNeighbor = new CANMessage(
                                msg::CAN_RM_NEIGHBOR);
                        rmNeighbor->setType(CANMsgType::RM_NEIGHBOR);
                        rmNeighbor->setContent(remove.dump().c_str());
                        rmNeighbor->setLabel(msg->getLabel());
                        r_transmit(rmNeighbor, elem.getId());
                        neighbors.erase(
                                std::remove(neighbors.begin(), neighbors.end(),
                                        elem));
                    }

                    // notify neighbors to update the host node
                    for (auto elem : neighbors) {
                        json update;
                        update["id"] = info.getId();
                        update["oldArea"] = oldInfo.getArea();
                        update["newArea"] = info.getArea();
                        update["neighborId"] = elem.getId();
                        update["neighborArea"] = elem.getArea();
                        CANMessage* updateArea = new CANMessage(
                                msg::CAN_UPDATE_AREA);
                        updateArea->setType(CANMsgType::UPDATE_AREA);
                        updateArea->setContent(update.dump().c_str());
                        updateArea->setLabel(msg->getLabel());
                        r_transmit(updateArea, elem.getId());
                    }

                    // notify neighbors to add the join node
                    for (auto elem : neighborToSend) {
                        json newNode;
                        newNode["id"] = msg->getSender();
                        newNode["area"] = subzones[1];
                        newNode["neighborId"] = elem.getId();
                        newNode["neighborArea"] = elem.getArea();
                        CANMessage* addNeighbor = new CANMessage(
                                msg::CAN_ADD_NEIGHBOR);
                        addNeighbor->setType(CANMsgType::ADD_NEIGHBOR);
                        addNeighbor->setContent(newNode.dump().c_str());
                        addNeighbor->setLabel(msg->getLabel());
                        r_transmit(addNeighbor, elem.getId());
                    }

                    // add the join node to the neighbor list
                    CANProfile joinNode;
                    joinNode.setId(msg->getSender());
                    joinNode.setArea(subzones[1]);
                    neighbors.push_back(joinNode);

                    // notify the join node of success
                    // add self in the neighbor list of the join node
                    neighborToSend.push_back(info);
                    CANMessage* joinReply = new CANMessage(msg::CAN_JOIN_REPLY);
                    joinReply->setType(CANMsgType::JOIN_REPLY);
                    json joinInfo;
                    joinInfo["area"] = subzones[1];
                    for (auto elem : neighborToSend) {
                        json neighbor;
                        neighbor["id"] = elem.getId();
                        neighbor["area"] = elem.getArea();
                        joinInfo["neighbors"].push_back(neighbor);
                    }
                    joinReply->setContent(joinInfo.dump().c_str());
                    joinReply->setSender(info.getId());
                    joinReply->setLabel(msg->getLabel());
                    r_transmit(joinReply, msg->getSender());
                } else {
                    // notify the join node of failure
                    CANMessage* joinReply = new CANMessage(
                            msg::CAN_JOIN_REPLY_FAIL);
                    joinReply->setType(CANMsgType::JOIN_REPLY_FAIL);
                    //                joinReply->setContent(joinInfo.dump().c_str());
                    joinReply->setSender(info.getId());
                    joinReply->setLabel(msg->getLabel());
                    r_transmit(joinReply, msg->getSender());
                }
            }
        }

        // modify neighbor list in consistent with the context
        if (oldInfo == this->info) {
            this->neighbors = neighbors;
            this->info = info;
        } else {
            if (std::find(takeovers.begin(), takeovers.end(), oldInfo)
                    != takeovers.end()) {
                std::replace(takeovers.begin(), takeovers.end(), oldInfo, info);
                neighborsOftakeovers[oldInfo] = neighbors;
            }
        }

        delete msg;
    }
}

void CANCtrl::onJoinReplyFail(CANMessage* msg) {
    string label = msg->getLabel();
    if (label.find(msg::LABEL_INIT) != string::npos) {
        UnderlayConfiguratorAccess().get()->initCANAdded();
    }

    cout << simTime() << " " << info
            << " join failed, unable to find a zone for the new node" << endl;

    GlobalStatisticsAccess().get()->JOIN_FAILS++;
    UnderlayConfiguratorAccess().get()->removeNode(ipAddress);

    delete msg;
}

void CANCtrl::onJoinReply(CANMessage* msg) {
    string content = msg->getContent();
    json joinInfo = json::parse(content);
    vector<int> area = joinInfo["area"];
    info.setArea(area);
    for (auto elem : joinInfo["neighbors"]) {
        unsigned long id = elem["id"];
        vector<int> area = elem["area"];
        CANProfile neighbor;
        neighbor.setId(id);
        neighbor.setArea(area);
        neighbors.push_back(neighbor);
    }

    GlobalNodeListAccess().get()->updateCANProfile(info);
    GlobalNodeListAccess().get()->addZone(info);
    GlobalNodeListAccess().get()->ready(info.getId());
    startMaint();

    string label = msg->getLabel();
    if (label.find(msg::LABEL_INIT) != string::npos) {
        UnderlayConfiguratorAccess().get()->initCANAdded();
    }

    cout << simTime() << " node join: " << info << endl;

    delete msg;
}

void CANCtrl::onNeighborAdd(CANMessage* msg) {
    string content = msg->getContent();
    json newNode = json::parse(content);
    CANProfile profile(newNode["neighborId"], newNode["neighborArea"]);
    CANProfile neighbor(newNode["id"], newNode["area"]);
    if (profile == info) {
        neighbors.push_back(neighbor);
    } else if (neighborsOftakeovers.count(profile) > 0) {
        neighborsOftakeovers[profile].push_back(neighbor);
    }

    delete msg;
}

void CANCtrl::onNeighborRemove(CANMessage* msg) {
    string content = msg->getContent();
    json toRemove = json::parse(content);
    CANProfile profile(toRemove["neighborId"], toRemove["neighborArea"]);
    CANProfile neighbor(toRemove["id"], toRemove["area"]);
    if (profile == info) {
        neighbors.erase(
                std::remove(neighbors.begin(), neighbors.end(), neighbor));
    } else if (neighborsOftakeovers.count(profile) > 0) {
        neighborsOftakeovers[profile].erase(
                std::remove(neighborsOftakeovers[profile].begin(),
                        neighborsOftakeovers[profile].end(), neighbor));
    }

    delete msg;
}

void CANCtrl::onAreaUpdate(CANMessage* msg) {
    string content = msg->getContent();
    json update = json::parse(content);
    CANProfile profile(update["neighborId"], update["neighborArea"]);
    CANProfile oldProfile(update["id"], update["oldArea"]);
    CANProfile newProfile(update["id"], update["newArea"]);

    if (profile == info) {
        std::replace(neighbors.begin(), neighbors.end(), oldProfile,
                newProfile);
    } else if (neighborsOftakeovers.count(profile) > 0) {
        std::replace(neighborsOftakeovers[profile].begin(),
                neighborsOftakeovers[profile].end(), oldProfile, newProfile);
    }
    delete msg;
}

void CANCtrl::onFinal(CANMessage* msg) {
//    GlobalStatisticsAccess().get()->addHop(msg->getHop());
    GlobalStatisticsAccess().get()->SUCCESS++;
    delete msg;
}

void CANCtrl::onNeighborUpdate(CANMessage* msg) {
    string label = msg->getLabel();
    string content = msg->getContent();
    json update = json::parse(content);
    unsigned long oldId = update["oldId"];
    unsigned long newId = update["newId"];
    vector<int> area = update["area"];
    CANProfile oldNeighbor(oldId, area);
    CANProfile newNeighbor(newId, area);
    CANProfile profile(update["neighborId"], update["neighborArea"]);

    if (profile == info) {
        if (std::find(neighbors.begin(), neighbors.end(), oldNeighbor)
                != neighbors.end()) {
            std::replace(neighbors.begin(), neighbors.end(), oldNeighbor,
                    newNeighbor);
        } else {
            neighbors.push_back(newNeighbor);
        }
    } else if (neighborsOftakeovers.count(profile) > 0) {
        if (std::find(neighborsOftakeovers[profile].begin(),
                neighborsOftakeovers[profile].end(), oldNeighbor)
                != neighborsOftakeovers[profile].end()) {
            std::replace(neighborsOftakeovers[profile].begin(),
                    neighborsOftakeovers[profile].end(), oldNeighbor,
                    newNeighbor);
        } else {
            neighborsOftakeovers[profile].push_back(newNeighbor);
        }
    }

    delete msg;
}

void CANCtrl::onNeighborTakeover(CANMessage* msg) {
    string label = msg->getLabel();
    string content = msg->getContent();
    json update = json::parse(content);
    unsigned long oldId = update["oldId"];
    unsigned long newId = update["newId"];
    vector<int> area = update["area"];
    CANProfile newNeighbor(newId, area);
    CANProfile profile(update["neighborId"], update["neighborArea"]);

    // conflict handling in zone takeover
//    cout << "[" << profile << "] received takeover event from " << newNeighbor
//            << " of " << CANProfile(oldId, area) << endl;

    // if already in takeover
    CANProfile takeover(info.getId(), area);
//        if (std::find(takeovers.begin(), takeovers.end(), takeover)
//                != takeovers.end()) {

    CANProfile oldNeighbor;
    for (auto elem : takeovers) {
        if (elem.getArea() == area) {
            if (elem.getId() != oldId && elem.getId() != newId) {

//                cout << "[" << profile << "] the zone ["
//                        << CANProfile(oldId, area)
//                        << "] has already been taken over by the host" << endl;

                // the zone has already been taken over by the host node
                if (newId < info.getId()) {

//                    cout << "[" << profile << "] the zone ["
//                            << CANProfile(oldId, area)
//                            << "] takeover cannot be overriden by "
//                            << newNeighbor << endl;

                    delete msg;
                    return;
                } else {
                    // no longer take over the zone
                    takeovers.erase(
                            std::remove(takeovers.begin(), takeovers.end(),
                                    takeover));
                    GlobalNodeListAccess().get()->delZone(takeover);

                    break;
                }
            }

            // for neighbor list update
            oldNeighbor = elem;
            break;
        }
    }

    // if already in neighbors or neighbors of takeover
    vector<CANProfile> neighborsInCharge;
    if (profile == info) {
        neighborsInCharge = neighbors;
    } else if (neighborsOftakeovers.count(profile) > 0) {
        neighborsInCharge = neighborsOftakeovers[profile];
    } else {
        delete msg;
        return;
    }
    for (auto elem : neighborsInCharge) {
        if (elem.getArea() == area) {
            if (elem.getId() != oldId && elem.getArea() == area
                    && newId <= elem.getId()) {
                // the zone has already been taken over by a neighbor

//                cout << "[" << profile << "] the zone ["
//                        << CANProfile(oldId, area)
//                        << "] has already been taken over by neighbor " << elem
//                        << " and cannot be overridden by " << newNeighbor
//                        << endl;

                delete msg;
                return;
            } else {
                // for changing the takeover in the neighbor
                if (profile == info) {
                    neighbors.erase(
                            std::remove(neighbors.begin(), neighbors.end(),
                                    elem));
                } else {
                    neighborsOftakeovers[profile].erase(
                            std::remove(neighborsOftakeovers[profile].begin(),
                                    neighborsOftakeovers[profile].end(), elem));
                }
            }

            // for neighbor list update
            oldNeighbor = elem;
            break;
        }
    }

    // update the neighbor list
    if (profile == info) {
        if (std::find(neighbors.begin(), neighbors.end(), oldNeighbor)
                != neighbors.end()) {

            cout << simTime() << " [" << info << "] update neighbor" << endl;

            std::replace(neighbors.begin(), neighbors.end(), oldNeighbor,
                    newNeighbor);
        } else {

            cout << simTime() << " [" << info << "] add new neighbor" << endl;
            cout << simTime() << " [" << info << "] old neighbor: "
                    << oldNeighbor << " new neighbor: " << newNeighbor << endl;

            neighbors.push_back(newNeighbor);
        }
    } else if (neighborsOftakeovers.count(profile) > 0) {
        if (std::find(neighborsOftakeovers[profile].begin(),
                neighborsOftakeovers[profile].end(), oldNeighbor)
                != neighborsOftakeovers[profile].end()) {
            std::replace(neighborsOftakeovers[profile].begin(),
                    neighborsOftakeovers[profile].end(), oldNeighbor,
                    newNeighbor);
        } else {
            neighborsOftakeovers[profile].push_back(newNeighbor);
        }
    }

    delete msg;
}

void CANCtrl::onNeighborExchange(CANMessage* msg) {
    string content = msg->getContent();
    json exchanges = json::parse(content);
    CANProfile neighbor(exchanges["id"], exchanges["area"]);
    CANProfile profile(exchanges["neighborId"], exchanges["neighborArea"]);

    // check whether the sender is in the neighbor list; if not, add it in
    if (profile == info) {
        if (std::find(neighbors.begin(), neighbors.end(), neighbor)
                == neighbors.end() && failed.count(neighbor) == 0
                && GlobalNodeListAccess().get()->validZone(neighbor)
                && commonEdge(profile.getArea(), neighbor.getArea())) {
            neighbors.push_back(neighbor);
        }
    } else if (std::find(takeovers.begin(), takeovers.end(), profile)
            != takeovers.end()) {
        if (std::find(neighborsOftakeovers[profile].begin(),
                neighborsOftakeovers[profile].end(), neighbor)
                == neighbors.end() && failed.count(neighbor) == 0
                && GlobalNodeListAccess().get()->validZone(neighbor)
                && commonEdge(profile.getArea(), neighbor.getArea())) {
            neighborsOftakeovers[profile].push_back(neighbor);
        }
    }

    // restore the neighbors in exchange
    vector<CANProfile> nnList;
    for (auto elem : exchanges["neighbors"]) {
        unsigned long nnId = elem["id"];
        vector<int> nnArea = elem["area"];
        CANProfile nn(nnId, nnArea);
        nnList.push_back(nn);
        // add the neighbor's neighbor into the neighbor list, if it share the common edge with the zone
        if (profile == info) {
            if (std::find(neighbors.begin(), neighbors.end(), nn)
                    == neighbors.end() && failed.count(nn) == 0
                    && GlobalNodeListAccess().get()->validZone(nn)
                    && commonEdge(profile.getArea(), nnArea)) {
                neighbors.push_back(nn);
            }
        } else if (std::find(takeovers.begin(), takeovers.end(), profile)
                != takeovers.end()) {
            if (std::find(neighborsOftakeovers[profile].begin(),
                    neighborsOftakeovers[profile].end(), nn) == neighbors.end()
                    && failed.count(nn) == 0
                    && GlobalNodeListAccess().get()->validZone(nn)
                    && commonEdge(profile.getArea(), nnArea)) {
                neighborsOftakeovers[profile].push_back(nn);
            }
        }
    }

    if (profile == info
            && std::find(neighbors.begin(), neighbors.end(), neighbor)
                    != neighbors.end()) {
        neighborsOfNeighbors[neighbor] = nnList;
    } else {
        for (auto elem : neighborsOftakeovers) {
            if (profile == elem.first
                    && std::find(elem.second.begin(), elem.second.end(),
                            neighbor) != elem.second.end()) {
                map<CANProfile, vector<CANProfile>> nnTakeover =
                        nnTakeovers[elem.first];
                nnTakeover[neighbor] = nnList;
                nnTakeovers[elem.first] = nnTakeover;
                break;
            }
        }
    }

    delete msg;
}

void CANCtrl::onNeighborRequest(CANMessage* msg) {
    // context merge
    string content = msg->getContent();
    json target = json::parse(content);
    CANProfile info(target["neighborId"], target["neighborArea"]);
    vector<CANProfile> neighbors;
    if (info == this->info) {
        neighbors = this->neighbors;
    } else if (std::find(takeovers.begin(), takeovers.end(), info)
            != takeovers.end()) {
        neighbors = neighborsOftakeovers[info];
    } else {
        delete msg;
        return;
    }

    int ttl = msg->getHop() - 1;
    if (ttl > 0) {
        for (auto elem : neighbors) {
            CANMessage* ringSearch = new CANMessage(msg::CAN_ERS);
            ringSearch->setSender(msg->getSender());
            ringSearch->setType(CANMsgType::ERS);
            ringSearch->setLabel(msg->getLabel());
            json neighbor;
            neighbor["neighborId"] = elem.getId();
            neighbor["neighborArea"] = elem.getArea();
            ringSearch->setContent(neighbor.dump().c_str());
            // i.e., the TTL value
            ringSearch->setHop(ttl);
            r_transmit(ringSearch, elem.getId());

            cout << simTime() << " [" << info << "] forward ERS for "
                    << msg->getLabel() << " and TTL = " << ttl << endl;
        }
    } else {
        json exchange;
        exchange["id"] = info.getId();
        exchange["area"] = info.getArea();
        for (auto elem : neighbors) {
            json neighbor;
            neighbor["id"] = elem.getId();
            neighbor["area"] = elem.getArea();
            exchange["neighbors"].push_back(neighbor);
        }
        CANMessage* reply = new CANMessage(msg::CAN_ERS_REPLY);
        reply->setType(CANMsgType::ERS_REPLY);
        reply->setLabel(msg->getLabel());
        reply->setContent(exchange.dump().c_str());
        reply->setSender(info.getId());
        r_transmit(reply, msg->getSender());

        cout << simTime() << " [" << info << "] reply ERS for "
                << msg->getLabel() << endl;
    }

    delete msg;
}

void CANCtrl::onNeighborReply(CANMessage* msg) {
    // context merge
    string label = msg->getLabel();
    json center = json::parse(label);
    CANProfile info(center["id"], center["area"]);
    vector<CANProfile> neighbors;
    if (info == this->info) {
        neighbors = this->neighbors;
    } else if (std::find(takeovers.begin(), takeovers.end(), info)
            != takeovers.end()) {
        neighbors = neighborsOftakeovers[info];
    } else {
        delete msg;
        return;
    }

    // calculate the update
    string content = msg->getContent();
    json reply = json::parse(content);
    for (auto elem : reply["neighbors"]) {
        CANProfile neighbor(elem["id"], elem["area"]);
        if (neighbor != info
                && std::find(neighbors.begin(), neighbors.end(), neighbor)
                        == neighbors.end() && failed.count(neighbor) == 0
                && commonEdge(info.getArea(), neighbor.getArea())) {
            neighbors.push_back(neighbor);

            cout << simTime() << " [" << info << "] find new neighbor "
                    << neighbor << " through ERS" << endl;
        }
    }

    // do the actual update in consistent with the context
    if (info == this->info) {
        this->neighbors = neighbors;
    } else {
        neighborsOftakeovers[info] = neighbors;
    }
    delete msg;
}

void CANCtrl::onReplicate(CANMessage* msg) {
    string content = msg->getContent();
    json request = json::parse(content);
    position p;
    p.x = request["x"];
    p.y = request["y"];
    string value = request["value"];
    string type = request["type"];
    if (type == data::DATA_TYPE_INVENTORY) {
        inventories[p] = value;
    } else if (type == data::DATA_TYPE_OBJECT) {
        data[p] = value;
    }

    delete msg;
}

void CANCtrl::onFixReplicas(CANMessage* msg) {
    string content = msg->getContent();
    json entries = json::parse(content);

    for (auto elem : entries["data"]) {
        position p;
        p.x = elem["x"];
        p.y = elem["y"];
        string value = elem["value"];
        string type = elem["type"];
        if (type == data::DATA_TYPE_INVENTORY) {
            inventories[p] = value;
        } else if (type == data::DATA_TYPE_OBJECT) {
            data[p] = value;
        }
    }

    delete msg;
}

void CANCtrl::onFixLoad(CANMessage* msg) {
    // merge context
    vector<CANProfile> zoneInCharge = takeovers;
    zoneInCharge.push_back(info);

    string content = msg->getContent();
    json entries = json::parse(content);
    for (auto elem : entries["data"]) {
        position p;
        p.x = elem["x"];
        p.y = elem["y"];
        string value = elem["value"];
        for (auto elem2 : zoneInCharge) {
            if (includedInArea(p, elem2.getArea())) {
                string type = elem["type"];
                if (type == data::DATA_TYPE_INVENTORY) {
                    inventories[p] = value;
                } else if (type == data::DATA_TYPE_OBJECT) {
                    data[p] = value;
                }
                break;
            }
        }
    }

    delete msg;
}

void CANCtrl::onMappingReply(ChordMessage* msg) {
    string content = msg->getContent();
    if (content.find(data::DATA_EMPTY) == string::npos) {
        json object = json::parse(content);
        position p;
        p.x = object["oCoord"]["x"];
        p.y = object["oCoord"]["y"];
        data[p] = object.dump();

//        cout << simTime() << " " << info.getId()
//                << " received data mapping update: " << content << endl;
    }

    delete msg;
}

void CANCtrl::onMappingQuery(CANMessage* msg) {
    load++;

    // merge context
    vector<CANProfile> zones = takeovers;
    zones.push_back(info);
    map<CANProfile, vector<CANProfile>> neighborsInCharge = neighborsOftakeovers;
    neighborsInCharge[info] = neighbors;

    string content = msg->getContent();
    json request = json::parse(content);
    CANProfile profile(request["id"], request["area"]);

    bool find = false;
    if (neighborsInCharge.count(profile) > 0) {
        find = replyContent(msg, profile, neighborsInCharge[profile]);
    }

    if (!find) {
        handleFailure(msg, info);
    }

    delete msg;
}

void CANCtrl::lookup(unsigned long nodeToAsk, string content, int type,
        string label) {
    CANMessage* predmsg = new CANMessage(msg::CAN_LOOK_UP);
    predmsg->setType(type);
    predmsg->setHop(0);
    predmsg->setContent(content.c_str());
    predmsg->setLabel(label.c_str());
    predmsg->setSender(info.getId());
    r_transmit(predmsg, nodeToAsk);

    cout << simTime() << " ["
            << GlobalNodeListAccess().get()->getCANProfile(nodeToAsk)
            << "] look up: " << content << endl;
}

void CANCtrl::join(unsigned long bootstrap, string label) {
    // add the macro for context switch
    Enter_Method_Silent();

    unsigned long length = GlobalParametersAccess().get()->getCANAreaSize();
    unsigned long x = uniform(0, length + 1);
    unsigned long y = uniform(0, length + 1);
//    string xy = to_string(x) + " " + to_string(y);
    json content;
    content["x"] = x;
    content["y"] = y;

    cout << simTime() << " " << info << " node join " << content << " from bootstrap: " << bootstrap << endl;

    lookup(bootstrap, content.dump(), CANMsgType::JOIN, label);
}

void CANCtrl::startMaint() {
    birth = simTime();
    scheduleAt(simTime() + maintain_cycle, maintainer);
}

void CANCtrl::maintain(cMessage *msg) {
    // exchange neighbors of neighbors
    exchangeNeighbors();
    // fix neighbors
    fixNeighbors();
    // expanding ring search
    expandRingSearch();
    // fix data replication
    fixReplicas();
    // fix data hosting
    fixLoad();
    // update data-to-addressing-bot mapping
    updateMapping();

    scheduleAt(simTime() + maintain_cycle, maintainer);
}

void CANCtrl::expandRingSearch() {
    int threshold = GlobalParametersAccess().get()->getERSThreshold();

    // context merge
    map<CANProfile, vector<CANProfile>> neighborsInCharge = neighborsOftakeovers;
    neighborsInCharge[info] = neighbors;

    // for takeover neighbors
    for (auto elem2 : neighborsInCharge) {
        CANProfile info = elem2.first;
        vector<CANProfile> neighbors = elem2.second;
        double survive = 0;
        for (auto elem : elem2.second) {
            if (GlobalNodeListAccess().get()->isUp(elem.getId())) {
                survive++;
            }
        }
        if (survive < (double) neighbors.size() / 2 || neighbors.size() <= 2) {
            int ttl;
            if (TTLs.count(info) > 0 && TTLs[info] < threshold) {
                ttl = TTLs[info] + 1;
            } else {
                ttl = 1;
            }
            TTLs[info] = ttl;

            cout << simTime() << " [" << elem2.first
                    << "] less than half neighbors are still available, start an ERS with TTL = "
                    << ttl << endl;
            cout << simTime() << " [" << elem2.first << "] survive: " << survive
                    << " neighbor size: " << neighbors.size() << endl;
            cout << simTime() << "[" << elem2.first << "] primary zone ID "
                    << this->info << endl;
            cout << simTime() << " [" << elem2.first << "] module "
                    << this->fullName << endl;
            cout << simTime() << " [" << elem2.first << "] ipAddress "
                    << ipAddress.get4().str() << endl;

            json request;
            request["id"] = info.getId();
            request["area"] = info.getArea();
            for (auto elem : neighbors) {
                if (GlobalNodeListAccess().get()->isUp(elem.getId())) {
                    CANMessage* ringSearch = new CANMessage(msg::CAN_ERS);
                    ringSearch->setSender(info.getId());
                    ringSearch->setType(CANMsgType::ERS);
                    ringSearch->setLabel(request.dump().c_str());
                    json neighbor;
                    neighbor["neighborId"] = elem.getId();
                    neighbor["neighborArea"] = elem.getArea();
                    ringSearch->setContent(neighbor.dump().c_str());
                    // i.e., the TTL value
                    ringSearch->setHop(ttl);
                    r_transmit(ringSearch, elem.getId());
                }
            }
        }
    }
}

void CANCtrl::exchangeNeighbors() {
    // context merge
    map<CANProfile, vector<CANProfile>> neighborsInCharge = neighborsOftakeovers;
    neighborsInCharge[info] = this->neighbors;

    json exchange;
    for (auto elem : neighborsInCharge) {
        exchange.clear();
        exchange["id"] = elem.first.getId();
        exchange["area"] = elem.first.getArea();
        for (auto elem2 : elem.second) {
            json neighbor;
            neighbor["id"] = elem2.getId();
            neighbor["area"] = elem2.getArea();
            exchange["neighbors"].push_back(neighbor);
        }
        for (auto elem2 : elem.second) {
            CANMessage* XNeighbor = new CANMessage(msg::CAN_NEIGHBOR_EXCHANGE);
            XNeighbor->setType(CANMsgType::NEIGHBOR_EXCHANGE);
            XNeighbor->setSender(elem.first.getId());
            exchange["neighborId"] = elem2.getId();
            exchange["neighborArea"] = elem2.getArea();
            XNeighbor->setContent(exchange.dump().c_str());
            r_transmit(XNeighbor, elem2.getId());
        }
    }
}

void CANCtrl::fixNeighbors() {
    // context merge
    map<CANProfile, vector<CANProfile>> neighborsInCharge = neighborsOftakeovers;
    neighborsInCharge[info] = this->neighbors;
    map<CANProfile, map<CANProfile, vector<CANProfile>>> nnInCharge =
            nnTakeovers;
    nnInCharge[info] = neighborsOfNeighbors;

    map<CANProfile, vector<CANProfile>> temp_neighborsOftakeovers;
    // elem: CANProfile of the zone in charge
    for (auto elem : neighborsInCharge) {
        for (auto elem3 : elem.second) {    // for each neighbor of the takeover
            if (!GlobalNodeListAccess().get()->isUp(elem3.getId())) {
                cout << simTime() << " [" << elem.first << "] neighbor "
                        << elem3 << " failed" << endl;
                vector<CANProfile> profiles = nnInCharge[elem.first][elem3];

                /*
                 *  check neighbor's neighbors for takeover:
                 *  1) if there is a live neighbor's neighbor with higher ID, then give up the takeover;
                 *  2) if there is no neighbor's neighbor of the failed neighbor, then give up the takeover.
                 *
                 *  TODO These conditions should be reviewed in future for improvement.
                 */
                bool toTakeover = true;
                // no neighbor's neighbor, no takeover, and remove the failed neighbor
                if (profiles.size() == 0) {

                    cout << simTime() << " [" << elem.first
                            << "] not take over neighbor " << elem3
                            << " due to empty nn" << endl;

                    toTakeover = false;
                } else {
                    for (auto elem2 : profiles) {
                        if ((GlobalNodeListAccess().get()->isUp(elem2.getId())
                                && elem2.getId() > elem.first.getId())
                                || failed.count(elem3) > 0) {

                            cout << simTime() << " [" << elem.first
                                    << "] not take over neighbor " << elem3
                                    << ", because " << elem2.getId()
                                    << " has greater ID than "
                                    << elem.first.getId() << endl;

                            toTakeover = false;
                            break;
                        }
                    }
                }

                failed.insert(elem3);
                failedHosts.insert(elem3);

                CANProfile takeover(elem.first.getId(), elem3.getArea());
                if (toTakeover
                        && std::find(takeovers.begin(), takeovers.end(),
                                takeover) == takeovers.end()) {
                    /*
                     * immediately take over the zone of the neighbor;
                     */
                    cout << simTime() << " [" << elem.first
                            << "] take over failed neighbor " << elem3 << endl;

                    // update list of areas in charge
                    takeovers.push_back(takeover);
                    CANProfile oldNeighbor = elem3;
                    CANProfile newNeighbor = takeover;
                    GlobalNodeListAccess().get()->addZone(takeover);

                    // notify neighbors of the neighbor of the takeover
                    vector<CANProfile> nt;
                    for (auto elem2 : profiles) {

                        //                    cout << "[" << elem.first << "] notify " << elem2
                        //                            << " of takeover of " << elem3 << endl;

                        nt.push_back(elem2);
                        CANMessage* notice = new CANMessage(
                                msg::CAN_NEIGHBOR_TAKEOVER);
                        notice->setType(CANMsgType::NEIGHBOR_TAKEOVER);
                        notice->setSender(info.getId());
                        json update;
                        update["oldId"] = elem3.getId();
                        update["newId"] = elem.first.getId();
                        update["area"] = elem3.getArea();
                        update["neighborId"] = elem2.getId();
                        update["neighborArea"] = elem2.getArea();
                        notice->setContent(update.dump().c_str());
                        notice->setLabel("takeover");
                        r_transmit(notice, elem2.getId());
                    }
                    temp_neighborsOftakeovers[newNeighbor] = nt;

                    // switch context back
                    if (elem.first == info) {
                        std::replace(neighbors.begin(), neighbors.end(),
                                oldNeighbor, newNeighbor);
                    } else {
                        std::replace(neighborsOftakeovers[elem.first].begin(),
                                neighborsOftakeovers[elem.first].end(),
                                oldNeighbor, newNeighbor);
                    }
                } else {
                    // remove failed neighbors
                    if (elem.first == info) {
                        neighbors.erase(
                                std::remove(neighbors.begin(), neighbors.end(),
                                        elem3));
                    } else {
                        neighborsOftakeovers[elem.first].erase(
                                std::remove(
                                        neighborsOftakeovers[elem.first].begin(),
                                        neighborsOftakeovers[elem.first].end(),
                                        elem3));
                    }
                }
            }
        }
    }
    neighborsOftakeovers.insert(temp_neighborsOftakeovers.begin(),
            temp_neighborsOftakeovers.end());
}

void CANCtrl::fixReplicas() {
    // merge context
    map<CANProfile, vector<CANProfile>> neighborsInCharge = neighborsOftakeovers;
    neighborsInCharge[info] = neighbors;

    for (auto elem1 : neighborsInCharge) {
        CANProfile info = elem1.first;
        vector<CANProfile> neighbors = elem1.second;
        json entries;
        for (auto elem : data) {
            position p = elem.first;
            if (includedInArea(p, info.getArea())) {
                string value = elem.second;
                json entry;
                entry["x"] = p.x;
                entry["y"] = p.y;
                entry["type"] = data::DATA_TYPE_OBJECT;
                entry["value"] = value;
                entries["data"].push_back(entry);
            }
        }
        for (auto elem : inventories) {
            position p = elem.first;
            if (includedInArea(p, info.getArea())) {
                string value = elem.second;
                json entry;
                entry["x"] = p.x;
                entry["y"] = p.y;
                entry["type"] = data::DATA_TYPE_INVENTORY;
                entry["value"] = value;
                entries["data"].push_back(entry);
            }
        }

        // replicate value to neighbors
        if (entries.count("data") > 0) {
            for (int i = 0; i < neighbors.size(); i++) {
                CANMessage* replicate = new CANMessage(msg::CAN_FIX_REPLICA);
                replicate->setSender(info.getId());
                replicate->setType(CANMsgType::CAN_FIX_REPLICA);
                replicate->setContent(entries.dump().c_str());
                r_transmit(replicate, neighbors[i].getId());
            }

//            cout << fullName << " [" << info << "] fix replicas" << endl;
        }
    }
}

void CANCtrl::fixLoad() {
    // merge context
    map<CANProfile, vector<CANProfile>> neighborsInCharge = neighborsOftakeovers;
    neighborsInCharge[info] = neighbors;

    for (auto elem1 : neighborsInCharge) {
        CANProfile info = elem1.first;
        vector<CANProfile> neighbors = elem1.second;
        for (auto elem2 : neighbors) {
            CANProfile neighbor = elem2;
            json entries;
            for (auto elem : data) {
                position p = elem.first;
                if (includedInArea(p, neighbor.getArea())) {
                    string value = elem.second;
                    json entry;
                    entry["x"] = p.x;
                    entry["y"] = p.y;
                    entry["type"] = data::DATA_TYPE_OBJECT;
                    entry["value"] = value;
                    entries["data"].push_back(entry);
                }
            }
            for (auto elem : inventories) {
                position p = elem.first;
                if (includedInArea(p, neighbor.getArea())) {
                    string value = elem.second;
                    json entry;
                    entry["x"] = p.x;
                    entry["y"] = p.y;
                    entry["type"] = data::DATA_TYPE_INVENTORY;
                    entry["value"] = value;
                    entries["data"].push_back(entry);
                }
            }

            // replicate value to the host
            if (entries.count("data") > 0) {
                CANMessage* replicate = new CANMessage(msg::CAN_FIX_LOAD);
                replicate->setSender(info.getId());
                replicate->setType(CANMsgType::FIX_LOAD);
                replicate->setContent(entries.dump().c_str());
                r_transmit(replicate, neighbor.getId());

//                cout << fullName << " [" << info << "] fix load" << endl;
            }
        }
    }
}

void CANCtrl::updateMapping() {
    // merge context
    vector<CANProfile> zoneIncharge = takeovers;
    zoneIncharge.push_back(info);

    ChordInfo* sender = NULL;
    do {
        sender = GlobalNodeListAccess().get()->randChord();
    } while (sender == NULL
            || !GlobalNodeListAccess().get()->isReady(sender->getChordId()));

    for (auto elem1 : zoneIncharge) {
        CANProfile info = elem1;
        for (auto elem : data) {
            json object = json::parse(elem.second);
            position p(object["oCoord"]["x"], object["oCoord"]["y"]);
            // for objects managed within the zone
            if (includedInArea(p, info.getArea())) {

                ChordMessage* message = new ChordMessage(msg::CHORD_LOOK_UP);
                message->setSender(info.getId());
                message->setHop(0);
                message->setType(ChordMsgType::CHORD_GET);
                message->setLabel("query");
                json content;
                content["id"] = object["oid"];
                content["x"] = p.x;
                content["y"] = p.y;
                message->setContent(content.dump().c_str());
                IPvXAddress destAddr =
                        GlobalNodeListAccess().get()->getNodeAddr(
                                sender->getChordId());
                UDPControlInfo* udpControlInfo = new UDPControlInfo();
                udpControlInfo->setDestAddr(destAddr);
                message->setControlInfo(udpControlInfo);
                SimpleNodeEntry* destEntry = sender->getEntry();
                cSimpleModule::sendDirect(message, 0, 0,
                        destEntry->getTcpIPv4Gate());

//                cout << simTime() << " " << info << " update mapping for data "
//                        << elem.second << endl;
            }
        }
    }
}

bool CANCtrl::operator<(const CANCtrl& ctrl) const {
    return info < ctrl.info;
}

void CANCtrl::initArea() {
    info.setArea(0, GlobalParametersAccess().get()->getCANAreaSize(), 0,
            GlobalParametersAccess().get()->getCANAreaSize());
}

bool CANCtrl::replyInventory(CANMessage* msg, position p) {
    if (inventories.count(p) > 0) {
        CANMessage* inventory = new CANMessage(msg::CAN_INVENTORY_REPLY);
        inventory->setContent(inventories[p].c_str());
        inventory->setType(CANMsgType::CAN_REPLY);
        inventory->setSender(info.getId());
        inventory->setHop(msg->getHop() + 1);
        r_transmit(inventory, msg->getSender());
        return true;
    }

    return false;
}

bool CANCtrl::replyContent(CANMessage* msg, CANProfile info,
        vector<CANProfile> neighbors) {
    string content = msg->getContent();
    json request = json::parse(content);
    position center(request["center"]["x"], request["center"]["y"]);
    int radius = request["radius"];

    json reply;
    if (request.count("id") > 0) {
        reply["id"] = request["id"];
        reply["area"] = request["area"];
    } else {
        reply["id"] = info.getId();
        reply["area"] = info.getArea();
    }

    Coordinate c(center.x, center.y);
    for (auto elem : data) {
        Coordinate posit(elem.first.x, elem.first.y);
        Coordinate c(center.x, center.y);
        int dist = distance(posit, c);
        // exclude data replication which are not hosted at the zone
        if (dist <= radius && includedInArea(elem.first, info.getArea())) {

//            cout << simTime() << " " << info << " data found " << posit << endl;

            json object = json::parse(elem.second);
            reply["objects"].push_back(object);
        }
    }

    json grid;
    for (auto elem : neighbors) {
        if (intercept(elem.getArea(), center, radius)) {
            grid["id"] = elem.getId();
            grid["area"] = elem.getArea();
            reply["grids"].push_back(grid);
        }
    }

    CANMessage* replyMsg = new CANMessage(msg::CAN_CONTENT_REPLY);
    replyMsg->setContent(reply.dump().c_str());
    replyMsg->setType(CANMsgType::CAN_REPLY);
    replyMsg->setSender(info.getId());
    replyMsg->setHop(msg->getHop() + 1);
    r_transmit(replyMsg, msg->getSender());
    return true;
}

void CANCtrl::handleFailure(CANMessage* msg, CANProfile info) {
    if (msg->getType() == CANMsgType::CAN_GET) {
        string content = msg->getContent();
        json request = json::parse(content);
        json reply;
        if (request.count("id") > 0) {
            reply["id"] = request["id"];
            reply["area"] = request["area"];
        } else {
            reply["id"] = info.getId();
            reply["area"] = info.getArea();
        }

        string replyMsgName;
        int replyMsgType;
        string label = msg->getLabel();
        if (label.find(msg::LABEL_INVENTORY) != string::npos) {
            replyMsgName = msg::CAN_INVENTORY_REPLY;
        } else if (label.find(msg::LABEL_CONTENT) != string::npos) {
            replyMsgName = msg::CAN_CONTENT_REPLY;
        }
        replyMsgType = CANMsgType::CAN_REPLY;
        CANMessage* replyMsg = new CANMessage(replyMsgName.c_str());
        replyMsg->setContent(reply.dump().c_str());
        replyMsg->setType(replyMsgType);
        replyMsg->setSender(info.getId());
        replyMsg->setHop(msg->getHop() + 1);
        r_transmit(replyMsg, msg->getSender());
    }
}

CANProfile CANCtrl::routeToNext(position id, CANProfile info,
        vector<CANProfile> neighbors) {
    CANProfile next;
    // default next hop;
    if (neighbors.size() > 0) {
        next = neighbors[neighbors.size() - 1];
    }

    if (includedInArea(id, info.getArea()))
        return info;

    for (int i = 0; i < neighbors.size(); i++) {
        if (includedInArea(id, neighbors[i].getArea())) {
            return neighbors[i];
        }
    }

    int length = GlobalParametersAccess().get()->getCANAreaSize();

    if (includeXValue(id, info.getArea())) { // include the x value
        if ((getYValue(id) - info.getArea()[2] <= length / 2
                && getYValue(id) > info.getArea()[3])   // upper neighbor
        || info.getArea()[2] - getYValue(id) > length / 2) { // cross-border search condition
            for (int i = 0; i < neighbors.size(); i++) {
                if (GlobalNodeListAccess().get()->isUp(next.getId())) {
                    next = neighbors[i];
                    if ((getYValue(id) > info.getArea()[3]
                            && getYValue(id) >= next.getArea()[2]  // not exceed
                            && info.getArea()[3] <= next.getArea()[2]) // correct direction
                            || (info.getArea()[2] - getYValue(id) > length / 2
                                    && (info.getArea()[3] <= next.getArea()[2]
                                            || (info.getArea()[3] == length
                                                    && next.getArea()[2] == 0)))) { // cross-border search
                        return next;
                    }
                }
            }
        } else if ((info.getArea()[2] - getYValue(id) <= length / 2
                && getYValue(id) <= info.getArea()[2])   // lower neighbor
        || getYValue(id) - info.getArea()[2] > length / 2) { // cross-border search condition
            for (int i = 0; i < neighbors.size(); i++) {
                if (GlobalNodeListAccess().get()->isUp(next.getId())) {
                    next = neighbors[i];
                    if ((getYValue(id) <= info.getArea()[2]
                            && getYValue(id) <= next.getArea()[3]  // not exceed
                            && info.getArea()[2] >= next.getArea()[3]) // correct direction
                            || (getYValue(id) - info.getArea()[2] > length / 2
                                    && (info.getArea()[2] >= next.getArea()[3]
                                            || (info.getArea()[2] == 0
                                                    && next.getArea()[3]
                                                            == length)))) { // cross-border search
                        return next;
                    }
                }
            }
        }
    } else if ((getXValue(id) - info.getArea()[0] <= length / 2
            && getXValue(id) >= info.getArea()[1]) // search X first and right neighbor
    || info.getArea()[0] - getXValue(id) > length / 2) { // cross-border search condition
        for (int i = 0; i < neighbors.size(); i++) {
            if (GlobalNodeListAccess().get()->isUp(next.getId())) {
                next = neighbors[i];
                if ((getXValue(id) >= info.getArea()[1]
                        && getXValue(id) > next.getArea()[0]   // not exceed
                        && info.getArea()[1] <= next.getArea()[0]) // correct direction
                        || (info.getArea()[0] - getXValue(id) > length / 2
                                && (info.getArea()[1] <= next.getArea()[0]
                                        || (info.getArea()[1] == length
                                                && next.getArea()[0] == 0)))) { // cross-border search
                    return next;
                }
            }
        }
    } else if ((info.getArea()[0] - getXValue(id) <= length / 2
            && getXValue(id) <= info.getArea()[0]) // search X first and left neighbor
    || getXValue(id) - info.getArea()[0] > length / 2) { // cross-border search condition
        for (int i = 0; i < neighbors.size(); i++) {
            if (GlobalNodeListAccess().get()->isUp(next.getId())) {
                next = neighbors[i];
                if ((getXValue(id) <= info.getArea()[0]
                        && getXValue(id) <= next.getArea()[1]  // not exceed
                        && info.getArea()[0] >= next.getArea()[1]) // correct direction
                        || (getXValue(id) - info.getArea()[0] > length / 2
                                && (info.getArea()[0] >= next.getArea()[1]
                                        || (info.getArea()[0] == 0
                                                && next.getArea()[1] == length)))) { // cross-border search
                    return next;
                }
            }
        }
    } else {
        int index = (int) uniform(0, neighbors.size());
        return neighbors[index];
    }

    return next;
}
