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

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <climits>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include <cstdarg>
#include <cstringtokenizer.h>
#include <cenvir.h>
#include "../crypto/sha1.h"

using namespace std;
using namespace omnetpp;

namespace util {

string getHostName(string& fullName);

int getHostIndex(string& fullName);

int strToInt(string& str);
unsigned long strToLong(string& str);

string intToStr(int i);
string longToStr(unsigned long i);

void splitString(string& str, string delimitor, vector<string>& container);

/* Convert a string into an unsigned long in [0, spacesize] */
unsigned long getSHA1(string str, int spacesize);

template<class K, class V>
typename multimap<K, V>::const_iterator multimap_find(const multimap<K, V>& map,
        const pair<K, V>& pair) {
    typedef typename multimap<K, V>::const_iterator it;
    std::pair<it, it> range = map.equal_range(pair.first);
    for (it p = range.first; p != range.second; ++p)
        if (p->second == pair.second)
            return p;
    return map.end();
}
// insert a pair into a multimap if it does not exist
template<class K, class V>
bool multimap_insert(multimap<K, V>& map, const pair<K, V>& pair) {
    if (multimap_find(map, pair) == map.end()) {
        map.insert(pair);
        return true;
    }
    return false;
}

}

#endif /* UTIL_H_ */
