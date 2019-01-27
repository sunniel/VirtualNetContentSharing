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

#include "BasicClientDelayTest.h"
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include "../common/Util.h"
#include "../global/UnderlayConfiguratorAccess.h"
#include "../global/GlobalStatisticsAccess.h"
#include "../global/GlobalNodeListAccess.h"
#include "../global/GlobalParametersAccess.h"
#include "../objects/IPAddress.h"
#include "../objects/IPvXAddress.h"
#include "../objects/SimpleNodeEntry.h"
#include "../objects/SimpleInfo.h"

Define_Module(BasicClientDelayTest);

BasicClientDelayTest::BasicClientDelayTest() {
    refreshTimer = new cMessage(msg::REFRESH_CONTENT);
    walkTimer = new cMessage(msg::REFRESH_POSITION);
    startTimer = new cMessage(msg::CLIENT_START);
    circle = nullptr;
    p_circle = nullptr;
    coverage = 0;
    inventories = 0;
    steps = 0;
    hops = 0;
    triggered = false;
    loaded = false;
}

BasicClientDelayTest::~BasicClientDelayTest() {
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

int BasicClientDelayTest::numInitStages() const {
    return 2;
}

void BasicClientDelayTest::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();
        loadCycle = par("load_cycle");
        walkCycle = par("walk_cycle");
        startCycle = par("start");
        moveDist = par("walk_distance");
        speed = par("speed");

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

bool BasicClientDelayTest::r_transmit(cMessage* msg, unsigned long destID) {
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

        delete msg;
        return false;
    }
}

void BasicClientDelayTest::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::REFRESH_CONTENT)) {
        loadContent(msg);
    } else if (msg->isName(msg::REFRESH_POSITION)) {
        move(msg);
    } else if (msg->isName(msg::CAN_INVENTORY_REPLY)) {
        CANMessage* canMsg = check_and_cast<CANMessage*>(msg);
        onInventory(canMsg);
    } else if (msg->isName(msg::CHORD_REPLY)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onDataReply(chordMsg);
    } else if (msg->isName(msg::CLIENT_START)) {
        startPlay(msg);
    }
}

void BasicClientDelayTest::loadContent(cMessage* msg) {
    if (!triggered) {
        triggered = true;
        start = simTime();

        // clear caches, in case that the last test has not terminated
        crc.clear();
        data.clear();
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

    scheduleAt(simTime() + walkCycle, refreshTimer);
}

void BasicClientDelayTest::onInventory(CANMessage* msg) {
    string content = msg->getContent();
//    hops += msg->getHop();

    if (content.find(data::DATA_EMPTY) == string::npos) {
        inventories++;
        json inventory = json::parse(content);
        for (auto elem : inventory["object"]) {
            // remove objects out of the perception range
            Coordinate coord(elem["oCoord"]["x"], elem["oCoord"]["y"]);
            long dist = distance(coord, origin);
            if (dist <= radius) {
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

        /*
         * start content retrieval based on inventory
         */
        // retrieve the data directly from the addressing bots, if the mapping is not found
        ChordInfo* chord = NULL;
        do {
            chord = GlobalNodeListAccess().get()->randChord();
        } while (chord == NULL
                || !GlobalNodeListAccess().get()->isReady(chord->getChordId()));
        for (auto elem : crc) {
            if (toDownloads.empty()) {
                json object = json::parse(elem.second);
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
                r_transmit(message, chord->getChordId());
            }
            toDownloads.push(elem.second);
        }
    }

    delete msg;
}

void BasicClientDelayTest::onDataReply(ChordMessage* msg) {
    if (!crc.empty()) {
        hops += msg->getHop();
        string content = msg->getContent();
        json reply = json::parse(content);
        if (content.find(data::DATA_EMPTY) != string::npos) {
            json object = json::parse(content);
            Coordinate c(object["oCoord"]["x"], object["oCoord"]["y"]);
            data[c] = object.dump();

            cout << simTime() << " [Client] received data: " << content << endl;
        } else {
            Coordinate c(reply["x"], reply["y"]);
            crc.erase(c);
        }

        toDownloads.pop();

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
            // conclude the run
            triggered = false;
            loaded = false;
            data.clear();
            crc.clear();
            while (!toDownloads.empty()) {
                toDownloads.pop();
            }
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

                string toDownload = toDownloads.front();
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

void BasicClientDelayTest::startPlay(cMessage* msg) {
    bool isReady = ContentDistributorAccess().get()->ready;
    if (isReady) {
        scheduleAt(simTime(), refreshTimer);
        scheduleAt(simTime() + loadCycle, walkTimer);

    } else {
        scheduleAt(simTime() + startCycle, startTimer);
    }
}

void BasicClientDelayTest::move(cMessage* msg) {
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

void BasicClientDelayTest::changePosition() {
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

void BasicClientDelayTest::displayPosition() {
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
    cout << "mapRadius: " << mapRadius << endl;
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

void BasicClientDelayTest::finish() {
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
