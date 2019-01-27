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

#ifndef OBJECTS_CANPROFILE_H_
#define OBJECTS_CANPROFILE_H_

#include <algorithm>
#include "SimpleInfo.h"
#include "../common/Util.h"
#include "../global/GlobalParametersAccess.h"

using namespace std;

struct position {
    int x;
    int y;

    position() {
        this->x = 0;
        this->y = 0;
    }
    position(int x, int y) {
        this->x = x;
        this->y = y;
    }

    string str() const {
        return to_string(x) + "," + to_string(y);
    }

    friend ostream& operator<<(ostream& os, const position& p) {
        os << p.str();
        return os;
    }
};

struct compare {
    bool operator()(const position& lhs, const position& rhs) const {
        return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
    }
};

class CANProfile {
    unsigned long id;
    vector<char> vid;
    // x0, x1, y0, y1 - the coordinates of four corners
    vector<int> area;
public:
    CANProfile();
    CANProfile(unsigned id);
    CANProfile(unsigned id, vector<int> area);
    virtual ~CANProfile();

    /**
     * setter and getter
     */
    void setId(unsigned long id);
    unsigned long getId() const;
    void setVId(string vid);
    string getVId();
    void setArea(vector<int> a);
    vector<int> getArea() const;

    bool isEmpty();

    /*
     * adds the parameter as last value.
     *
     */
    void addToVID(char newVID);

    void setArea(int x0, int x1, int y0, int y1);
    void setX0(int x0);
    void setX1(int x1);
    void setY0(int y0);
    void setY1(int y0);

    CANProfile dup();

    bool operator<(const CANProfile& info) const;
    bool operator==(const CANProfile& info) const;
    bool operator!=(const CANProfile& info) const;

    friend ostream& operator<<(ostream& os, const CANProfile& profile);
};

struct lex_compare {
    bool operator()(const CANProfile& lhs, const CANProfile& rhs) const {
        return lhs < rhs;
    }
};

/*
 * Compares two CanVID and gives the number of common values back.
 * So it gives the number of common parents
 */
extern int numberCommon(vector<char> id, vector<char> compare);

/*
 * Compares two CanVID and if all value until the n-1 number are common
 * the return value is true.
 * So it is true, if all parents are common
 */
extern bool closestNeighbour(vector<char> id, vector<char> compare);

/*
 * gives a List of all common values.
 */
extern vector<char> listCommon(vector<char> id, vector<char> compare);

/*
 * checks if the x edge of area has common part with the betweenArea
 */
extern bool betweenXboth(vector<int> area, vector<int> betweenArea);

/*
 * checks if both y edge of area has common part with the betweenArea
 */
extern bool betweenYboth(vector<int> area, vector<int> betweenArea);
/*
 * checks if the area has at least on common edge with the
 * testArea
 */
extern bool commonEdge(vector<int> area, vector<int> testArea);

/**
 * Gives the x value of the hash. Therefore it takes the modular value of
 * the first half of the hash and CANSize
 */
extern int getXValue(position p);

/*
 * Gives the y value of the hash. Therefore it takes the modular value of
 * the second half of the hash and CANSize
 */
extern int getYValue(position p);

/**
 * Checks if the x value of the hash is in the same region as the x corner
 * of the area
 */
extern bool includeXValue(position p, vector<int> area);

/*
 * Checks if the y value of the hash is in the same region as the y corner
 * of the area
 */
extern bool includeYValue(position p, vector<int> area);

/*
 * Checks if the x value of the hash should be in this area
 */
extern bool includedInArea(position p, vector<int> area);

/*
 * check whether a rectangle (area) intercepts a circle (center, radisu)
 */
extern bool intercept(vector<int> area, position center, int radius);

/*
 * split the given zone into two adjacent zones, no cross-border zone split
 */
extern vector<vector<int>> splitZone(vector<int> area);

#endif /* OBJECTS_CANPROFILE_H_ */
