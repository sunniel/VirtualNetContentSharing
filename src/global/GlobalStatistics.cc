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

#include "GlobalStatistics.h"
#include "GlobalNodeListAccess.h"
#include "../ctrl/CANCtrl.h"

Define_Module(GlobalStatistics);

double meanCalculator(vector<double> list) {
    int lenght = list.size();
    if (lenght == 0)
        return 0;
    double sum = 0;
    for (int i = 0; i < lenght; i++) {
        sum = sum + list[i];
    }
    double mean = sum / (double) lenght;
    return mean;
}

double meanCalculator(vector<int> list) {
    int lenght = list.size();
    if (lenght == 0)
        return 0;
    int sum = 0;
    for (int i = 0; i < lenght; i++) {
        sum = sum + list[i];
    }
    double mean = (double) sum / lenght;
    return mean;
}

double meanCalculator(vector<simtime_t> list) {
    int lenght = list.size();
    if (lenght == 0)
        return 0;
    double sum = 0;
    for (int i = 0; i < lenght; i++) {
        sum = sum + list[i].dbl();
    }
    double mean = (double) sum / lenght;
    return mean;
}

GlobalStatistics::GlobalStatistics() {
    sampler = new cMessage(msg::CAN_LOAD_SAMPLE);
}

GlobalStatistics::~GlobalStatistics() {
    cancelAndDelete(sampler);
}

void GlobalStatistics::initialize() {
    sample_cycle = par("sample_cycle");
    loadSignal = registerSignal("load");

    WATCH_VECTOR(hops);
    WATCH_VECTOR(contentLoadHops);
    WATCH_VECTOR(contentLoadDelays);

    scheduleAt(simTime() + sample_cycle, sampler);
}

void GlobalStatistics::handleMessage(cMessage *msg) {
    if (msg->isName(msg::CAN_LOAD_SAMPLE)) {
        sample(msg);
    }
}

void GlobalStatistics::sample(cMessage *msg) {
    // query the load of each CAN node
    map<unsigned long, IPvXAddress>& cans =
            GlobalNodeListAccess().get()->getAllCANs();
    vector<double> loads;
    for (auto elem : cans) {
        // get the corresponding CANCtrl model
        IPvXAddress addr = elem.second;
        CANInfo* info =
                dynamic_cast<CANInfo*>(GlobalNodeListAccess().get()->getPeerInfo(
                        addr));

        cModule* CAN = getSimulation()->getModule(info->getModuleID());
        CANCtrl* ctrl = dynamic_cast<CANCtrl*>(CAN->getSubmodule("ctrl"));

//        cout << simTime() << " load: " << ctrl->load << endl;
//        cout << simTime() << " age: " << (simTime() - ctrl->birth) << endl;

        if (ctrl->birth <= simTime() - sample_cycle) {
            loads.push_back(ctrl->load);
        } else {
            simtime_t age = simTime() - ctrl->birth;
            if (age > simtime_t::ZERO) {
                double quantifiedLoad = (double) ctrl->load / age.dbl()
                        * sample_cycle.dbl();
                loads.push_back(quantifiedLoad);
            }
        }
        ctrl->load = 0;
    }

    double average = meanCalculator(loads);

//    cout << "average load: " << average << endl;
//    for(auto elem : loads){
//        cout << "load: " << elem << endl;
//    }

    avgLoads.push_back(average);
    emit(loadSignal, average);

    scheduleAt(simTime() + sample_cycle, sampler);
}

void GlobalStatistics::finish() {
    cout << "Generating statistical results" << endl;

//    cout << "[Start routing hop measurement]" << endl;
//    cout << "Sample size: " << hops.size() << endl;
//
//    int max = 0;
//    int min = INT_MAX;
//    for (int j = 0; j < hops.size(); j++) {
//        int hop_count = hops[j];
//        if (hop_count > max)
//            max = hop_count;
//        if (hop_count < min)
//            min = hop_count;
//    }
//    double mean = meanCalculator(hops);
//
//    cout << "routing hops: " << endl;
//    cout << "Mean:  " << mean << "(" << mean - 1 << ") Max Value: " << max
//            << "(" << max - 1 << ") Min Value: " << min << "(" << min - 1 << ")"
//            << endl;
//    cout << "[End routing hop measurement]" << endl;

    cout << "[Start content retrieval measurement]" << endl;

    cout << "Sample size: " << contentLoadHops.size() << endl;
    int max = 0;
    int min = INT_MAX;
    for (int j = 0; j < contentLoadHops.size(); j++) {
        int hop_count = contentLoadHops[j];
        if (hop_count > max)
            max = hop_count;
        if (hop_count < min)
            min = hop_count;
    }
    double mean = meanCalculator(contentLoadHops);
    cout << "Mean hop:  " << mean << " Max hop: " << max << " Min hop: " << min
            << endl;

    simtime_t maxDelay(0.0);
    simtime_t minDelay = SimTime::getMaxTime().getMaxTime();
    for (int j = 0; j < contentLoadDelays.size(); j++) {
        simtime_t delay = contentLoadDelays[j];
        if (delay > maxDelay)
            maxDelay = delay;
        if (delay < minDelay)
            minDelay = delay;
    }
    double meanDelay = meanCalculator(contentLoadDelays);
    cout << "Mean delay:  " << meanDelay << "s, Max delay: " << maxDelay
            << "s, Min delay: " << minDelay << "s" << endl;

    cout << "[End content retrieval measurement]" << endl;

    cout << "CAN join failures: " << JOIN_FAILS << endl;
    cout << "Failures: " << FAILS << " Successes: " << SUCCESS << endl;
    cout << "Chord departure: " << CHORD_DEPARTURE << " Chord new arrival: "
            << CHORD_ARRIVAL << endl;
    cout << "CAN departure: " << CAN_DEPARTURE << " CAN new arrival: "
            << CAN_ARRIVAL << endl;
    cout << "Final Chord size: " << GlobalNodeListAccess().get()->chordSize()
            << endl;
    cout << "Final CAN size: " << GlobalNodeListAccess().get()->canSize()
            << endl;

    cout << "CAN node loads: " << endl;
    for (int i = 1; i <= avgLoads.size(); i++) {
        cout << "[" << (i * sample_cycle) << "] " << avgLoads[i - 1] << endl;
    }

    cout << "End of statistical results generation" << endl;
}

void GlobalStatistics::addHop(int hop) {
    hops.push_back(hop);
}

void GlobalStatistics::addContentLoadHop(int hop) {
    contentLoadHops.push_back(hop);
}

void GlobalStatistics::addContentLoadDelay(simtime_t delay) {
    contentLoadDelays.push_back(delay);
}
