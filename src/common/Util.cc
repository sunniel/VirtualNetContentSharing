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

#include "Util.h"

string util::getHostName(string& fullName) {
    int end = fullName.find("[");
    string hostName = fullName.substr(0, end);
    return hostName;
}

int util::getHostIndex(string& fullName) {
    int i = -1;
    size_t start = fullName.find("[") + 1;
    size_t end = fullName.find("]");
    if (start != string::npos && end != string::npos) {
        size_t range = end - start;
        string index = fullName.substr(start, range);
        if (!index.empty()) {
            i = util::strToInt(index);
        }
    }
    return i;
}

int util::strToInt(string& str) {
    int value = INT_MIN;
    stringstream ss(str);
    ss >> value;
    return value;
}

unsigned long util::strToLong(string& str) {
    unsigned long value = INT_MIN;
    stringstream ss(str);
    ss >> value;
    return value;
}

string util::intToStr(int i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

string util::longToStr(unsigned long i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

void util::splitString(string& str, string delimitor,
        vector<string>& container) {
    cStringTokenizer tokenizer(str.c_str(), delimitor.c_str());
    while (tokenizer.hasMoreTokens()) {
        const char* token = tokenizer.nextToken();
//        EV << "entries contains: " << token << endl;
        container.push_back(token);
    }
}

unsigned long util::getSHA1(string str, int spacesize) {
    SHA1 *sha1 = new SHA1();
    sha1->addBytes(str.c_str(), strlen(str.c_str()));
    unsigned char* digest = sha1->getDigest();
    unsigned long res = (unsigned long) sha1->shaToInteger(digest, 20,
            pow(2, spacesize));
    delete sha1;
    free(digest);
    return res;
}
