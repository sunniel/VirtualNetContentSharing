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

package ctrl;

//
// TODO auto-generated module
//
simple ClientCtrl
{
    parameters:
//        @class(Client);
        //                @class(BasicClient);
        //        @class(ClientDelayTest);
        //        		@class(BasicClientDelayTest);
        //        @class(BasicClientSortedDelayTest);
//                @class(ClientLoadTest);
                @class(ClientLoadTestNoCache);
//                @class(TrailCreator);
        @signal[hop](type="double");
        @signal[duration](type="double");
        @display("i=block/broadcast");
        //@signal[latency](type="double");
        // cycle for content load
        double load_cycle @unit(s);
        double walk_cycle @unit(s);
        double start @unit(s);
        double load_timeout_length @unit(s);
        int walk_distance;
        int speed;
        bool trail_from_file;
    gates:
        inout udplink;
        inout tcplink;
}
