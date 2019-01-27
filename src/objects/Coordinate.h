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

#ifndef OBJECTS_COORDINATE_H_
#define OBJECTS_COORDINATE_H_

#include <iostream>

using namespace std;

class Coordinate {
public:
    long x = 0;
    long y = 0;

    Coordinate() {
        this->x = 0;
        this->y = 0;
    }

    Coordinate(long x, long y) {
        this->x = x;
        this->y = y;
    }

    string str() const {
        return to_string(x) + "," + to_string(y);
    }

    friend ostream& operator<<(ostream& os, const Coordinate& c) {
        os << c.str();
        return os;
    }
};

struct CoordntCompare {
    bool operator()(const Coordinate& lhs, const Coordinate& rhs) const {
        return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
    }
};

#endif /* OBJECTS_COORDINATE_H_ */
