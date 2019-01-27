# Introduction

## Overview
This repository contains the demo codes for the paper "Efficient Peer-to-peer Content Sharing for Learning in Virtual Worlds".

The project simulates the object sharing in a decentralized virtual world and evaluate the efficiency of the proposed solution. The entire idea is: Assume user-created virtual objects are stored on the logical computers [1] of their users. A *Chord* [2] overlay, composed of reliable peer nodes, manages the mapping between object identifier and their actual network location of storage (i.e., the network address of logical computer nodes). To reduce content search overhead, a *CAN* overlay [3] is maintained by other reliable peer nodes, which maintains the mapping of object geographical location and their network storage location. It also maintains content inventories by region. Since the mapping between the logical computer ID and the actual network location and the mapping between the geographical location and the actual network location have to be incompatibly maintained over the network, the Chord layer has to be retained. Thus, two overlays are maintained in the design, and the mapping is

Object geographical locations -> CAN overlay -> Chord host network addresses -> Chord overlay -> logical computer nodes

## Simulation Components

1. **Location Mapping Service**, i.e., the CAN overlay:
	1) maintains the mapping between object geographical locations and the chord host network addresses;
	2) queries the Chord overlay for mapping update due to chord overlay node dynamics;
	3) virtual world content inventory of each virtual world region.
	
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

## Experiment results

Detailed experiment results can be found in the Experiment Results folder.

Simulation environment and 3rd-party dependency: 

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