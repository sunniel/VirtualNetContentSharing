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

#include "CANProfile.h"

CANProfile::CANProfile() {
    area.resize(4);
}

CANProfile::CANProfile(unsigned id) {
    this->id = id;
    area.resize(4);
}

CANProfile::CANProfile(unsigned id, vector<int> area) {
    this->id = id;
    this->area = area;
}

CANProfile::~CANProfile() {
    // TODO Auto-generated destructor stub
}

void CANProfile::setId(unsigned long canId) {
    id = canId;
}
unsigned long CANProfile::getId() const {
    return id;
}

void CANProfile::setVId(string canVId) {
    vid.clear();
    vid.assign(canVId.begin(), canVId.end());
}
string CANProfile::getVId() {
    string canVId(vid.begin(), vid.end());
    return canVId;
}

void CANProfile::setArea(vector<int> a) {
    area = a;
}
vector<int> CANProfile::getArea() const {
    return area;
}

bool CANProfile::isEmpty() {
    if (area[0] == 0 && area[1] == 0 && area[2] == 0 && area[3] == 0) {
        return true;
    }
    return false;
}

void CANProfile::addToVID(char newBit) {
    vid.push_back(newBit);
}

void CANProfile::setArea(int x0, int x1, int y0, int y1) {
    area[0] = x0;
    area[1] = x1;
    area[2] = y0;
    area[3] = y1;
}

void CANProfile::setX0(int x0) {
    area[0] = x0;
}

void CANProfile::setX1(int x1) {
    area[1] = x1;
}

void CANProfile::setY0(int y0) {
    area[2] = y0;
}

void CANProfile::setY1(int y1) {
    area[3] = y1;
}

CANProfile CANProfile::dup() {
    CANProfile profile;
    profile.setId(id);
    profile.setArea(area);
    return profile;
}

bool CANProfile::operator<(const CANProfile& info) const {
//    return getId() < info.getId();
    return area[0] < info.area[0]
            || (area[0] == info.area[0] && area[1] < info.area[1])
            || (area[0] == info.area[0] && area[1] == info.area[1]
                    && area[2] < info.area[2])
            || (area[0] == info.area[0] && area[1] == info.area[1]
                    && area[2] == info.area[2] && area[3] < info.area[3])
            || (area[0] == info.area[0] && area[1] == info.area[1]
                    && area[2] == info.area[2] && area[3] == info.area[3]
                    && id < info.id);
}

bool CANProfile::operator==(const CANProfile& info) const {
    return getId() == info.getId() && area == info.area;
}

bool CANProfile::operator!=(const CANProfile& info) const {
    return getId() != info.getId() || area != info.area;
}

ostream& operator<<(ostream& os, const CANProfile& profile) {
    os << profile.id << " [" << profile.area[0] << "," << profile.area[1] << ","
            << profile.area[2] << "," << profile.area[3] << "]";
    return os;
}

int numberCommon(vector<char> id, vector<char> compare) {
    int number = 0;
    for (int i = 0; i < min(id.size(), compare.size()); i++) {
        if (id[i] != compare[i])
            break;
        number++;
    }
    return number;
}

bool closestNeighbour(vector<char> id, vector<char> compare) {
    if (id.size() != compare.size())
        return false;

    for (int i = 0; i < id.size() - 1; i++) {
        if (id[i] != compare[i])
            return false;
    }

    return true;
}

vector<char> listCommon(vector<char> id, vector<char> compare) {
    int number = 0;

    for (int i = 0; i < min(id.size(), compare.size()); i++) {
        if (id[i] == compare[i])
            number++;
    }

    vector<char> listCommon;
    for (int i = 0; i < number; i++)
        listCommon.push_back(id[i]);
    return listCommon;
}

//bool betweenXboth(vector<int> area, vector<int> betweenArea) {
//    if ((betweenArea[0] >= area[0] && betweenArea[0] < area[1]) // intersect
//    || (betweenArea[1] > area[0] && betweenArea[1] <= area[1])  // intersect
//            || (betweenArea[0] <= area[0] && betweenArea[0] > area[1]) // area cross border
//            || (betweenArea[1] < area[0] && betweenArea[1] >= area[1]) // area cross border
//            || (betweenArea[0] <= area[0] && betweenArea[1] >= area[1]) // in between
//            || (betweenArea[0] >= area[0] && betweenArea[1] <= area[1])) // in between
//        return true;
//    else
//        return false;
//}

bool betweenXboth(vector<int> area, vector<int> betweenArea) {
    if (betweenArea[1] > area[0] && betweenArea[0] < area[1])
        return true;
    else
        return false;
}

//bool betweenYboth(vector<int> area, vector<int> betweenArea) {
//    if ((betweenArea[2] >= area[2] && betweenArea[2] < area[3]) // intersect
//    || (betweenArea[3] > area[2] && betweenArea[3] <= area[3])  // intersect
//            || (betweenArea[2] <= area[2] && betweenArea[2] > area[3]) // area cross border
//            || (betweenArea[3] < area[2] && betweenArea[3] >= area[3]) // area cross border
//            || (betweenArea[2] <= area[2] && betweenArea[3] >= area[3]) // in between
//            || (betweenArea[2] >= area[2] && betweenArea[3] <= area[3])) // in between
//        return true;
//    else
//        return false;
//}

bool betweenYboth(vector<int> area, vector<int> betweenArea) {
    if (betweenArea[2] < area[3] && betweenArea[3] > area[2])
        return true;
    else
        return false;
}

//bool commonCorner(vector<int> area, vector<int> testArea) {
//    if ((testArea[1] == area[0] && (betweenYboth(testArea, area)))
//            || (testArea[0] == area[1] && (betweenYboth(testArea, area)))
//            || (testArea[3] == area[2] && (betweenXboth(testArea, area)))
//            || (testArea[2] == area[3] && (betweenXboth(testArea, area)))) {
//        return true;
//    } else {
//        return false;
//    }
//}

bool commonEdge(vector<int> area, vector<int> testArea) {
    if (((testArea[1] == area[0]    // in-between the CANSize range
            || (testArea[1] == GlobalParametersAccess().get()->getCANAreaSize()
                    && area[0] == 0)) // head-rail check
    && (betweenYboth(testArea, area)))
            || ((testArea[0] == area[1] // in-between the CANSize range
                    || (area[1]
                            == GlobalParametersAccess().get()->getCANAreaSize()
                            && testArea[0] == 0)) // head-rail check
            && (betweenYboth(testArea, area)))
            || ((testArea[3] == area[2] // in-between the CANSize range
                    || (testArea[3]
                            == GlobalParametersAccess().get()->getCANAreaSize()
                            && area[2] == 0)) // head-rail check
            && (betweenXboth(testArea, area)))
            || ((testArea[2] == area[3] // in-between the CANSize range
                    || (area[3]
                            == GlobalParametersAccess().get()->getCANAreaSize()
                            && testArea[2] == 0)) // head-rail check
            && (betweenXboth(testArea, area)))) {
        return true;
    } else {
        return false;
    }
}

//unsigned long getXValue(unsigned long value) {
//    string v = to_string(value);
//    string subV = v.substr(0, (v.length() / 2) - 1);
//    unsigned long vl = util::strToLong(subV);
//    return vl % GlobalParametersAccess().get()->getCANAreaSize();
//}

//unsigned long getXValue(unsigned long value) {
//    string v = to_string(value);
//    string subV = v.substr(0, (v.length() / 2));
//    unsigned long vl = util::strToLong(subV);
//    return vl % GlobalParametersAccess().get()->getCANAreaSize();
//}

int getXValue(position p) {
    return p.x;
}

//unsigned long getYValue(unsigned long value) {
//    string v = to_string(value);
//    string subV = v.substr((v.length() / 2), v.length());
//    unsigned long vl = util::strToLong(subV);
//    return vl % GlobalParametersAccess().get()->getCANAreaSize();
//}

int getYValue(position p) {
    return p.y;
}

//bool includeXValue(unsigned long id, vector<int> area) {
//    if (getXValue(id) >= area[0] && getXValue(id) <= area[1])
//        return true;
//    return false;
//}

bool includeXValue(position p, vector<int> area) {
    if (p.x >= area[0] && p.x <= area[1])
        return true;
    return false;
}

//bool includeYValue(unsigned long id, vector<int> area) {
//    if (getYValue(id) >= area[2] && getYValue(id) <= area[3])
//        return true;
//    return false;
//}

bool includeYValue(position p, vector<int> area) {
    if (p.y >= area[2] && p.y <= area[3])
        return true;
    return false;
}

//bool includedInArea(unsigned long id, vector<int> area) {
//    if (includeXValue(id, area) && includeYValue(id, area))
//        return true;
//
//    return false;
//}

bool includedInArea(position p, vector<int> area) {
    if (includeXValue(p, area) && includeYValue(p, area))
        return true;

    return false;
}

bool includedInRect(position p, vector<int> area) {
    if (p.y > area[2] && p.y < area[3] && p.x > area[0] && p.x < area[1])
        return true;

    return false;
}

bool includedInCircle(position p, position center, int radius) {
    int dist = hypot(p.x - center.x, p.y - center.y);
    if (dist < radius)
        return true;

    return false;
}

bool intercept(vector<int> area, position center, int radius) {
    // rectangle positions
    position lowerleft(area[0], area[2]);
    position lowerright(area[1], area[2]);
    position upperleft(area[0], area[3]);
    position upperright(area[1], area[3]);

    // circle positions
    position upper(center.x, center.y + radius);
    position left(center.x - radius, center.y);
    position lower(center.x, center.y - radius);
    position right(center.x + radius, center.y);

    if (includedInRect(upper, area) || includedInRect(lower, area)
            || includedInRect(left, area) || includedInRect(right, area)
            || includedInCircle(lowerleft, center, radius)
            || includedInCircle(lowerright, center, radius)
            || includedInCircle(upperleft, center, radius)
            || includedInCircle(upperright, center, radius))
        return true;
    return false;
}

vector<vector<int>> splitZone(vector<int> area) {
    vector<vector<int>> twoZones;
// left / lower sub-zone
    vector<int> sub1(4);
// right / upper sub-zone
    vector<int> sub2(4);
    int canSize = GlobalParametersAccess().get()->getCANAreaSize();
    int XLength =
            area[1] > area[0] ?
                    (area[1] - area[0]) : (canSize - area[0] + area[1]);
    int YLength =
            area[3] > area[2] ?
                    (area[3] - area[2]) : (canSize - area[2] + area[3]);
    if (XLength >= YLength) {
        if (XLength > 2) {
            sub1[0] = area[0];
            sub2[1] = area[1];
            sub1[2] = area[2];
            sub2[2] = area[2];
            sub1[3] = area[3];
            sub2[3] = area[3];
            sub1[1] = sub2[0] = (area[1] + area[0]) / 2;
        } else {
            return twoZones;
        }
    } else {
        if (YLength > 2) {
            sub1[2] = area[2];
            sub2[3] = area[3];
            sub1[0] = area[0];
            sub2[0] = area[0];
            sub1[1] = area[1];
            sub2[1] = area[1];
            sub1[3] = sub2[2] = (area[3] + area[2]) / 2;
        } else {
            return twoZones;
        }
    }
    twoZones.push_back(sub1);
    twoZones.push_back(sub2);
    return twoZones;
}
