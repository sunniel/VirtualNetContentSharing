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

#ifndef CONSTANTS_H_
#define CONSTANTS_H_
// message names
namespace msg {
// Chord message types
extern const char* CHORD_LOOK_UP;
extern const char* CHORD_FINAL;
extern const char* CHORD_SUCCESSOR;
extern const char* CHORD_QUERY_SUCCESSOR;
extern const char* CHORD_SUCCESSOR_FOUND;
extern const char* CHORD_NOTIFY;
extern const char* CHORD_STORE;
extern const char* CHORD_GET;
extern const char* CHORD_REPLICATE;
extern const char* CHORD_FIX_REPLICA;
extern const char* CHORD_UPDATE_PREDECESSOR;
extern const char* CHORD_REPLY;

// Chord schedules
extern const char* CHORD_MAINT;

// CAN message types
extern const char* CAN_LOOK_UP;
extern const char* CAN_FINAL;
extern const char* CAN_JOIN;
extern const char* CAN_JOIN_REPLY;
extern const char* CAN_JOIN_REPLY_FAIL;
extern const char* CAN_ADD_NEIGHBOR;
extern const char* CAN_RM_NEIGHBOR;
extern const char* CAN_UPDATE_AREA;
extern const char* CAN_NEIGHBOR_UPDATE;
extern const char* CAN_NEIGHBOR_TAKEOVER;
extern const char* CAN_NEIGHBOR_EXCHANGE;
extern const char* CAN_ERS;
extern const char* CAN_ERS_REPLY;
extern const char* CAN_REPLICATE;
extern const char* CAN_FIX_REPLICA;
extern const char* CAN_FIX_LOAD;
extern const char* CAN_INVENTORY_REPLY;
extern const char* CAN_CONTENT_REPLY;
extern const char* CAN_MAPPING_QUERY;

// global schedules
extern const char* INIT_OVERLAYS;
extern const char* CHURN_CHORD_CYCLE;
extern const char* CHURN_CAN_CYCLE;
extern const char* MANUAL_QUERY;

// Chord schedules
extern const char* CAN_MAINT;
extern const char* CAN_LOAD_SAMPLE;

// client schedules
extern const char* REFRESH_CONTENT;
extern const char* REFRESH_POSITION;
extern const char* CLIENT_START;
extern const char* TIMEOUT;

// message labels
extern const char* LABEL_INIT;
extern const char* LABEL_TEST;
extern const char* LABEL_INVENTORY;
extern const char* LABEL_CONTENT;
extern const char* LABEL_OBJECT;
}

namespace data{
// chord data type
extern const char* DATA_EMPTY;
extern const char* DATA_TYPE_OBJECT;
extern const char* DATA_TYPE_INVENTORY;
}
#endif /* CONSTANTS_H_ */
