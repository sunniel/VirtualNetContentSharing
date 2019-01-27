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

#include "UnderlayConfigurator.h"
#include "../ctrl/ChordCtrl.h"
#include "../ctrl/CANCtrl.h"

Define_Module(UnderlayConfigurator);

UnderlayConfigurator::UnderlayConfigurator() {
    chord_counter = 0;
    can_counter = 0;
}

UnderlayConfigurator::~UnderlayConfigurator() {
    disposeFailures();
}

int UnderlayConfigurator::numInitStages() const {
    return 1;
}

void UnderlayConfigurator::initialize(int stage) {
    nextFreeAddress = 0x1000001;
    globalNodeList = GlobalNodeListAccess().get();
    parameterList = GlobalParametersAccess().get();
    contentDist = ContentDistributorAccess().get();

    WATCH_MAP(death_schedule);

    // initialize Chord overlay
    cMessage* overlayInit = new cMessage(msg::INIT_OVERLAYS);
    scheduleAt(0, overlayInit);
}

void UnderlayConfigurator::handleMessage(cMessage* msg) {
    if (msg->isName(msg::INIT_OVERLAYS)) {
        handleChordInit();
        handleCANInit();
        delete msg;
    }
}

bool compare(ChordCtrl* a, ChordCtrl* b) {
    return a->chordId < b->chordId;
}

void UnderlayConfigurator::handleChordInit() {
    vector<ChordCtrl*> overlay;
    int size = parameterList->getChordInitSize();

    const char* chordType = par("chord_type");
    const char* chordName = par("chord_name");

    for (int i = 0; i < size; i++) {
        // create a new node
        cModuleType *moduleType = cModuleType::get(chordType);
        cModule* parent = getParentModule()->getSubmodule("chordOverlay");
        // create (possibly compound) module and build its submodules (if any)
        cModule* chord = moduleType->create(chordName, parent,
                chord_counter + 1, chord_counter);
        chord_counter++;
        // set up parameters, if any
        chord->finalizeParameters();
        chord->buildInside();
        // create activation messages
        chord->scheduleStart(0);
        chord->callInitialize(0);
        // create address for the chord control protocol
        ChordCtrl* ctrl = check_and_cast<ChordCtrl*>(
                chord->getSubmodule("ctrl"));
        IPvXAddress addr = IPAddress(nextFreeAddress++);
        ctrl->setIPAddress(addr);
        ctrl->chordId = util::getSHA1(addr.get4().str() + "4000",
                parameterList->getAddrSpaceSize()); // Hash(IP || port)
        chord->callInitialize(1);
        // create meta information
        SimpleNodeEntry* entry = new SimpleNodeEntry(chord);
        ChordInfo* info = new ChordInfo(chord->getId(), chord->getFullName());
        info->setEntry(entry);
        info->setChordId(ctrl->chordId);
        //add host to bootstrap oracle
        globalNodeList->addPeer(addr, info);
        overlay.push_back(ctrl);
    }

    // sort chord nodes by chordId
    std::sort(overlay.begin(), overlay.end(), compare);

    // create finger table and successor list for each chord node
    for (int i = 0; i < overlay.size(); i++) {
        ChordCtrl* ctrl = overlay[i];
        for (int a = 0; a < parameterList->getSuccListSize(); a++) {
            int index = (a + i + 1) % overlay.size();
            ctrl->successorList[a] = overlay[index]->chordId;
        }
        if (i > 0) {
            ctrl->predecessor = overlay[i - 1]->chordId;
        } else {
            ctrl->predecessor = overlay[overlay.size() - 1]->chordId;
        }
        int fingerTableSize = parameterList->getAddrSpaceSize();
        for (int j = 0; j < fingerTableSize; j++) {
            unsigned long a = (unsigned long) (ctrl->chordId
                    + (unsigned long) pow(2, j))
                    % (unsigned long) pow(2, fingerTableSize);
            // find the chord node by for the given ID
            ChordCtrl* cp = overlay[0];
            for (int k = 0; k < overlay.size(); k++) {
                ChordCtrl* temp = overlay[k];
                if (temp->chordId >= a) {
                    cp = temp;
                    break;
                }
            }
            ctrl->fingerTable[j] = cp->chordId;
        }
    }

    for (auto elem : overlay) {
        GlobalNodeListAccess().get()->ready(elem->chordId);
        elem->startMaint();
    }
}

void UnderlayConfigurator::handleCANInit() {
    const char* canType = par("can_type");
    const char* canName = par("can_name");

    // create the master node, i.e.,  the first node
    // create a new node
    cModuleType *moduleType = cModuleType::get(canType);
    cModule* parent = getParentModule()->getSubmodule("canOverlay");
    // create (possibly compound) module and build its submodules (if any)
    cModule* first = moduleType->create(canName, parent, can_counter + 1,
            can_counter);
    can_counter++;
    // set up parameters, if any
    first->finalizeParameters();
    first->buildInside();
    // create activation messages
    first->scheduleStart(0);
    first->callInitialize(0);

    // create address for the can control protocol
    CANCtrl* ctrl = check_and_cast<CANCtrl*>(first->getSubmodule("ctrl"));
    IPvXAddress firstAddr = IPAddress(nextFreeAddress++);
    ctrl->setIPAddress(firstAddr);
    ctrl->info.setId(
            util::getSHA1(firstAddr.get4().str() + "4000",
                    parameterList->getAddrSpaceSize())); // Hash(IP || port)
    first->callInitialize(1);

    // create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(first);
    CANInfo* info = new CANInfo(first->getId(), first->getFullName());
    info->setEntry(entry);
    info->setId(ctrl->info.getId());
    ctrl->info.setId(info->getId());
    //add host to bootstrap oracle
    globalNodeList->addPeer(firstAddr, info);

    // init the master node
    ctrl->initArea();
    ctrl->startMaint();
    globalNodeList->updateCANProfile(ctrl->info);
    globalNodeList->addZone(ctrl->info);
    globalNodeList->ready(ctrl->info.getId());
    addCAN(true);
}

void UnderlayConfigurator::removeNode(IPvXAddress& nodeAddr) {
    SimpleInfo* info = dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(
            nodeAddr));
    if (info != nullptr) {
        SimpleNodeEntry* destEntry = info->getEntry();
        cModule* node = destEntry->getUdpIPv4Gate()->getOwnerModule();
        node->callFinish();
        node->deleteModule();
        globalNodeList->killPeer(nodeAddr);
        parameterList->remoteHost(nodeAddr);
    }
}

void UnderlayConfigurator::addChord() {
    const char* chordType = par("chord_type");
    const char* chordName = par("chord_name");

    // search a bootstrap node to join
    ChordInfo* bootstrap = NULL;
    do {
        bootstrap = GlobalNodeListAccess().get()->randChord();
    } while (bootstrap == NULL
            || !GlobalNodeListAccess().get()->isReady(bootstrap->getChordId()));

    // create a new node
    cModuleType *moduleType = cModuleType::get(chordType);
    cModule* parent = getParentModule()->getSubmodule("chordOverlay");
    // create (possibly compound) module and build its submodules (if any)
    cModule* chord = moduleType->create(chordName, parent, chord_counter + 1,
            chord_counter);
    chord_counter++;
    // set up parameters, if any
    chord->finalizeParameters();
    chord->buildInside();
    // create activation messages
    chord->scheduleStart(0);
    chord->callInitialize(0);

    // create address for the chord control protocol
    ChordCtrl* ctrl = check_and_cast<ChordCtrl*>(chord->getSubmodule("ctrl"));
    IPvXAddress addr = IPAddress(nextFreeAddress++);
    ctrl->setIPAddress(addr);
    int port = 4000;
    unsigned long id = util::getSHA1(addr.get4().str() + to_string(4000),
            parameterList->getAddrSpaceSize()); // Hash(IP || port)
    while (globalNodeList->isUp(id)) {
        port++;
        id = util::getSHA1(addr.get4().str() + to_string(port),
                parameterList->getAddrSpaceSize()); // Hash(IP || port)
        cout << "id: " << id << endl;
    }
    ctrl->chordId = id;
    chord->callInitialize(1);

    // create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(chord);
    ChordInfo* info = new ChordInfo(chord->getId(), chord->getFullName());
    info->setEntry(entry);
    info->setChordId(ctrl->chordId);
    //add host to bootstrap oracle
    globalNodeList->addPeer(addr, info);

    // add the node to the overlay through bootstrap
    ctrl->join(bootstrap->getChordId());
}

void UnderlayConfigurator::addCAN(bool init) {
    const char* canType = par("can_type");
    const char* canName = par("can_name");

    // search a bootstrap node to join
    CANInfo* bootstrap = NULL;
    do {
        bootstrap = GlobalNodeListAccess().get()->randCAN();
    } while (bootstrap == NULL
            || !GlobalNodeListAccess().get()->isReady(bootstrap->getId()));

    // create a new node
    cModuleType *moduleType = cModuleType::get(canType);
    cModule* parent = getParentModule()->getSubmodule("canOverlay");
    // create (possibly compound) module and build its submodules (if any)
    cModule* can = moduleType->create(canName, parent, can_counter + 1,
            can_counter);
    can_counter++;
    // set up parameters, if any
    can->finalizeParameters();
    can->buildInside();
    // create activation messages
    can->scheduleStart(0);
    can->callInitialize(0);

    // create address for the can control protocol
    CANCtrl* ctrl = check_and_cast<CANCtrl*>(can->getSubmodule("ctrl"));
    IPvXAddress addr = IPAddress(nextFreeAddress++);
    ctrl->setIPAddress(addr);
    int port = 4000;
    unsigned long id = util::getSHA1(addr.get4().str() + to_string(4000),
            parameterList->getAddrSpaceSize()); // Hash(IP || port)
    while (globalNodeList->isUp(id)) {
        port++;
        id = util::getSHA1(addr.get4().str() + to_string(port),
                parameterList->getAddrSpaceSize()); // Hash(IP || port)
    }
    ctrl->info.setId(id);
    can->callInitialize(1);

// create meta information
    SimpleNodeEntry* entry = new SimpleNodeEntry(can);
    CANInfo* info = new CANInfo(can->getId(), can->getFullName());
    info->setEntry(entry);
    info->setId(ctrl->info.getId());
    ctrl->info.setId(info->getId());
//add host to bootstrap oracle
    globalNodeList->addPeer(addr, info);

// add the node to the overlay through bootstrap
    if (init) {
        ctrl->join(bootstrap->getId(), msg::LABEL_INIT);
    } else {
        ctrl->join(bootstrap->getId());
    }
}

void UnderlayConfigurator::initCANAdded() {
    if (globalNodeList->canSize() < parameterList->getCANInitSize()) {
        addCAN(true);
    } else {
        contentDist->generate();
    }
}

uint32_t UnderlayConfigurator::freeAddress() {
    return nextFreeAddress++;
}

void UnderlayConfigurator::disposeFailures() {
//    try {
//        for (map<string, Failure*>::iterator it = failures.begin();
//                it != failures.end(); ++it) {
//            Failure* failure = it->second;
//            if (failure != NULL) {
//                cancelAndDelete(failure);
//            }
//        }
//        failures.clear();
//    } catch (exception& e) {
//        EV << e.what() << endl;
//    }
}
