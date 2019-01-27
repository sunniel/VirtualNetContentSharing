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

#ifndef COORDINATOR_H_
#define COORDINATOR_H_

#include <cmath>
#include <set>
#include <omnetpp.h>
#include "../objects/Coordinate.h"

using namespace std;
using namespace omnetpp;

class Coordinator: public cSimpleModule {
public:
    long XMax;
    long YMax;
    long XMin;
    long YMin;
    // length of a region
    long regionSize;
    // length of a grid
    long gridSize;
    long mapXMax;
    long mapYMax;
    long XBorder;
    long YBorder;

    Coordinator();
    virtual ~Coordinator();
    // randomly determine the location of a logical computer
    long randomX();
    long randomY();
    Coordinate randomLocation();
    Coordinate centerLocation();
    int getRegionSize();
    Coordinate getRegion(Coordinate coord);
    Coordinate getGrid(Coordinate coord);
    vector<Coordinate> neighborRegions(Coordinate region);
    vector<Coordinate> neighborGrids(Coordinate grid);
    // convert actual location to map location
    Coordinate mapLocation(Coordinate c);
    // convert actual value to map value
    double mapValue(long c);
protected:
    int numInitStages() const;
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage* msg);

private:

};

extern long distance(Coordinate a, Coordinate b);

#endif /* COORDINATOR_H_ */
