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

package hosts;

import others.*;
import transport.*;
import app.*;
import ctrl.*;

module CAN
{
    parameters:
        @display("i=block/buffer2");
        //volatile double lifespan @unit(s);
    gates:
        inout udpgate @directIn;
        inout tcpgate @directIn;
    submodules:
        app: CANApp {
            parameters:
                @display("p=82,80");
        }
        ctrl: CANCtrl {
            parameters:
                @display("p=176,80");
        }
        udp: SimpleUDP {
            parameters:
                @display("p=252, 80");
        }
        tcp: ReliableUDP {
            parameters:
                @display("p=252, 160");
        }        
//        interfaceTable: InterfaceTable {
//            parameters:
//                @display("p=82,160");
//        }
    connections:
        ctrl.uplink$o --> app.link$i;
        ctrl.uplink$i <-- app.link$o;
        udp.uplink$o --> ctrl.udplink$i;
        udp.uplink$i <-- ctrl.udplink$o;
        tcp.uplink$o --> ctrl.tcplink$i;
        tcp.uplink$i <-- ctrl.tcplink$o;
        udpgate$i --> udp.downlink$i;
        udpgate$o <-- udp.downlink$o;
        tcpgate$i --> tcp.downlink$i;
        tcpgate$o <-- tcp.downlink$o;
}
