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

#include "HostInfo.h"

HostInfo::~HostInfo() {
    ;
}

HostInfo::HostInfo(int moduleId) :
        SimpleInfo(moduleId) {
    id = 0L;
}

HostInfo::HostInfo(int moduleId, string moduleName) :
        SimpleInfo(moduleId, moduleName) {
    id = 0L;
}

/**
 * setter and getter
 */
void HostInfo::setHostId(unsigned long id) {
    this->id = id;
}
unsigned long HostInfo::getHostId() {
    return id;
}
