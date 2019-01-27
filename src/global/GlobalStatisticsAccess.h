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

#ifndef GLOBAL_GLOBALSTATISTICSACCESS_H_
#define GLOBAL_GLOBALSTATISTICSACCESS_H_

#include <omnetpp.h>
#include "GlobalStatistics.h"

class GlobalStatisticsAccess {
public:
    GlobalStatistics* get() {
        return (GlobalStatistics*) getSimulation()->getModuleByPath(
                "globalObserver.globalStatistics");
    }
};

#endif /* GLOBAL_GLOBALSTATISTICSACCESS_H_ */
