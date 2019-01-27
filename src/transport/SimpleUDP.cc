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

#include "SimpleUDP.h"

Define_Module(SimpleUDP);

void SimpleUDP::initialize() {
    Copt = getSimulation()->getSystemModule()->par("Copt");
    BW = getSimulation()->getSystemModule()->par("BW");
    MTU = getSimulation()->getSystemModule()->par("MTU");
    mu = getSimulation()->getSystemModule()->par("jitter_mu");
    sigma = getSimulation()->getSystemModule()->par("jitter_sigma");
    pktLossRate = getSimulation()->getSystemModule()->par("pktLossRate");
    Dmin = getSimulation()->getSystemModule()->par("Dmin");
}

void SimpleUDP::handleMessage(cMessage *msg) {
    if (msg->getArrivalGateId()
            == this->gateHalf("uplink", cGate::INPUT)->getId()) {
        // process packet from upper layer
        handleOutgoingMessage(msg);

    } else if (msg->getArrivalGateId()
            == this->gateHalf("downlink", cGate::INPUT)->getId()) {
        // process packet from remote host
        handleIncomingMessage(msg);
    }
}

/*
 * msgSize: message size, in bytes
 */
void SimpleUDP::handleOutgoingMessage(cMessage* packet) {
    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
            packet->getControlInfo());
    IPvXAddress srcAddr = udpCtrl->getSrcAddr();
    IPvXAddress destAddr = udpCtrl->getDestAddr();
    GlobalNodeList* globalNodeList = GlobalNodeListAccess().get();
    PeerInfo* peerInfo = globalNodeList->getPeerInfo(destAddr);
    if (peerInfo != NULL) {
        SimpleInfo* simpleInfo =
                dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(destAddr));
        SimpleNodeEntry* destEntry = simpleInfo->getEntry();

        simtime_t mtu_delay = 0;
        int latencyGenerationMode =
                GlobalParametersAccess().get()->getLatencyGenerationMode();
        if (latencyGenerationMode == 0) {
            // calculate the minimum one-way latency, in millisecond
            double distance = GlobalParametersAccess().get()->getDistance(
                    srcAddr, destAddr);
            double Dmin = 1000 * (distance / Copt);
            // calculate one-way jitter
            double j = lognormal(mu, sigma) / 2;
            // calculate the communication delay
            double Dcomm = 1000 * MTU / BW;
            //    double Dcomm = truncnormal(j, j);
            // calculate the variable latency
            double Dvar = exponential(1 / j);
            // calculate the latency of a single MTU, in second
            mtu_delay = (Dmin + Dcomm + Dvar) / 1000;
        } else {
            simtime_t jitter = getSimulation()->getSystemModule()->par(
                    "jitter");
            mtu_delay = Dmin + jitter;
        }

        // calculate the whole latency, in second
        simtime_t delay = mtu_delay;
        // message size, in bit
        int msgSize =
                dynamic_cast<cPacket*>(packet) != NULL ?
                        dynamic_cast<cPacket*>(packet)->getBitLength() : 0;
        if (msgSize > 0) {
            // delay = propagation delay + transmission delay
            delay = msgSize / BW + mtu_delay;
        }

//        EV << "distance: " << distance << endl;
//        EV << "Dmin: " << Dmin << endl;
//        EV << "Dcomm: " << Dcomm << endl;
//        EV << "Dvar: " << Dvar << endl;
//        EV << "mtu_delay: " << mtu_delay << endl;
//        EV << "mesage size: " << msgSize << endl;
//        EV << "delay: " << delay << endl;

        if (uniform(0, 1) > pktLossRate) {
            cSimpleModule::sendDirect(packet, delay, 0,
                    destEntry->getUdpIPv4Gate());
        } else {
            delete packet;
        }
    } else {
        delete packet;
    }
}

void SimpleUDP::handleIncomingMessage(cMessage* packet) {
    send(packet, this->gateHalf("uplink", cGate::OUTPUT));
}
