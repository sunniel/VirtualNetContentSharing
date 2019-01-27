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

#include "ClientDelayTest.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include "../common/Util.h"
#include "../global/UnderlayConfiguratorAccess.h"
#include "../global/GlobalStatisticsAccess.h"
#include "../global/GlobalNodeListAccess.h"
#include "../global/GlobalParametersAccess.h"
#include "../global/CoordinatorAccess.h"
#include "../objects/IPAddress.h"
#include "../objects/IPvXAddress.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/HostInfo.h"

Define_Module(ClientDelayTest);

ClientDelayTest::ClientDelayTest() {
    refreshTimer = new cMessage(msg::REFRESH_CONTENT);
    walkTimer = new cMessage(msg::REFRESH_POSITION);
    startTimer = new cMessage(msg::CLIENT_START);
    timeout = new cMessage(msg::TIMEOUT);
    circle = nullptr;
    p_circle = nullptr;
    coverage = 0;
    inventories = 0;
    steps = 0;
    hops = 0;
    triggered = false;
    loaded = false;
}

ClientDelayTest::~ClientDelayTest() {
    if (refreshTimer != NULL) {
        cancelAndDelete(refreshTimer);
    }
    if (walkTimer != NULL) {
        cancelAndDelete(walkTimer);
    }
    if (startTimer != NULL) {
        cancelAndDelete(startTimer);
    }
}

int ClientDelayTest::numInitStages() const {
    return 2;
}

void ClientDelayTest::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();
        loadCycle = par("load_cycle");
        walkCycle = par("walk_cycle");
        startCycle = par("start");
        moveDist = par("walk_distance");
        speed = par("speed");
        timeoutLength = par("load_timeout_length");

        // Let radius be the half of the region diagonal length
        radius = sqrt(2) / (double) 2 * CoordinatorAccess().get()->regionSize;
        perception = radius / 2;

        delaySignal = registerSignal("duration");

        WATCH(id);
    } else if (stage == 1) {
        IPvXAddress addr = IPAddress(
                UnderlayConfiguratorAccess().get()->freeAddress());
        setIPAddress(addr);
        id = util::getSHA1(ipAddress.get4().str() + "4000",
                GlobalParametersAccess().get()->getAddrSpaceSize());
        // create meta information
        SimpleNodeEntry* entry = new SimpleNodeEntry(this->getParentModule());
        HostInfo* info = new HostInfo(this->getParentModule()->getId(),
                this->getParentModule()->getFullName());
        info->setHostId(id);
        info->setEntry(entry);
        //add host to bootstrap oracle
        GlobalNodeListAccess().get()->addPeer(addr, info);
        // initialize start location
        origin = location = CoordinatorAccess().get()->centerLocation();
        displayPosition();

        scheduleAt(simTime() + startCycle, startTimer);
    }
}

bool ClientDelayTest::r_transmit(cMessage* msg, unsigned long destID) {
    if (GlobalNodeListAccess().get()->isUp(destID)) {
        // in case of message forwarding
        msg->removeControlInfo();
        IPvXAddress srcAddr = GlobalNodeListAccess().get()->getNodeAddr(id);
        IPvXAddress destAddr = GlobalNodeListAccess().get()->getNodeAddr(
                destID);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(destAddr);
        udpControlInfo->setSrcAddr(srcAddr);
        msg->setControlInfo(udpControlInfo);
        HostBase::r_transmit(msg, destAddr);
        return true;
    } else {

        cout << simTime() << " "
                << "[Client] transmission failed due to destination " << destID
                << " failure" << endl;

        return false;
    }
}

void ClientDelayTest::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::REFRESH_CONTENT)) {
        refreshContent(msg);
    } else if (msg->isName(msg::REFRESH_POSITION)) {
        move(msg);
    } else if (msg->isName(msg::CAN_INVENTORY_REPLY)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onInventory(canMsg);
    } else if (msg->isName(msg::CAN_CONTENT_REPLY)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onContent(canMsg);
    } else if (msg->isName(msg::CHORD_REPLY)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onDataReply(chordMsg);
    } else if (msg->isName(msg::CLIENT_START)) {
        startPlay(msg);
    } else if (msg->isName(msg::TIMEOUT)) {
        onDataRetrievalTimeout(msg);
    }
}

void ClientDelayTest::refreshContent(cMessage* msg) {
    clearCache();
    loadContent();

    scheduleAt(simTime() + loadCycle, refreshTimer);
    scheduleAt(simTime() + timeoutLength, timeout);
}

void ClientDelayTest::clearCache() {
    // distance condition for cache clear: 2 region diagonal length, approximate to 2 region length
    long M = radius * 4;
    for (map<Coordinate, string, CoordntCompare>::iterator it = history.begin();
            it != history.end();) {
        Coordinate c = it->first;
        long dist = distance(c, location);
        if (dist > M) {
            it = history.erase(it);
        } else {
            it++;
        }
    }
}

void ClientDelayTest::onDataRetrievalTimeout(cMessage* msg) {
    cancelEvent(refreshTimer);

    cout << "Content retrieval timeout, cancel the cycle" << endl;

    // prepare for the next content retrieval cycle
    triggered = false;
    data.clear();
    crc.clear();
    visited.clear();
    mappings.clear();
    cout << simTime() << " [Client] number of hop: " << hops << endl;
    GlobalStatisticsAccess().get()->addContentLoadHop(hops);
    hops = 0;
    end = simTime();
    cout << simTime() << " [Client] content load delay: " << (end - start)
            << endl;
    GlobalStatisticsAccess().get()->addContentLoadDelay(end - start);

    // time  the new content retrieval cycle
    simtime_t nextCycle = (int) (simTime().dbl() / 10) * 10 + loadCycle;

    cout << "current time: " << simTime() << endl;
    cout << "next cycle: " << nextCycle << endl;

    scheduleAt(nextCycle, refreshTimer);
}

void ClientDelayTest::loadContent() {
    if (!triggered) {
        triggered = true;
        start = simTime();

        // clear caches, in case that the last test has not terminated
        crc.clear();
        data.clear();
        mappings.clear();
        toVisit.clear();
        visited.clear();
        visiting.clear();
        hops = 0;
        origin = location;

        // generate cross-region content inventory
        CANInfo* sender = NULL;
        do {
            sender = GlobalNodeListAccess().get()->randCAN();
        } while (sender == NULL
                || !GlobalNodeListAccess().get()->isReady(sender->getId()));
        Coordinate region = CoordinatorAccess().get()->getRegion(origin);
        vector<Coordinate> regions = CoordinatorAccess().get()->neighborRegions(
                region);
        regions.push_back(region);

        cout << simTime() << " [Client] number of regions to query: "
                << regions.size() << endl;

        for (auto elem : regions) {
            CANMessage* message = new CANMessage(msg::CAN_LOOK_UP);
            message->setSender(id);
            message->setHop(1);
            message->setType(CANMsgType::CAN_GET);
            message->setLabel(msg::LABEL_INVENTORY);
            json content;
            content["x"] = elem.x;
            content["y"] = elem.y;
            message->setContent(content.dump().c_str());
            r_transmit(message, sender->getId());
        }
        coverage = regions.size();
    }
}

void ClientDelayTest::onInventory(CANMessage* msg) {
    string content = msg->getContent();
//    hops += msg->getHop();

//    cout << "hops for inventory: " << msg->getHop() << endl;

    if (content.find(data::DATA_EMPTY) == string::npos) {
        inventories++;
        json inventory = json::parse(content);
        for (auto elem : inventory["object"]) {
            // remove objects out of the perception range
            Coordinate coord(elem["oCoord"]["x"], elem["oCoord"]["y"]);
            long dist = distance(coord, origin);
            if (dist <= radius && history.count(coord) == 0) {
                crc[coord] = elem.dump();
            }
        }
    } else {
        coverage--;
    }

    if (inventories >= coverage) {
        coverage = 0;
        inventories = 0;

        cout << simTime() << " [Client] number of objects to query: "
                << crc.size() << endl;
//        for (auto elem : crc) {
//            cout << elem.first << ": " << elem.second << endl;
//        }

        CANInfo* sender = NULL;
        do {
            sender = GlobalNodeListAccess().get()->randCAN();
        } while (sender == NULL
                || !GlobalNodeListAccess().get()->isReady(sender->getId()));
        // start proximity-based content retrieval
        Coordinate grid = CoordinatorAccess().get()->getGrid(origin);

        cout << "start grid: " << grid << endl;

        CANMessage* message = new CANMessage(msg::CAN_LOOK_UP);
        message->setSender(id);
        message->setHop(1);
        message->setType(CANMsgType::CAN_GET);
        message->setLabel(msg::LABEL_CONTENT);
        json request;
        request["x"] = origin.x;
        request["y"] = origin.y;
        request["center"]["x"] = origin.x;
        request["center"]["y"] = origin.y;
        request["radius"] = radius;
        message->setContent(request.dump().c_str());
        r_transmit(message, sender->getId());
    }

    delete msg;
}

void ClientDelayTest::onContent(CANMessage* msg) {
    string content = msg->getContent();
    json reply = json::parse(content);
    hops += msg->getHop();
    CANProfile senderProfile(reply["id"], reply["area"]);
    visiting.erase(senderProfile);

//    cout << "received content: " << content << endl;
    cout << "hops for content: " << msg->getHop() << endl;

    if (content.find(data::DATA_EMPTY) == string::npos) {
        visited.insert(senderProfile);

        cout << "number of objects to query: " << reply["objects"].size()
                << " from " << senderProfile << endl;

        for (auto elem : reply["objects"]) {
            Coordinate c(elem["oCoord"]["x"], elem["oCoord"]["y"]);
            mappings[c] = elem.dump();
            unsigned long target = elem["mapping"];

//            cout << "received object mapping: " << target << endl;

            /*
             * retrieve the real data
             */
            long dist = distance(c, origin);
            if (dist <= radius && history.count(c) == 0) {
                if (toDownloads.empty()) {
                    ChordMessage* message = new ChordMessage(
                            msg::CHORD_LOOK_UP);
                    message->setSender(id);
                    message->setHop(1);
                    message->setType(ChordMsgType::CHORD_GET);
                    message->setLabel(msg::LABEL_CONTENT);
                    json content;
                    content["id"] = elem["oid"];
                    content["x"] = c.x;
                    content["y"] = c.y;
                    message->setContent(content.dump().c_str());
                    if (!r_transmit(message, target)) {
                        ChordInfo* chord = NULL;
                        do {
                            chord = GlobalNodeListAccess().get()->randChord();
                        } while (chord == NULL
                                || !GlobalNodeListAccess().get()->isReady(
                                        chord->getChordId()));
                        r_transmit(message, chord->getChordId());
                    }

                    // fake the distance to be zero so that it can be correctly removed after data receipt
                    util::multimap_insert(toDownloads, { 0, elem.dump() });
                } else {
                    long dist = distance(c, origin);
                    util::multimap_insert(toDownloads, { dist, elem.dump() });
                }
            }

        }

        // retrieve the candidate region bots for mapping query
        for (auto elem : reply["grids"]) {
            CANProfile host(elem["id"], elem["area"]);
            if (visited.count(host) == 0 && visiting.count(host) == 0) {

                cout << "received host: " << host << endl;

                toVisit.insert(host);
            }
        }

        // check whether all mappings on the CRC inventory have been retrieved
        bool allRetrieved = true;
        for (auto elem : crc) {
            if (mappings.count(elem.first) == 0) {
                allRetrieved = false;
                break;
            }
        }

        // query neighbor grids
        if (!allRetrieved) {
            for (set<CANProfile>::iterator it = toVisit.begin();
                    it != toVisit.end();) {
                CANProfile host = (*it);
                CANMessage* message = new CANMessage(msg::CAN_MAPPING_QUERY);
                message->setSender(id);
                message->setHop(1);
                message->setType(CANMsgType::CAN_GET);
                message->setLabel(msg::LABEL_CONTENT);
                json content;
                content["id"] = host.getId();
                content["area"] = host.getArea();
                content["center"]["x"] = origin.x;
                content["center"]["y"] = origin.y;
                content["radius"] = radius;
                message->setContent(content.dump().c_str());
                if (r_transmit(message, host.getId())) {
                    visiting.insert(host);
                    it = toVisit.erase(it);
                } else {
                    visited.insert(host);
                    it = toVisit.erase(it);
                    delete message;
                }
            }
        } else {

            cout << "All object mappings are found" << endl;

            toVisit.clear();
            visiting.clear();
        }
    }

    if (toVisit.empty() && visiting.empty()) {

        cout << simTime()
                << " [Client] number of mappings found in proximate-based retrieval: "
                << mappings.size() << endl;

        /*
         * start content retrieval based on inventory
         */
        // retrieve the data directly from the addressing bots, if mappings are not found
        ChordInfo* chord = NULL;
        do {
            chord = GlobalNodeListAccess().get()->randChord();
        } while (chord == NULL
                || !GlobalNodeListAccess().get()->isReady(chord->getChordId()));
        for (auto elem : crc) {
            if (mappings.count(elem.first) == 0) {

                cout << "object " << elem.second
                        << " not found through proximate-based search" << endl;

                if (toDownloads.empty()) {
                    json object = json::parse(elem.second);
                    ChordMessage* message = new ChordMessage(
                            msg::CHORD_LOOK_UP);
                    message->setSender(id);
                    message->setHop(1);
                    message->setType(ChordMsgType::CHORD_GET);
                    message->setLabel(msg::LABEL_CONTENT);
                    json content;
                    content["id"] = object["oid"];
                    content["x"] = object["oCoord"]["x"];
                    content["y"] = object["oCoord"]["y"];
                    message->setContent(content.dump().c_str());
                    r_transmit(message, chord->getChordId());

                    // fake the distance to be zero so that it can be correctly removed after data receipt
                    util::multimap_insert(toDownloads, { 0, elem.second });
                } else {
                    long dist = distance(Coordinate(elem.first.x, elem.first.y),
                            origin);
                    util::multimap_insert(toDownloads, { dist, elem.second });
                }
            }
        }
    }

    delete msg;
}

void ClientDelayTest::onDataReply(ChordMessage* msg) {
    if (!crc.empty()) {
        hops += msg->getHop();

        cout << "hops for data: " << msg->getHop() << endl;

        string content = msg->getContent();
        json reply = json::parse(content);
        if (content.find(data::DATA_EMPTY) == string::npos) {
            json object = json::parse(content);
            Coordinate c(object["oCoord"]["x"], object["oCoord"]["y"]);
            data[c] = object.dump();

            long dist = distance(c, origin);
            if (dist < perception) {
                cout << simTime() << " object at " << c
                        << " inside the perception range" << endl;
            } else {
                cout << simTime() << " object at " << c
                        << " outside the perception range" << endl;
            }

//            cout << simTime() << " [Client] received data: " << content << endl;
        } else {
            Coordinate c(reply["x"], reply["y"]);
            crc.erase(c);
        }

        toDownloads.erase(toDownloads.begin());

        // check whether all objects on the CRC inventory have been retrieved
        bool allRetrieved = true;
        set<Coordinate, CoordntCompare> perceive;
        for (auto elem : crc) {
            long dist = distance(elem.first, origin);
            if (dist <= perception) {
                perceive.insert(elem.first);
            }
            if (data.count(elem.first) == 0) {
                allRetrieved = false;
            }
        }

        // check whether all content within the perception range have been retrieved
        bool allPerceived = true;
        int count = 0;
        for (auto elem : perceive) {
            if (data.count(elem) == 0) {
                allPerceived = false;
                break;
            }
        }

        cout << simTime() << " [Client] accumulated object perception delay: "
                << (simTime() - start) << endl;

        if (allPerceived && !loaded) {
            end = simTime();
            cout << simTime() << " [Client] content perception delay: "
                    << (end - start) << endl;
            cout << simTime() << " [Client] number of perceived objects: "
                    << perceive.size() << endl;
            GlobalStatisticsAccess().get()->addContentLoadDelay(end - start);
            emit(delaySignal, (end - start));
            loaded = true;
        }

        if (allRetrieved) {

            cout << simTime() << " [Client] number of objects found: "
                    << data.size() << endl;
            cout << simTime() << " [Client] number of region bots visited: "
                    << visited.size() << endl;

            /*
             * conclude the run
             */
//            history.insert(data.begin(), data.end());
            cancelEvent(timeout);
            triggered = false;
            loaded = false;
            data.clear();
            crc.clear();
            visited.clear();
            mappings.clear();
            toDownloads.clear();
            cout << simTime() << " [Client] number of hop: " << hops << endl;
            GlobalStatisticsAccess().get()->addContentLoadHop(hops);
            hops = 0;
        } else {
            if (!toDownloads.empty()) {
                ChordInfo* chord = NULL;
                do {
                    chord = GlobalNodeListAccess().get()->randChord();
                } while (chord == NULL
                        || !GlobalNodeListAccess().get()->isReady(
                                chord->getChordId()));

                string toDownload = toDownloads.begin()->second;
                json object = json::parse(toDownload);
                unsigned long target = chord->getChordId();
                if (object.count("mapping") > 0) {
                    target = object["mapping"];
                }
                ChordMessage* message = new ChordMessage(msg::CHORD_LOOK_UP);
                message->setSender(id);
                message->setHop(1);
                message->setType(ChordMsgType::CHORD_GET);
                message->setLabel(msg::LABEL_CONTENT);
                json content;
                content["id"] = object["oid"];
                content["x"] = object["oCoord"]["x"];
                content["y"] = object["oCoord"]["y"];
                message->setContent(content.dump().c_str());
                r_transmit(message, target);
            }
        }
    }

    delete msg;
}

void ClientDelayTest::startPlay(cMessage* msg) {
    bool isReady = ContentDistributorAccess().get()->ready;
    if (isReady) {
        scheduleAt(simTime(), refreshTimer);
        scheduleAt(simTime() + walkCycle, walkTimer);
    } else {
        scheduleAt(simTime() + startCycle, startTimer);
    }
}

void ClientDelayTest::move(cMessage* msg) {
    Coordinate oldPosit = location;
    Coordinate oldPositMap = CoordinatorAccess().get()->mapLocation(oldPosit);
    changePosition();
    Coordinate newPositMap = CoordinatorAccess().get()->mapLocation(location);

//    cout << "oldPosit: " << oldPosit << endl;
//    cout << "newPosit: " << location << endl;

    auto line = new cLineFigure("line");
    line->setStart(
            cFigure::Point((double) oldPositMap.x, (double) oldPositMap.y));
    line->setEnd(
            cFigure::Point((double) newPositMap.x, (double) newPositMap.y));
    line->setLineColor(cFigure::BLACK);
    line->setLineWidth(2);
    line->setVisible(true);
    getParentModule()->getParentModule()->getCanvas()->addFigure(line);

    // refresh display
    displayPosition();

    scheduleAt(simTime() + walkCycle, walkTimer);
}

void ClientDelayTest::changePosition() {
    long XMax = CoordinatorAccess().get()->XMax;
    long XMin = 0;
    long YMax = CoordinatorAccess().get()->YMax;
    long YMin = 0;
    long step;

    //if the node has covered the defined distance chose a new direction and speed
    if (steps == 0) {
        //choose the direction angle, from 0 to 2*Pi
        alpha = uniform(0, 2 * M_PI);
        //compute a single step length
        step = walkCycle.dbl() * speed;
        steps = step > 0 ? (int) (moveDist / step) : 1;
    }

    //compute a single step
    step = walkCycle.dbl() * speed;
    long dX = (long) (step * cos(alpha));
    long dY = (long) (step * sin(alpha));

    //do not go outside the map
    //define new <x,y>
    location.x = (location.x + dX);
    location.y = (location.y + dY);
    // rebound x and y
    if (location.x < XMin) {
        dX *= -1; // change the sign
        location.x = XMin;
        alpha = M_PI - alpha;
    }
    if (location.x > XMax) {
        location.x = XMax;
        dX *= -1;
        alpha = M_PI - alpha;
    }
    if (location.y < YMin) {
        dY *= -1;
        location.y = YMin;

        alpha = 2 * M_PI - alpha;
    }
    if (location.y > YMax) {
        dY *= -1;
        location.y = YMax;
        alpha = 2 * M_PI - alpha;
    }

    steps--;
}

void ClientDelayTest::displayPosition() {
    Coordinate mapPosit = CoordinatorAccess().get()->mapLocation(location);
    getParentModule()->getDisplayString().setTagArg("p", 0, mapPosit.x);
    getParentModule()->getDisplayString().setTagArg("p", 1, mapPosit.y);

    // display the perception range
    if (circle != nullptr) {
        getParentModule()->getParentModule()->getCanvas()->removeFigure(circle);
        delete circle;
    }
    if (p_circle != nullptr) {
        getParentModule()->getParentModule()->getCanvas()->removeFigure(
                p_circle);
        delete p_circle;
    }
    circle = new cOvalFigure("search");
    double mapRadius = CoordinatorAccess().get()->mapValue(radius);
    circle->setBounds(
            cFigure::Rectangle((double) (mapPosit.x - mapRadius),
                    (double) (mapPosit.y - mapRadius), (double) (2 * mapRadius),
                    (double) (2 * mapRadius)));
    circle->setLineColor(cFigure::MAGENTA);
    circle->setLineWidth(2);
    circle->setLineStyle(cFigure::LINE_DOTTED);
    circle->setVisible(true);
    getParentModule()->getParentModule()->getCanvas()->addFigure(circle);

    p_circle = new cOvalFigure("AoI");
    double mapPerception = CoordinatorAccess().get()->mapValue(perception);
    p_circle->setBounds(
            cFigure::Rectangle((double) (mapPosit.x - mapPerception),
                    (double) (mapPosit.y - mapPerception),
                    (double) (2 * mapPerception),
                    (double) (2 * mapPerception)));
    p_circle->setLineColor(cFigure::MAGENTA);
    p_circle->setLineWidth(2);
    p_circle->setLineStyle(cFigure::LINE_SOLID);
    p_circle->setVisible(true);
    getParentModule()->getParentModule()->getCanvas()->addFigure(p_circle);
}

void ClientDelayTest::finish() {
    if (circle != NULL) {
        getParentModule()->getParentModule()->getCanvas()->removeFigure(circle);
        delete circle;
    }
    if (p_circle != NULL) {
        getParentModule()->getParentModule()->getCanvas()->removeFigure(
                p_circle);
        delete p_circle;
    }
}
