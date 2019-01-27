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

package global;

//
// TODO auto-generated module
//
simple UniformChurnGenerator
{
    parameters:
        @display("i=block/timer");
        double churn_chord_cycle @unit(s);
        double churn_can_cycle @unit(s);
        double depart_chord_rate;
        double depart_can_rate;
        double arrival_chord_rate;
        double arrival_can_rate;
        int overlay_size_max;
        int overlay_size_min;
}
