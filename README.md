# Introduction

## Overview
This repository contains the demo codes for the paper "Efficient Peer-to-peer Content Sharing for Learning in Virtual Worlds".

The project simulates the object sharing in a decentralized virtual world and evaluate the efficiency of the proposed solution. The entire idea is: Assume user-created virtual objects are stored on the logical computers [1] of their users. A *Chord* [2] overlay, composed of reliable peer nodes, manages the mapping between object identifier and their actual network location of storage (i.e., the network address of logical computer nodes). To reduce content search overhead, a *CAN* overlay [3] is maintained by other reliable peer nodes, which maintains the mapping of object geographical location and their network storage location. It also maintains content inventories by region. Since the mapping between the logical computer ID and the actual network location and the mapping between the geographical location and the actual network location have to be incompatibly maintained over the network, the Chord layer has to be retained. Thus, two overlays are maintained in the design, and the mapping is

Object geographical locations -> CAN overlay -> Chord host network addresses -> Chord overlay -> logical computer nodes

## Simulation Components

1. **Location Mapping Service**, i.e., the CAN overlay:
	1. maintains the mapping between object geographical locations and the chord host network addresses;
	2. queries the Chord overlay for mapping update due to chord overlay node dynamics;
	3. virtual world content inventory of each virtual world region.
	
2. **Object Resource Lookup Service**, i.e., the Chord overlay, which maintains the mapping between logical computer ID and the network address of logical computer nodes;

3. **Client**: which runs the improved content retrieval algorithm.

Also, to facilitate simulation without compromising the design feasibility, the following components are employed:

4. **Content Distributer**: initializes the location of virtual objects on a virtual world map.

5. **Churn Generator**: adds / delete nodes for both the CAN overlay and the Chord overlay.

6. **Coordinates:** provides utility functions for coordinate-based calculation and conversion.

7. **Network Configurator**: initialize all the overlay nodes, including network address assignment.

8. **Global Observers**: includes global parameter assignment (*globalParameters*), node IP address query (*globalNodeList*), and statistical result collection (*globalStatistics*).

To simplify simulation, logical computers are not modeled in the simulation. Instead, the process is simplified to search for the Chord node maintaining the given mapping between an object and the corresponding storage location, since the communication from a chord node to a logical computer node is deterministic: i.e., one communication hop.

## Trail Generation

Moreover, a random walk algorithm is employed to generate the path for client movement, which is in the client component. Particularly, a special client variant, called TrailCtreator, stores the generated movement trails into a CSV file (under /simulations path) so that the same trail can be re-used in different tests to reduce the test result difference caused by experiment settings, especially in the load test. 

The following image illustrates a random walk for 10000 simulation cycles (i.e., 10000s). The red flag represents the virtual objects distributed on the map. The solid line represents the trail of client movement. The dashed circle represents the content discovery range, while the solid circle represents the client perception range.

![Simulation Overiew!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Overlay.png)

## Experiments

Different test cases can be switched in the /src/ctrl/ClientCtr.ned file which contains multiple @class annotations. Comment out all but the one targeted for test, instructed as follows.

1. Communication overhead test: The number of communication hops in retrieving all the virtual objects are counted in a content retrieval cycle, tested with and without network churn (i.e., set all departure/arrival rate to zero).  

	1. Client: implement the improved content retrieval strategy.
	2. BasicClient: implement the basic content retrieval strategy.
	
2. Perceived content load latency test: The time of content download is recorded from the start of a content retrieval cycle to the end of the last object downloaded:

	1. ClientDelayTest: implement the improved content retrieval strategy.
	2. BasicClientDelayTest: implement the basic content retrieval strategy.
	3. BasicClientSortDelayTest: implement the basic content retrieval strategy, but contents are retrieved in the order of their distance to the client position.
	
3. Node load test: The quantified load of CAN nodes are calculated for each specified sampling cycle.

	1. ClientLoadTest: implement the improved content retrieval strategy with load cache management
	2. ClientLoadTestNoCache: implement the improved content retrieval strategy without local cache for retrieved objects

## Simulation Configurations
The follows configurations may be changed in simulations

\# Number of virtual objects distributed on the map  
**.contentDistributor.content_num = 500  

\# Departure and arrival rate of Chord nodes and CAN nodes  
**.churnGenerator.depart_chord_rate = 0.1  
**.churnGenerator.depart_can_rate = 0.1  
**.churnGenerator.arrival_chord_rate = 0.1  
**.churnGenerator.arrival_can_rate = 0.1  

\# Average size of object files to download: 5898262B = 4.5MB  
**.chord[*].ctrl.object_size = 5898262B  

\# Cycle of content retrieval  
**.client.ctrl.load_cycle = 10000ms  

\#  Cycle of client position change  
**.client.ctrl.walk_cycle = 10000ms  

\# Distance of each consecutive walk steps (*walk_distance = N \* steps, where step = speed \* walk_cycle*)  
**.client.ctrl.walk_distance = 6000000  

\# Client walk speed  
**.client.ctrl.speed = 200000  

\# Content retrieval timeout  
**.client.ctrl.load_timeout_length = 9500ms  

\# Whether the client movement trail is read from a csv file (true) or auto-generated (false)
**.client.ctrl.trail_from_file = true  

\# Chord node maintenance cycle  
**.chord[*].ctrl.maintain_cycle = 4000ms  

\# CAN node maintenance cycle
**.can[*].ctrl.maintain_cycle = 4000ms  

## load test sampling cycle  
**.globalStatistics.sample_cycle = 100s  

\# length of simulation
sim-time-limit = 10000s

## Experiment results

Detailed experiment results can be found in the Experiment Results folder.

## Simulation environment and 3rd-party dependency: 

1. OMNet++ 5.4 or later (supporting c++ 11)

2. Boost C++ libraries (1.66.0, https://www.boost.org/)

3. JSON for Modern C++ (https://github.com/nlohmann/json)

# References:
[1] Shen, B., Guo, J., Li., L. X.: "Cost optimization in persistent virtual world design"; *Information Technology and Management*, 19, 3 (2017), 108-114.  
[2] Stoica, I., Morris, R., Karger, D., Kaashoek, M. F., Balakrishnan, H. "Chord: A scalable peer-to-peer lookup service for internet applications"; *Proc. SIGCOMM '01*, ACM, New York (2001), 149-160.  
[3] Ratnasamy, S., Francis, P., Handley, M., Karp, R., Shenker, S.: "A scalable content-addressable network"; *Proc. SIGCOMM '01*, ACM, New York (2001), 161-172.  

---

# Contacts: 
Daniel Shen [daniel.shen@connect.umac.mo](daniel.shen@connect.umac.mo), Dr. Jingzhi Guo [jzguo@umac.mo](jzguo@umac.mo)