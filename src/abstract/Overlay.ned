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

package abstract;

import global.*;
import hosts.*;

network Overlay
{
    parameters:
        @display("bgb=1500,1100");
        @statistic[NodeLoad](source=load; title="Quantified Node Load"; record=vector);
        @statistic[HopNumber](source=hop; title="Number of Hops for Content Retrieval"; record=stats);
        @statistic[LoadDuration](source=duration; title="Content Load Duration"; record=stats);
        double pktLossRate;
        double Copt @unit(mps);
        double BW @unit(bps);
        double MTU @unit(b);
        double jitter_mu;
        double jitter_sigma;
        double Dmin @unit(s);
        volatile double jitter @unit(s);
    submodules:
        chordOverlay: ChordOverlay {
            @display("p=500,70;t=Object Resource Lookup Service");
        }
        canOverlay: CANOverlay {
            @display("p=800,70;t=Location Mapping Service");
        }
        globalObserver: GlobalObserver {
            @display("p=100,100;t=Global Observers");
        }
        underlayConfigurator: UnderlayConfigurator {
            @display("p=100,250;t=Network Configurator");
        }
        contentDistributor: ContentDistributor {
            @display("p=100,400;t=Content Distributor");
        }
        coordinator: Coordinator {
            @display("p=100,550;t=Coordinates");
        }
        churnGenerator: UniformChurnGenerator {
            @display("p=100,700;t=Churn Generator");
        }
        //        valueQuery: ManualValueQuery {
        //            @display("p=50,550");
        //        }
        client: Client {
            @display("p=300,300;t=Client");
        }
    connections allowunconnected:
}
