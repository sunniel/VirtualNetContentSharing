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

#include "Coordinator.h"
#include <cmath>

Define_Module(Coordinator);

Coordinator::Coordinator() {
    ;
}

Coordinator::~Coordinator() {
    // TODO Auto-generated destructor stub
}

int Coordinator::numInitStages() const {
    return 1;
}

void Coordinator::initialize(int stage) {
    XMax = par("XMax");
    YMax = par("YMax");
    XMin = par("XMin");
    YMin = par("YMin");
    mapXMax = par("map_XMax");
    mapYMax = par("map_YMax");
    XBorder = par("XBorder");
    YBorder = par("YBorder");
    regionSize = par("region_size");
    gridSize = par("grid_size");

    // draw map border
    cFigure::Point upperLeft((double) XBorder, (double) YBorder);
    cFigure::Point upperRight((double) mapXMax, (double) YBorder);
    cFigure::Point lowerLeft((double) XBorder, (double) mapYMax);
    cFigure::Point lowerRight((double) mapXMax, (double) mapYMax);
    auto line = new cLineFigure("line1");
    line->setStart(upperLeft);
    line->setEnd(upperRight);
    line->setLineColor(cFigure::BLUE);
    line->setLineWidth(2);
    line->setVisible(true);
    getParentModule()->getCanvas()->addFigure(line);
    line = new cLineFigure("line2");
    line->setStart(upperLeft);
    line->setEnd(lowerLeft);
    line->setLineColor(cFigure::BLUE);
    line->setLineWidth(2);
    line->setVisible(true);
    getParentModule()->getCanvas()->addFigure(line);
//    line = new cLineFigure("line3");
//    line->setStart(upperRight);
//    line->setEnd(lowerRight);
//    line->setLineColor(cFigure::BLUE);
//    line->setLineWidth(2);
//    line->setVisible(true);
//    getParentModule()->getCanvas()->addFigure(line);
//    line = new cLineFigure("line4");
//    line->setStart(lowerLeft);
//    line->setEnd(lowerRight);
//    line->setLineColor(cFigure::BLUE);
//    line->setLineWidth(2);
//    line->setVisible(true);
//    getParentModule()->getCanvas()->addFigure(line);

    int regionRows = (int) ceil((double) XMax / regionSize);
    double regionSpan = mapValue(regionSize);
    int gridRows = regionSize / gridSize - 1;
    double gridSpan = mapValue(gridSize);
    for (int i = 1; i <= regionRows; i++) {
        // draw grid borders
        for (int j = 1; j <= gridRows; j++) {
            auto line = new cLineFigure("line");
            cFigure::Point top(
                    (double) XBorder + j * gridSpan + (i - 1) * regionSpan,
                    (double) YBorder);
            cFigure::Point bottom(
                    (double) XBorder + j * gridSpan + (i - 1) * regionSpan,
                    (double) mapYMax);
            line->setStart(top);
            line->setEnd(bottom);
            line->setLineColor(cFigure::BLUE);
            line->setLineStyle(cFigure::LINE_DASHED);
            line->setLineWidth(1);
            line->setVisible(true);
            getParentModule()->getCanvas()->addFigure(line);
        }
        auto line = new cLineFigure("line");
        cFigure::Point top((double) XBorder + i * regionSpan, (double) YBorder);
        cFigure::Point bottom((double) XBorder + i * regionSpan,
                (double) mapYMax);
        line->setStart(top);
        line->setEnd(bottom);
        line->setLineColor(cFigure::BLUE);
        line->setLineWidth(2);
        line->setVisible(true);
        getParentModule()->getCanvas()->addFigure(line);
    }
    int regionCols = (int) ceil((double) YMax / regionSize);
    int gridCols = regionSize / gridSize - 1;
    for (int i = 1; i <= regionCols; i++) {
        // draw grid borders
        for (int j = 1; j <= gridCols; j++) {
            auto line = new cLineFigure("line");
            cFigure::Point top((double) XBorder,
                    (double) YBorder + j * gridSpan + (i - 1) * regionSpan);
            cFigure::Point bottom((double) mapXMax,
                    (double) YBorder + j * gridSpan + (i - 1) * regionSpan);
            line->setStart(top);
            line->setEnd(bottom);
            line->setLineColor(cFigure::BLUE);
            line->setLineStyle(cFigure::LINE_DASHED);
            line->setLineWidth(1);
            line->setVisible(true);
            getParentModule()->getCanvas()->addFigure(line);
        }
        auto line = new cLineFigure("line");
        cFigure::Point top((double) XBorder, (double) YBorder + i * regionSpan);
        cFigure::Point bottom((double) mapXMax,
                (double) YBorder + i * regionSpan);
        line->setStart(top);
        line->setEnd(bottom);
        line->setLineColor(cFigure::BLUE);
        line->setLineWidth(2);
        line->setVisible(true);
        getParentModule()->getCanvas()->addFigure(line);
    }
}

void Coordinator::handleMessage(cMessage *msg) {
}

long Coordinator::randomX() {
    return (long) uniform((double) 0, (double) XMax);
}

long Coordinator::randomY() {
    return (long) uniform((double) 0, (double) YMax);
}

Coordinate Coordinator::randomLocation() {
    Coordinate c;
    c.x = randomX();
    c.y = randomY();
    return c;
}

Coordinate Coordinator::centerLocation() {
    Coordinate c;
    c.x = XMax / 2;
    c.y = YMax / 2;
    return c;
}

Coordinate Coordinator::getRegion(Coordinate coord) {
    return Coordinate(coord.x / regionSize * regionSize,
            coord.y / regionSize * regionSize);
}

Coordinate Coordinator::getGrid(Coordinate coord) {
    return Coordinate(coord.x / gridSize * gridSize,
            coord.y / gridSize * gridSize);
}

vector<Coordinate> Coordinator::neighborRegions(Coordinate region) {
    vector<Coordinate> neighbors;
    /*
     * four edges
     */
    if (region.x + regionSize < XMax) {
        Coordinate right(region.x + regionSize, region.y);
        neighbors.push_back(right);
    }
    if (region.x > 0) {
        Coordinate left(region.x - regionSize, region.y);
        neighbors.push_back(left);
    }
    if (region.y + regionSize < YMax) {
        Coordinate upper(region.x, region.y + regionSize);
        neighbors.push_back(upper);
    }
    if (region.y > 0) {
        Coordinate lower(region.x, region.y - regionSize);
        neighbors.push_back(lower);
    }

    /*
     * four corners
     */
    if (region.y + regionSize < YMax && region.x > 0) {
        Coordinate upperLeft(region.x - regionSize, region.y + regionSize);
        neighbors.push_back(upperLeft);
    }
    if (region.y + regionSize < YMax && region.x + regionSize < XMax) {
        Coordinate upperRight(region.x + regionSize, region.y + regionSize);
        neighbors.push_back(upperRight);
    }
    if (region.y > 0 && region.x > 0) {
        Coordinate lowerLeft(region.x - regionSize, region.y - regionSize);
        neighbors.push_back(lowerLeft);
    }
    if (region.y > 0 && region.x > 0) {
        Coordinate lowerRight(region.x + regionSize, region.y - regionSize);
        neighbors.push_back(lowerRight);
    }

    return neighbors;
}

vector<Coordinate> Coordinator::neighborGrids(Coordinate grid) {
    vector<Coordinate> neighbors;
    /*
     * four edges
     */
    if (grid.x + gridSize < XMax) {
        Coordinate right(grid.x + gridSize, grid.y);
        neighbors.push_back(right);
    }
    if (grid.x > 0) {
        Coordinate left(grid.x - gridSize, grid.y);
        neighbors.push_back(left);
    }
    if (grid.y + gridSize < YMax) {
        Coordinate upper(grid.x, grid.y + gridSize);
        neighbors.push_back(upper);
    }
    if (grid.y > 0) {
        Coordinate lower(grid.x, grid.y - gridSize);
        neighbors.push_back(lower);
    }

    return neighbors;
}

Coordinate Coordinator::mapLocation(Coordinate c) {
    Coordinate ml;
    ml.x = (long) ((double) c.x / XMax * (mapXMax - XBorder) + XBorder);
    ml.y = (long) ((double) c.y / YMax * (mapYMax - YBorder) + YBorder);
    return ml;
}

double Coordinator::mapValue(long size) {
    return (mapXMax - XBorder) / (double) XMax * size;
}

long distance(Coordinate a, Coordinate b) {
    return (long) hypot(a.x - b.x, a.y - b.y);
}
