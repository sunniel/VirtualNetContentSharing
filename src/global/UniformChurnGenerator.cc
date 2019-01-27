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

#include "UniformChurnGenerator.h"

Define_Module(UniformChurnGenerator);

UniformChurnGenerator::UniformChurnGenerator() {
    churnChord = NULL;
    churnCan = NULL;
}

UniformChurnGenerator::~UniformChurnGenerator() {
    if (churnChord != NULL) {
        cancelAndDelete(churnChord);
    }
    if (churnCan != NULL) {
        cancelAndDelete(churnCan);
    }
}

void UniformChurnGenerator::initialize() {
    departChordRate = par("depart_chord_rate");
    arrivalChordRate = par("arrival_chord_rate");
    churnChordCycle = par("churn_chord_cycle");
    departCanRate = par("depart_can_rate");
    arrivalCanRate = par("arrival_can_rate");
    churnCanCycle = par("churn_can_cycle");
    max = par("overlay_size_max");
    min = par("overlay_size_min");
    churnChord = new cMessage(msg::CHURN_CHORD_CYCLE);
    churnCan = new cMessage(msg::CHURN_CAN_CYCLE);
    scheduleAt(simTime() + churnChordCycle, churnChord);
    scheduleAt(simTime() + churnCanCycle, churnCan);
}

void UniformChurnGenerator::handleMessage(cMessage *msg) {
    if (msg->isName(msg::CHURN_CHORD_CYCLE)) {
        double depart = uniform(0, 1);
        int size = GlobalNodeListAccess().get()->chordSize();
        if (depart < departChordRate && size > min) {
            ChordInfo* toRemove = NULL;
            do {
                toRemove = GlobalNodeListAccess().get()->randChord();
            } while (toRemove == NULL);
            IPvXAddress addrToRemove =
                    GlobalNodeListAccess().get()->getNodeAddr(
                            toRemove->getChordId());
            UnderlayConfiguratorAccess().get()->removeNode(addrToRemove);

            cout << simTime() << " remove Chord node: " << toRemove->getChordId()
                    << endl;

            GlobalStatisticsAccess().get()->CHORD_DEPARTURE++;
        }
        double arrival = uniform(0, 1);
        if (arrival < arrivalChordRate && size < max) {
            UnderlayConfiguratorAccess().get()->addChord();
            GlobalStatisticsAccess().get()->CHORD_ARRIVAL++;
        }
        scheduleAt(simTime() + churnChordCycle, churnChord);
    } else if (msg->isName(msg::CHURN_CAN_CYCLE)) {
        double depart = uniform(0, 1);
        int size = GlobalNodeListAccess().get()->canSize();
        if (depart < departCanRate && size > min) {
            CANInfo* toRemove = NULL;
            int zoneNumInCharge = 0;
            do {
                toRemove = GlobalNodeListAccess().get()->randCAN();
                zoneNumInCharge = GlobalNodeListAccess().get()->zonesInCharge(
                        toRemove->getId());
                /*
                 * Restrict the selection of node failure, since currently the
                 * zone re-assignement algorithm is not implemented. This restriction
                 * can be removed the implementation of the zone re-assignment algorithm.
                 */
            } while (toRemove == NULL || zoneNumInCharge < 1
                    || zoneNumInCharge > 2);
            IPvXAddress addrToRemove =
                    GlobalNodeListAccess().get()->getNodeAddr(
                            toRemove->getId());
            UnderlayConfiguratorAccess().get()->removeNode(addrToRemove);

            cout << simTime() << " remove CAN node: " << toRemove->getId() << endl;

            GlobalStatisticsAccess().get()->CAN_DEPARTURE++;
        }
        double arrival = uniform(0, 1);
        if (arrival < arrivalCanRate && size < max) {
            UnderlayConfiguratorAccess().get()->addCAN();
            GlobalStatisticsAccess().get()->CAN_ARRIVAL++;
        }
        scheduleAt(simTime() + churnCanCycle, churnCan);
    }
}
