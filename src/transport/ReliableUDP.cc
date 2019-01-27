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

#include "ReliableUDP.h"

Define_Module(ReliableUDP);

void ReliableUDP::initialize() {
    Copt = getSimulation()->getSystemModule()->par("Copt");
    BW = getSimulation()->getSystemModule()->par("BW");
    MTU = getSimulation()->getSystemModule()->par("MTU");
    mu = getSimulation()->getSystemModule()->par("jitter_mu");
    sigma = getSimulation()->getSystemModule()->par("jitter_sigma");
    pktLossRate = getSimulation()->getSystemModule()->par("pktLossRate");
    Dmin = getSimulation()->getSystemModule()->par("Dmin");
}

void ReliableUDP::handleMessage(cMessage *msg) {
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
void ReliableUDP::handleOutgoingMessage(cMessage* packet) {
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
            double Dmin = 1000 * distance / Copt;
            // calculate one-way jitter
            double j = lognormal(mu, sigma) / 2;
            // calculate the communication delay
            double Dcomm = 1000 * (MTU / BW);
            //    double Dcomm = truncnormal(j, j);
            // calculate the variable latency
            double Dvar = exponential(1 / j);
            // calculate the latency of a single MTU, in second
            // double (2 *) the time for receipt confirmation
            mtu_delay = 2 * (Dmin + Dcomm + Dvar) / 1000;
        } else {
            simtime_t jitter = getSimulation()->getSystemModule()->par(
                    "jitter");
            // double (2 *) the time for receipt confirmation
            mtu_delay = 2 * (Dmin + jitter);
        }

        // calculate the whole latency, in second
        simtime_t delay = mtu_delay;
        // reliable packet sent for each MTU
        int packetNum = 1;
        // message size, in bit
        int msgSize =
                dynamic_cast<cPacket*>(packet) != NULL ?
                        dynamic_cast<cPacket*>(packet)->getBitLength() : 0;
        if (msgSize > 0) {
            packetNum = ceil((msgSize) / (double) MTU);
            // delay = propagation delay + transmission delay
            delay = msgSize / BW + mtu_delay;
        }
        simtime_t total_packet_delay = 0;
        for (int i = 0; i < packetNum; i++) {
            simtime_t packet_delay = 0;
            double packetLoss = uniform(0, 1);
            // in case of packet loss, re-send the packet
            while (packetLoss < pktLossRate) {
                packet_delay += mtu_delay;
                packetLoss = uniform(0, 1);
            }
            total_packet_delay += packet_delay;
        }
        delay += total_packet_delay;

//        cout << "BW: " << BW << endl;
//        cout << "MTU: " << MTU << endl;
//        cout << "mtu_delay: " << mtu_delay << endl;
//        cout << "msgSize: " << msgSize << endl;
//        cout << "packetNum: " << packetNum << endl;
//        cout << "total_packet_delay: " << total_packet_delay << endl;
//        cout << "delay: " << delay << endl;

        cSimpleModule::sendDirect(packet, delay, 0,
                destEntry->getTcpIPv4Gate());
    } else {
        delete packet;
    }
}

void ReliableUDP::handleIncomingMessage(cMessage* packet) {
    send(packet, this->gateHalf("uplink", cGate::OUTPUT));
}
