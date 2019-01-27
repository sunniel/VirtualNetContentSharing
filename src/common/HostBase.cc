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

#include "HostBase.h"

HostBase::HostBase() {
    // TODO Auto-generated constructor stub
}

HostBase::~HostBase() {
    // TODO Auto-generated destructor stub
}

void HostBase::initialize() {
    fullName = getFullName();
    moduleId = getId();
//    maxDelay = simulation.getSystemModule()->par("maxDelay");
//    dropRate = simulation.getSystemModule()->par("dropRate").doubleValue();
    globalNodeList = GlobalNodeListAccess().get();
    clockError = 0;
    WATCH(ipAddress);
    WATCH(moduleId);
    WATCH(clockError);
}

void HostBase::initialize(int stage){
    ;
}

IPvXAddress HostBase::getIPAddress(string ipAddr) {
    IPvXAddress addr(ipAddr.c_str());
    return addr;
}

IPvXAddress HostBase::getIPAddress() {
    return ipAddress;
}

void HostBase::multicast(cMessage* message, set<IPvXAddress> destAddrs) {
    for (set<IPvXAddress>::iterator it = destAddrs.begin();
            it != destAddrs.end(); ++it) {
        cMessage* msgDup = message->dup();
        IPvXAddress dest = *it;
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(dest);
        udpControlInfo->setSrcAddr(ipAddress);
        msgDup->setControlInfo(udpControlInfo);
        transmit(msgDup, dest);
    }
    delete message;
}

void HostBase::multicast(cMessage* message, set<string> destAddrs) {
    for (set<string>::iterator it = destAddrs.begin(); it != destAddrs.end();
            it++) {
        cMessage* msgDup = message->dup();
        IPvXAddress dest = getIPAddress(*it);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(dest);
        udpControlInfo->setSrcAddr(ipAddress);
        msgDup->setControlInfo(udpControlInfo);
        transmit(msgDup, dest);
    }
    delete message;
}

/*
 * Reliable multicast
 */
void HostBase::r_multicast(cMessage* message, set<IPvXAddress> destAddrs) {
    for (set<IPvXAddress>::iterator it = destAddrs.begin();
            it != destAddrs.end(); ++it) {
        cMessage* msgDup = message->dup();
        IPvXAddress dest = *it;
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(dest);
        udpControlInfo->setSrcAddr(ipAddress);
        msgDup->setControlInfo(udpControlInfo);
        r_transmit(msgDup, dest);
    }
    delete message;
}

/*
 * Reliable multicast
 */
void HostBase::r_multicast(cMessage* message, set<string> destAddrs) {
    for (set<string>::iterator it = destAddrs.begin(); it != destAddrs.end();
            it++) {
        cMessage* msgDup = message->dup();
        IPvXAddress dest = getIPAddress(*it);
        UDPControlInfo* udpControlInfo = new UDPControlInfo();
        udpControlInfo->setDestAddr(dest);
        udpControlInfo->setSrcAddr(ipAddress);
        msgDup->setControlInfo(udpControlInfo);
        r_transmit(msgDup, dest);
    }
    delete message;
}

void HostBase::transmit(cMessage* msg, IPvXAddress dest) {
    if (dest == ipAddress) {
        // send to self
        sendIdeal(msg);
    } else {
        sendMsg(msg);
    }
}

/*
 * Reliable message transmission
 */
void HostBase::r_transmit(cMessage* msg, IPvXAddress dest) {
    if (dest == ipAddress) {
        // send to self
        sendIdeal(msg);
    } else {
        sendReliable(msg);
    }
}

/*
 * a message send method without transmission delay and packet loss
 */
void HostBase::sendIdeal(cMessage* msg) {
    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
            msg->getControlInfo());
    IPvXAddress destAddr = udpCtrl->getDestAddr();
    PeerInfo* peerInfo = globalNodeList->getPeerInfo(destAddr);
    if (peerInfo != NULL) {
        SimpleInfo* simpleInfo =
                dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(destAddr));
        SimpleNodeEntry* destEntry = simpleInfo->getEntry();

        cSimpleModule::sendDirect(msg, 0, 0, destEntry->getUdpIPv4Gate());
    } else {
        delete msg;
    }
}

void HostBase::sendFixedDelay(cMessage* msg) {
    UDPControlInfo *udpCtrl = check_and_cast<UDPControlInfo *>(
            msg->getControlInfo());
    IPvXAddress destAddr = udpCtrl->getDestAddr();
    PeerInfo* peerInfo = globalNodeList->getPeerInfo(destAddr);
    if (peerInfo != NULL) {
        SimpleInfo* simpleInfo =
                dynamic_cast<SimpleInfo*>(globalNodeList->getPeerInfo(destAddr));
        SimpleNodeEntry* destEntry = simpleInfo->getEntry();

        simtime_t latency = GlobalParametersAccess().get()->getFixLatency();
        cSimpleModule::sendDirect(msg, 0, latency, destEntry->getUdpIPv4Gate());
    } else {
        delete msg;
    }
}

void HostBase::sendMsg(cMessage* msg) {
    send(msg, this->gateHalf("udplink", cGate::OUTPUT));
}

/*
 * Reliable messages send, no message drop
 */
void HostBase::sendReliable(cMessage* msg) {
    send(msg, this->gateHalf("tcplink", cGate::OUTPUT));
}

simtime_t HostBase::readClock() {
    return simTime() + clockError;
}

void HostBase::setIPAddress(IPvXAddress& addr) {
    ipAddress = addr;
}

void HostBase::handleMessage(cMessage* msg) {

    dispatchHandler(msg);
}
