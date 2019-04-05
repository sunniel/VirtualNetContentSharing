# Experiment Results

## 1. Communication Overhead  
The first experiment evaluates the number of hops per routing as a function of the number of objects. In the experiment, the number of objects in a virtual world is scaled from 100 to 500, and randomly distributed on the map. Though, only the objects located within the range of a region are retrieved, increasing the total number of objects also proportionally changes the number of objects within a region with high probability. The configurations are listed in the following table.

| Parameters                     | Values    |
|--------------------------------|-----------|
| Simulation time                | 5000s     |
| CAN overlay size               | 100 nodes |
| Chord                          | 100 nodes |
| Node dynamics cycle            | 10s       |
| Node join rate                 | 0.1       |
| Node departure rate            | 0.1       |
| Node maintenance   cycle       | 4s        |
| Packet delay (RTT)             | 100ms     |
| Node bandwidth                 | 10Mbps    |
| Average object size            | 4.5MB     |
| MTU                            | 1518B     |
| Map size (X/Y maximum)		 | 900000000m|
| Region size (R)      			 | 18000000m |
| Search Radius (S)              | 0.707 * R |
| Client content   refresh cycle | 10s       |
| Client movement   rate         | 10s       |
| Client movement   speed        | 200000m/s |

Figure 1 and Figure 2 shows the experiment result. Figure 1 describes the content retrieval process without addressing bot dynamics, while Figure 2 describes the process with addressing bot dynamics. It can be found that, the communication overhead increases when addressing bot dynamics is applied, because some cached addressing bots are no longer in charge of the specified objects. Thus, searching over the Chord overlay has to be performed, which increases communication overhead. In both Figures, however, the communication overhead in the improved content retrieval scheme is much lower than that in the basic content retrieval scheme, showing the effectiveness of the proposed strategies. Moreover, the communication cost increases in the improved scheme is also slower than that in the basic content retrieval scheme, meaning that the improved scheme is optimal for virtual world content sharing.

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Communication%20Overhead%20without%20Churn.png)  
Figure 1. Number of hops of routing versus number of objects without Address bot dynamics

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Communication%20Overhead%20with%20Churn.png)  
Figure 2. Number of hops of routing versus number of objects with Address bot dynamics

## 2. Perceived Content Load Delay

The second experiment evaluates the user perceived delay of content load. Since virtual objects may contains large multimedia files, in this test, they are downloaded one after another. The experiment contains three models. The proposed model downloads the objects follows the sequence of proximate-based content discovery. The basic model follows the sequence of contents listed in the cross-region content inventory. Moreover, a third model is also compared, which calculates the distance of each object on the inventory to the client position and downloads the objects in the ascending order of this geographical distance. The configurations are listed in the following table.

| Parameters                     | Values    |
|--------------------------------|-----------|
| Simulation time                | 5000s     |
| CAN overlay size               | 100 nodes |
| Chord                          | 100 nodes |
| Node dynamics cycle            | 10s       |
| Node join rate                 | 0.1       |
| Node departure rate            | 0.1       |
| Node maintenance   cycle       | 4s        |
| Packet delay (RTT)             | 100ms     |
| Node bandwidth                 | 10Mbps    |
| Average object size            | 4.5MB     |
| MTU                            | 1518B     |
| Map size (X/Y maximum)		 | 900000000m|
| Region size (R)      			 | 18000000m |
| Search Radius (S)              | 0.707 * R |
| Client content   refresh cycle | 10s       |
| Client movement   rate         | 10s       |
| Client movement   speed        | 200000m/s |

Figure 3 and Figure 4 shows the experiment result. Figure 3 describes the content retrieval process without addressing bot dynamics, while Figure 4 describes the process with addressing bot dynamics. It is obvious that users can perceive less content retrieval delay in the proximate-based content download strategy than in the inventory-based stragtegy (i.e., the basic model), because, in  the former strategy, more objects within the user perception range can be retrieved and loaded to the screen first, reducing the time of waiting in play. In this experiment, the proposed content retrieval approach shows the similar  performance to the third model (i.e., the augmented basic approach with distance-based content retrieval). To this point, it seems that the third model is simpler than the proposed model. However, the last experiment shows that the basic model will generate more communication overhead than the proposed model. Thus, combined the two results, the proposed model is optimal.

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Perceived%20Content%20Retrieval%20Delay%20without%20Churn.png)  
Figure 3. Content Retrieval Time versus number of objects without Address bot dynamics

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Perceived%20Content%20Retrieval%20Delay%20with%20Churn.png)  
Figure 4. Content Retrieval Time versus number of objects with Address bot dynamics

## 3. Load Distribution

The third experiment increases the experiment time 10,0000 cycles to study the load distribution on region bots as a function of time. The load on each region bot is quantified and measured by the number of routing request handling and forwarding during a certain period. In the experiment, region bot load is sampled in every 100 cycles so that 100 samples can be collected in total. 

| Parameters                     | Values    |
|--------------------------------|-----------|
| Simulation time                | 10000s    |
| CAN overlay size               | 100 nodes |
| Chord                          | 100 nodes |
| Node dynamics cycle            | 10s       |
| Node join rate                 | 0.1       |
| Node departure rate            | 0.1       |
| Node maintenance   cycle       | 4s        |
| Packet delay (RTT)             | 100ms     |
| Node bandwidth                 | 10Mbps    |
| Average object size            | 4.5MB     |
| MTU                            | 1518B     |
| Number of objects              | 2000      |
| Map size (X/Y maximum)		 | 900000000m|
| Region size (R)      			 | 18000000m |
| Search Radius (S)              | 0.707 * R |
| Max. distance of object cache	 | 4 * S 	 |
| Client content   refresh cycle | 10s       |
| Client movement   rate         | 10s       |
| Client movement   speed        | 200000m/s |

Figure 5 and Figure 6 show the trend of the mean load per region bot without and with addressing bot dynamics respectively. Figure 6 contains more fluctuation than Figure 5, because more communication hops have to be reached when the search paths are broken due to node failure. Both Figures show that caching the nearby objects in the local storage can reduce the load imposed on remote nodes in query, which is achieved by the region-based content inventory. By comparing the retrieved content inventory with the local one, the up-to-date object resource files that are already in the local cache can be removed from the download list, reducing the need for object re-download.

The results also show that both the mean value and the greatest value of region bot load are large at the beginning of the simulation. With the increase of cycles, load starts decreasing. This is because, with the increase of routing process, more objects are locally cached, unchanged, and do not need to be downloaded again. This result validates the effectiveness of the second strategy such that utilizing local cache and content inventory can reduce the number of content retrieval. Moreover, Figure 5 and Figure 6 have the similar trends, showing that region bot dynamics does not have large impact on the performance.

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Load%20Distribution%20without%20Churn.png)  
Figure 5. Region bot load versus number of cycles without addressing  bot dynamics

![Number of hops of routing versus number of objects without Address bot dynamics!](https://github.com/sunniel/VirtualNetContentSharing/blob/master/Experiment%20Results/Load%20Distribution%20with%20Churn.png)  
Figure 6. Region bot load versus number of cycles with addressing  bot dynamics