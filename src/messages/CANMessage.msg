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

//
// TODO generated message class
//

enum CANMsgType
{
    CAN_LOOK_UP = 0;
    CAN_FINAL = 1;
    JOIN = 2;
    JOIN_REPLY = 3;
    ADD_NEIGHBOR = 4;
    RM_NEIGHBOR = 5;
	JOIN_REPLY_FAIL = 6;
	UPDATE_AREA = 7;
	NEIGHBOR_UPDATE = 8;
	NEIGHBOR_EXCHANGE = 9;
	ERS = 10;
	ERS_REPLY = 11;
	NEIGHBOR_TAKEOVER = 12;
	CAN_STORE = 13;
	CAN_GET = 14; 
	CAN_REPLICATE = 15;
	CAN_FIX_REPLICA = 16;	
	FIX_LOAD = 17;
	CAN_REPLY = 18;
}

packet CANMessage {
    int type @enum(CANMsgType);
    string label;
    unsigned long sender;
    string content;
    // hop count
    int hop;
}
