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

#include "TrailCreator.h"
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

Define_Module(TrailCreator);

TrailCreator::TrailCreator() {
    walkTimer = new cMessage(msg::REFRESH_POSITION);
    startTimer = new cMessage(msg::CLIENT_START);
    circle = nullptr;
    steps = 0;
}

TrailCreator::~TrailCreator() {
    if (walkTimer != NULL) {
        cancelAndDelete(walkTimer);
    }
    if (startTimer != NULL) {
        cancelAndDelete(startTimer);
    }
}

int TrailCreator::numInitStages() const {
    return 2;
}

void TrailCreator::initialize(int stage) {
    if (stage == 0) {
        HostBase::initialize();
        walkCycle = par("walk_cycle");
        startCycle = par("start");
        moveDist = par("walk_distance");
        speed = par("speed");
        timeoutLength = par("load_timeout_length");

        // Let radius be the half of the region diagonal length
        radius = sqrt(2) / (double) 2 * CoordinatorAccess().get()->regionSize;
        outfile.open("../simulations/trail.csv", ios_base::out);
        if (!outfile.is_open()) {
            std::cerr << "Couldn't open 'trail.csv'" << std::endl;
            endSimulation();
        }
        outfile << "// x" << "," << "y" << endl;

        WATCH(id);
    } else if (stage == 1) {
        // initialize start location
        origin = location = CoordinatorAccess().get()->centerLocation();
        displayPosition();

        scheduleAt(simTime() + startCycle, startTimer);
    }
}

void TrailCreator::dispatchHandler(cMessage *msg) {
    if (msg->isName(msg::REFRESH_POSITION)) {
        move(msg);
    } else if (msg->isName(msg::CLIENT_START)) {
        startPlay(msg);
    }
}

void TrailCreator::startPlay(cMessage* msg) {
    bool isReady = ContentDistributorAccess().get()->ready;
    if (isReady) {
        scheduleAt(simTime() + walkCycle, walkTimer);
    } else {
        scheduleAt(simTime() + startCycle, startTimer);
    }
}

void TrailCreator::move(cMessage* msg) {
    Coordinate oldPosit = location;
    Coordinate oldPositMap = CoordinatorAccess().get()->mapLocation(oldPosit);
    changePosition();
    outfile << location.x << "," << location.y << endl;
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

void TrailCreator::changePosition() {
    long XMax = CoordinatorAccess().get()->XMax - radius;
    long XMin = radius;
    long YMax = CoordinatorAccess().get()->YMax - radius;
    long YMin = radius;
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

void TrailCreator::displayPosition() {
    Coordinate mapPosit = CoordinatorAccess().get()->mapLocation(location);
    getParentModule()->getDisplayString().setTagArg("p", 0, mapPosit.x);
    getParentModule()->getDisplayString().setTagArg("p", 1, mapPosit.y);

    // display the perception range
    if (circle != nullptr) {
        getParentModule()->getParentModule()->getCanvas()->removeFigure(circle);
        delete circle;
    }
    circle = new cOvalFigure("AoI");
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
}

void TrailCreator::finish() {
    if (circle != NULL) {
        getParentModule()->getParentModule()->getCanvas()->removeFigure(circle);
        delete circle;
    }
    outfile.close();
}
