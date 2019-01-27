# Experiment Results

## 1. Communication Overhead  
The first experiment evaluates the number of hops per routing as a function of the number of objects. In the experiment, the number of objects in a virtual world is scaled from 100 to 500, and randomly distributed on the map. Though, only the objects located within the range of a region are retrieved, increasing the total number of objects also proportionally changes the number of objects within a region with high probability. Configurations are listed in the following table.

| Parameters                     | Values    |
|--------------------------------|-----------|
| Simulation time                | 5000s     |
| CAN overlay size               | 100 nodes |
| Chord                          | 100 nodes |
| Node dynamics cycle            | 10s       |
| Node join rate                 | 0         |
| Node departure rate            | 0         |
| Node maintenance   cycle       | 4s        |
| Client content   refresh cycle | 10s       |
| Client movement   rate         | 10s       |
| Client movement   speed        | 200000m/s |
| Packet delay (RTT)             | 100ms     |
| Node bandwidth                 | 10Mbps    |
| Average object size            | 4.5MB     |
| MTU                            | 1518B     |

The following figures shows the experiment result. The first figure describes the content retrieval process without addressing bot dynamics, while the second figure describes the process with addressing bot dynamics. It can be found that, the communication overhead increases when addressing bot dynamics is applied, because some cached addressing bots are no longer in charge of the specified objects. Thus, searching over the Chord overlay has to be performed, which increases communication overhead. In both Figures, however, the communication overhead in the improved content retrieval scheme is much lower than that in the basic content retrieval scheme, showing the effectiveness of the proposed strategies. Moreover, the communication cost increases in the improved scheme is also slower than that in the basic content retrieval scheme, meaning that the improved scheme is optimal for virtual world content sharing.

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Communication%20Overhead%20without%20Churn.png)  
Figure 1. Number of hops of routing versus number of objects without Address bot dynamics

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Communication%20Overhead%20with%20Churn.png)  
Figure 2. Number of hops of routing versus number of objects with Address bot dynamics

## 2. Perceived Content Load Delay

Configurations are listed in the following table.

| Parameters                     | Values    |
|--------------------------------|-----------|
| Simulation time                | 5000s     |
| CAN overlay size               | 100 nodes |
| Chord                          | 100 nodes |
| Node dynamics cycle            | 10s       |
| Node join rate                 | 0         |
| Node departure rate            | 0         |
| Node maintenance   cycle       | 4s        |
| Client content   refresh cycle | 10s       |
| Client movement   rate         | 10s       |
| Client movement   speed        | 200000m/s |
| Packet delay (RTT)             | 100ms     |
| Node bandwidth                 | 10Mbps    |
| Average object size            | 4.5MB     |
| MTU                            | 1518B     |

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Perceived%20Content%20Retrieval%20Delay%20without%20Churn.png)  
Figure 3. Content Retrieval Time versus number of objects without Address bot dynamics

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Perceived%20Content%20Retrieval%20Delay%20with%20Churn.png)  
Figure 4. Content Retrieval Time versus number of objects with Address bot dynamics

## 3. Load Distribution