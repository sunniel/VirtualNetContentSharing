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

#ifndef GLOBAL_MANUALVALUEQUERY_H_
#define GLOBAL_MANUALVALUEQUERY_H_

#include <string>
#include "../common/Constants.h"
#include "../common/Util.h"
#include "../objects/IPvXAddress.h"
#include "UnderlayConfiguratorAccess.h"
#include "GlobalNodeListAccess.h"

using namespace std;

class ManualValueQuery: public cSimpleModule {
private:
    simtime_t scan_cycle;
    cMessage* scan;
    string id;
    string coordinate;
protected:
    void initialize();
    void handleMessage(cMessage *msg);
public:
    ManualValueQuery();
    virtual ~ManualValueQuery();
};

#endif /* GLOBAL_MANUALVALUEQUERY_H_ */
