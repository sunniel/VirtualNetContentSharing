//
// Copyright (C) 2006 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

//
// @author Ingmar Baumgart
//

package global;

//
// Module for storing global simulation parameters
//
simple GlobalParameters
{
    parameters:
        @display("i=block/control");
        double max_distance @unit(m);
        double fix_latency @unit(s);
        int latency_gen_mode;
        int address_space_size;
        
       	int chord_init_size;
        int successor_list_size;
        int chord_replica_size;

        int can_init_size;
        int can_area_size;
        int max_hop; 
        int ers_threshold;
}
