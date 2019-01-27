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

#include "ChordCtrl.h"
#include "../global/ContentDistributorAccess.h"

Define_Module(ChordCtrl);

ChordCtrl::ChordCtrl() {
    fingerToFix = 0;
    maintainer = new cMessage(msg::CHORD_MAINT);
}

void ChordCtrl::final() {
    ;
}

ChordCtrl::~ChordCtrl() {
    // destroy self timer messages
    cancelAndDelete(maintainer);
}

int ChordCtrl::numInitStages() const {
    return 2;
}

void ChordCtrl::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        fullName = getParentModule()->getFullName();
        M = GlobalParametersAccess().get()->getAddrSpaceSize();
        MAXid = pow(2, M);
        succSize = GlobalParametersAccess().get()->getSuccListSize();
        successorList.resize(succSize);
        fingerTable.resize(M);
        maintain_cycle = par("maintain_cycle");
        objectSize = par("object_size");

        WATCH(MAXid);
        WATCH(chordId);
        WATCH(predecessor);
        WATCH_VECTOR(successorList);
        WATCH_VECTOR(fingerTable);
        WATCH_MAP(data);
    } else if (stage == 1) {
        getParentModule()->getDisplayString().setTagArg("t", 0,
                to_string(chordId).c_str());
    }
}

bool ChordCtrl::operator<(const ChordCtrl& chord) const {
    return this->chordId < chord.chordId;
}

void ChordCtrl::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::CHORD_LOOK_UP)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onRoute(chordMsg);
    } else if (msg->isName(msg::CHORD_SUCCESSOR)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onRoute(chordMsg);
    } else if (msg->isName(msg::CHORD_SUCCESSOR_FOUND)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onSuccessorFound(chordMsg);
    } else if (msg->isName(msg::CHORD_FINAL)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onFinal(chordMsg);
    } else if (msg->isName(msg::CHORD_NOTIFY)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onNotify(chordMsg);
    } else if (msg->isName(msg::CHORD_QUERY_SUCCESSOR)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onQuerySuccessor(chordMsg);
    } else if (msg->isName(msg::CHORD_MAINT)) {
        maintain(msg);
    } else if (msg->isName(msg::CHORD_STORE)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onStore(chordMsg);
    } else if (msg->isName(msg::CHORD_GET)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onGet(chordMsg);
    } else if (msg->isName(msg::CHORD_REPLICATE)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onReplicate(chordMsg);
    } else if (msg->isName(msg::CHORD_FIX_REPLICA)) {
        ChordMessage* chordMsg = check_and_cast<ChordMessage*>(msg);
        onFixReplicas(chordMsg);
    }
}

void ChordCtrl::r_transmit(ChordMessage* msg, unsigned long destID) {
    if (GlobalNodeListAccess().get()->isUp(destID)) {
        // in case of message forwarding
        msg->removeControlInfo();
        IPvXAddress srcAddr = GlobalNodeListAccess().get()->getNodeAddr(
                chordId);
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
        cout << simTime() << " lookup failed on " << chordId << " for "
                << msg->getContent() << " with label '" << msg->getLabel()
                << "' at hop " << msg->getHop() << ", due to destination "
                << destID << " failed" << endl;
        GlobalStatisticsAccess().get()->FAILS++;
        handleFailure(msg);
        delete msg;
    }
}

void ChordCtrl::onRoute(ChordMessage* msg) {
    string content = msg->getContent();
    unsigned long target;
    if (msg->getType() != ChordMsgType::CHORD_STORE
            && msg->getType() != ChordMsgType::CHORD_GET) {
        target = util::strToLong(content);
    } else {
        json request = json::parse(content);
        target = request["id"].get<unsigned long>();
    }

    if (msg->getType() == ChordMsgType::CHORD_GET
            && inAB(target, predecessor, chordId)) {
        ChordMessage* getmsg = new ChordMessage(msg::CHORD_GET);
        getmsg->setContent(content.c_str());
        getmsg->setType(msg->getType());
        getmsg->setHop(msg->getHop());
        getmsg->setLabel(msg->getLabel());
        getmsg->setSender(msg->getSender());
        onGet(getmsg);
        delete msg;
    } else if (target == chordId
            || ((msg->getType() == ChordMsgType::SUCCESSOR
                    || msg->getType() == ChordMsgType::CHORD_STORE
                    || msg->getType() == ChordMsgType::CHORD_GET)
                    && inAB(target, chordId, successorList[0]))) {
        if (msg->getType() == ChordMsgType::SUCCESSOR && target == chordId) {
            ChordMessage* finalmsg = new ChordMessage(
                    msg::CHORD_SUCCESSOR_FOUND);
            finalmsg->setType(ChordMsgType::SUCCESSOR_FOUND);
            finalmsg->setContent(profile().dump().c_str());
            finalmsg->setHop(msg->getHop() + 1);
            finalmsg->setLabel(msg->getLabel());
            finalmsg->setSender(chordId);
            r_transmit(finalmsg, msg->getSender());
        } else if (msg->getType() == ChordMsgType::SUCCESSOR
                && inAB(target, chordId, successorList[0])) {
            ChordMessage* querymsg = new ChordMessage(
                    msg::CHORD_QUERY_SUCCESSOR);
            querymsg->setContent(content.c_str());
            querymsg->setType(ChordMsgType::QUERY_SUCCESSOR);
            querymsg->setHop(msg->getHop() + 1);
            querymsg->setLabel(msg->getLabel());
            querymsg->setSender(msg->getSender());
            r_transmit(querymsg, successorList[0]);
        } else { // for look_up messages
            if (msg->getType() == ChordMsgType::CHORD_STORE) {
                ChordMessage* storemsg = new ChordMessage(msg::CHORD_STORE);
                storemsg->setContent(content.c_str());
                storemsg->setType(msg->getType());
                storemsg->setHop(msg->getHop() + 1);
                storemsg->setLabel(msg->getLabel());
                storemsg->setSender(msg->getSender());
                r_transmit(storemsg, successorList[0]);
            } else if (msg->getType() == ChordMsgType::CHORD_GET) {
                ChordMessage* getmsg = new ChordMessage(msg::CHORD_GET);
                getmsg->setContent(content.c_str());
                getmsg->setType(msg->getType());
                getmsg->setHop(msg->getHop() + 1);
                getmsg->setLabel(msg->getLabel());
                getmsg->setSender(msg->getSender());
                r_transmit(getmsg, successorList[0]);
            } else {
                ChordMessage* finalmsg = new ChordMessage(msg::CHORD_FINAL);
                finalmsg->setType(ChordMsgType::CHORD_FINAL);
                // TODO content for look_up message final
                //            finalmsg->setContent(profile.dump().c_str());
                finalmsg->setHop(msg->getHop() + 1);
                finalmsg->setLabel(msg->getLabel());
                finalmsg->setSender(chordId);
                r_transmit(finalmsg, msg->getSender());
            }
        }
        delete msg;
    } else {
        unsigned long dest = closestPrecedingNode(target);
        if (dest == MAXid + 1) {

            cout << simTime() << " lookup " << target << " failed on "
                    << chordId << " with label '" << msg->getLabel()
                    << "' at hop " << msg->getHop() << ", due to max hop "
                    << (MAXid + 1) << " reached for " << chordId << endl;

            GlobalStatisticsAccess().get()->FAILS++;
            handleFailure(msg);
            delete msg;
        } else {
            if (msg->getType() == ChordMsgType::CHORD_LOOK_UP
                    || msg->getType() == ChordMsgType::CHORD_STORE
                    || msg->getType() == ChordMsgType::CHORD_GET) {
                msg->setHop(msg->getHop() + 1);
            }
            r_transmit(msg, dest);
        }
    }
}

void ChordCtrl::onQuerySuccessor(ChordMessage* msg) {
    ChordMessage* finalmsg = new ChordMessage(msg::CHORD_SUCCESSOR_FOUND);
    finalmsg->setType(ChordMsgType::SUCCESSOR_FOUND);
    finalmsg->setContent(profile().dump().c_str());
    finalmsg->setHop(msg->getHop() + 1);
    finalmsg->setLabel(msg->getLabel());
    finalmsg->setSender(chordId);
    r_transmit(finalmsg, msg->getSender());
    delete msg;
}

void ChordCtrl::onFinal(ChordMessage* msg) {
//    GlobalStatisticsAccess().get()->addHop(msg->getHop());
    GlobalStatisticsAccess().get()->SUCCESS++;
    delete msg;
}

void ChordCtrl::onSuccessorFound(ChordMessage* msg) {
    string label = msg->getLabel();
    string content = msg->getContent();
    json succ = json::parse(content);
    bool updatePredecessor = false;
    if (label.find("successor") != string::npos) {  //predecessor
        unsigned long pred = succ["predecessor"];
        if (label.find("first") != string::npos || pred == chordId
                || !GlobalNodeListAccess().get()->isUp(pred)) { // for node join or successor lookup
            successorList.resize(succSize);
            successorList[0] = succ["chordId"];
            if (label.find("first") != string::npos) {
                predecessor = pred;
                GlobalNodeListAccess().get()->ready(chordId);
                startMaint();
            }
            vector<unsigned long> receivedSuccList = succ["successorList"];
            std::copy(receivedSuccList.begin(), receivedSuccList.end() - 1,
                    successorList.begin() + 1);
            updatePredecessor = true;

            // in case the predecessor of the first successor failed
            if (pred != chordId) {
                notify(successorList[0]);
            }
        } else if (label.find("stabilize") != string::npos) {
            if (inAB(pred, chordId, succ["chordId"])) { // in case a new node is found
                successorList.resize(succSize);
                successorList[0] = pred;
                successorList[1] = succ["chordId"];
                vector<unsigned long> receivedSuccList = succ["successorList"];
                std::copy(receivedSuccList.begin(), receivedSuccList.end() - 2,
                        successorList.begin() + 2);
            }
            notify(successorList[0]);
        }

        // update replicas
        for (auto elem : succ["data"]) {
            unsigned long id = elem["id"];
            string value = elem["value"];
            if (inAB(id, predecessor, chordId)) {
                data[id] = value;
            }
        }
    } else if (label.find("finger") != string::npos) {
        vector<string> parts;
        util::splitString(label, " ", parts);
        int index = util::strToInt(parts[1]);
        fingerTable[index] = succ["chordId"];
    }
    delete msg;
}

void ChordCtrl::onNotify(ChordMessage* msg) {
    string content = msg->getContent();
    unsigned long nodeId = util::strToLong(content);
    if (predecessor == MAXid + 1
            || (inAB(nodeId, predecessor, this->chordId) && nodeId != chordId)
            || !GlobalNodeListAccess().get()->isUp(predecessor)) {
        predecessor = nodeId;

//        cout << fullName << " [" << chordId << "] update predecessor" << endl;
    }
    delete msg;
}

void ChordCtrl::onStore(ChordMessage* msg) {
    string content = msg->getContent();
    json request = json::parse(content);
    unsigned long id = request["id"];
    string value = request["value"];
    data[id] = value;
//    GlobalStatisticsAccess().get()->addHop(msg->getHop());

    // replicate value to successors
    int r = GlobalParametersAccess().get()->getChordReplicaSize();
    for (int i = 0; i < r; i++) {
        ChordMessage* replicate = new ChordMessage(msg::CHORD_REPLICATE);
        replicate->setSender(chordId);
        replicate->setContent(request.dump().c_str());
        replicate->setLabel(msg->getLabel());
        r_transmit(replicate, successorList[i]);
    }

//    cout << "Store value " << content << " on " << chordId << endl;

    ContentDistributorAccess().get()->callback(value, chordId);

    delete msg;
}

void ChordCtrl::onGet(ChordMessage* msg) {
    string target = msg->getContent();
    json request = json::parse(target);
    unsigned long id = request["id"];
//    GlobalStatisticsAccess().get()->addHop(msg->getHop());
    string label = msg->getLabel();
    if (data.count(id) > 0) {
        if (label.find(msg::LABEL_TEST) != string::npos) {
            cout << "Return GET result: " << data[id] << endl;
        } else {
            ChordMessage* reply = new ChordMessage(msg::CHORD_REPLY);
            reply->setType(ChordMsgType::CHORD_REPLY);
            json content = json::parse(data[id]);
            content["mapping"] = chordId;
            content["x"] = request["x"];
            content["y"] = request["y"];
            reply->setContent(content.dump().c_str());
            reply->setSender(chordId);
            reply->setLabel(msg->getLabel());
            reply->setHop(msg->getHop() + 1);
            reply->setByteLength(objectSize);
            r_transmit(reply, msg->getSender());

//            cout << simTime() << " " << chordId << " return GET result: "
//                    << data[id] << endl;
        }
        GlobalStatisticsAccess().get()->SUCCESS++;
    } else {
        if (label.find(msg::LABEL_TEST) != string::npos) {
            cout << "Return GET result: " << data::DATA_EMPTY << endl;
        } else {
            handleFailure(msg);
            cout << simTime() << " " << chordId << ": " << data::DATA_EMPTY
                    << " for id " << id << endl;
        }
        GlobalStatisticsAccess().get()->FAILS++;
    }

    delete msg;
}

void ChordCtrl::handleFailure(ChordMessage* msg) {
    if (msg->getType() == ChordMsgType::CHORD_GET) {
        string content = msg->getContent();
        json request = json::parse(content);
        ChordMessage* replyMsg = new ChordMessage(msg::CHORD_REPLY);
        replyMsg->setType(ChordMsgType::CHORD_REPLY);
        json reply;
        reply["data"] = data::DATA_EMPTY;
        reply["x"] = request["x"];
        reply["y"] = request["y"];
        replyMsg->setContent(reply.dump().c_str());
        replyMsg->setSender(chordId);
        replyMsg->setLabel(msg->getLabel());
        replyMsg->setHop(msg->getHop());
        r_transmit(replyMsg, msg->getSender());
    }
}

void ChordCtrl::onReplicate(ChordMessage* msg) {
    string content = msg->getContent();
    json request = json::parse(content);
    unsigned long id = request["id"];
    string value = request["value"];
    data[id] = value;

    delete msg;
}

void ChordCtrl::onFixReplicas(ChordMessage* msg) {
    string content = msg->getContent();
    json entries = json::parse(content);

    for (auto elem : entries["data"]) {
        unsigned long id = elem["id"];
        string value = elem["value"];
        data[id] = value;
    }

    delete msg;
}

void ChordCtrl::startMaint() {
    scheduleAt(simTime() + maintain_cycle, maintainer);
}

void ChordCtrl::maintain(cMessage *msg) {
    stabilize();
    fixFingers();
    fixReplicas();
    scheduleAt(simTime() + maintain_cycle, maintainer);
}

void ChordCtrl::stabilize() {
    for (auto elem : successorList) {
        if (elem != MAXid + 1 && GlobalNodeListAccess().get()->isUp(elem)) {
            successorList[0] = elem;
            findSuccessor(elem, elem, "successor stabilize");
            return;
        }
    }
    EV << "All successors of node " << this->chordId << " are down!" << endl;
}

void ChordCtrl::fixFingers() {
    if (fingerToFix >= M)
        fingerToFix = 0;

    unsigned long id = (unsigned long) (chordId
            + (unsigned long) pow(2, fingerToFix)) % (unsigned long) pow(2, M);

    findSuccessor(chordId, id, "finger " + to_string(fingerToFix));

//    cout << simTime() << " fix finger " << fingerToFix << " on " << chordId
//            << " for id " << id << endl;

    fingerToFix++;
}

void ChordCtrl::fixReplicas() {
    json entries;
    for (auto elem : data) {
        unsigned long id = elem.first;
        if (inAB(id, predecessor, chordId)) {
            string value = elem.second;
            json entry;
            entry["id"] = id;
            entry["value"] = value;
            entries["data"].push_back(entry);
        }
    }

    // replicate value to successors
    if (entries.count("data") > 0) {
        int r = GlobalParametersAccess().get()->getChordReplicaSize();
        for (int i = 0; i < r; i++) {
            ChordMessage* replicate = new ChordMessage(msg::CHORD_FIX_REPLICA);
            replicate->setSender(chordId);
            replicate->setType(ChordMsgType::CHORD_FIX_REPLICA);
            replicate->setContent(entries.dump().c_str());
            r_transmit(replicate, successorList[i]);
        }

//        cout << fullName << " [" << chordId << "] fix replicas" << endl;
    }
}

void ChordCtrl::join(unsigned long bootstrap) {
    // add the macro for context switch
    Enter_Method_Silent();

    cout << simTime() << " add node: " << chordId << endl;

    findSuccessor(bootstrap, chordId, "successor first");
    for (int i = 0; i < M; i++) {
        unsigned long a = (unsigned long) (chordId + (unsigned long) pow(2, i))
        % (unsigned long) pow(2, M);
        findSuccessor(bootstrap, a, "finger " + to_string(i));
    }
}

void ChordCtrl::findSuccessor(unsigned long nodeToAsk, unsigned long id,
        string label) {
    ChordMessage* predmsg = new ChordMessage(msg::CHORD_SUCCESSOR);
    predmsg->setType(ChordMsgType::SUCCESSOR);
    predmsg->setHop(0);
    predmsg->setContent(util::longToStr(id).c_str());
    predmsg->setLabel(label.c_str());
    predmsg->setSender(chordId);

//    cout << simTime() << " look up node: " << id << " on " << chordId
//            << " label: '" << label << "' nodeToAsk: " << nodeToAsk << endl;

    r_transmit(predmsg, nodeToAsk);
}

unsigned long ChordCtrl::closestPrecedingNode(unsigned long id) {
    vector<unsigned long> fullTable = getFullTable();
    unsigned long found = MAXid + 1;
    // equivalent to the last node in fullTable preceding the given ID
    for (int i = fullTable.size() - 1; i >= 0; i--) {
        unsigned long entry = fullTable[i];
        if (entry != MAXid + 1 && GlobalNodeListAccess().get()->isUp(entry)
                && inAB(entry, this->chordId, id)) {
            found = entry;
            break;
        }
    }

    // if the first preceding node is not found, search the id through one-to-one hop
    if (found == MAXid + 1) {
        for (auto elem : successorList) {
            if (GlobalNodeListAccess().get()->isUp(elem)) {
                found = elem;
                break;
            }
        }
    }

    return found;
}

vector<unsigned long> ChordCtrl::getFullTable() {
    set<unsigned long> temp(fingerTable.begin(), fingerTable.end());
    std::copy(successorList.begin(), successorList.end(),
            std::inserter(temp, temp.end()));
    vector<unsigned long> fullTable(temp.begin(), temp.end());

    std::sort(fullTable.begin(), fullTable.end(), Comparator(*this));

    fullTable.push_back(predecessor);
    // remove the chordId from the fullTable
    fullTable.erase(std::remove(fullTable.begin(), fullTable.end(), chordId),
            fullTable.end());
    return fullTable;
}

void ChordCtrl::notify(unsigned long nodeId) {
    ChordMessage* notifyMsg = new ChordMessage(msg::CHORD_NOTIFY);
    notifyMsg->setType(ChordMsgType::NOTIFY);
    notifyMsg->setContent(util::longToStr(chordId).c_str());
    notifyMsg->setSender(chordId);
    r_transmit(notifyMsg, nodeId);

//    cout << fullName << " [" << chordId
//            << "] notify successor for predecessor update" << endl;
}

bool ChordCtrl::inAB(unsigned long id, unsigned long a, unsigned long b) {
    if (id == a || id == b)
        return true;

    if (id > a && id < b)
        return true;
    if (id < a && a > b && id < b)
        return true;

    if (id > b && a > b && id > a)
        return true;

    return false;
}

json ChordCtrl::profile() {
    json profile;
    profile["predecessor"] = predecessor;
    profile["chordId"] = chordId;
    profile["successorList"] = successorList;
    for (auto elem : data) {
        unsigned long id = elem.first;
        if (inAB(id, predecessor, chordId)) {
            string value = elem.second;
            json entry;
            entry["id"] = id;
            entry["value"] = value;
            profile["data"].push_back(entry);
        }
    }
    return profile;
}
