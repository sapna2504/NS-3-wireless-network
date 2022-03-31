# NS-3-wireless-network


In this I have created a topology where two personal computers n1 and n2 are connected through a LAN with a network id 10.1.2.0.
The five other devices are connected over WiFi where n0 acts as an AP with a network id 10.1.3.0
The n0 (router or AP) and the n1 csma node are connected through a point to point link with network id 10.1.1.0
The topology is shown below:


![Alt text](https://github.com/sapna2504/NS-3-wireless-network/blob/main/topology.png) 
        
The different scenarios are emulated below:

_**1. Only one PC is running and no other device is running**._
    I have attached a file named assignment2a.cc where only one PC n1 is running and no other device is running. 
    In this case the AP acts as a sink or receiver and n1 is a sender which is running out of all available devices.
    The parameters like bandwidth was 12Mbps and delay was 6ms for the simulation. The simulation time was 15 seconds.
    The number of csma nodes were 1 and number of wifi nodes were 5, The point to point node n1 is later added into
    csma and so the number of nodes becomes 2 and these 2 nodes are PC. 
    We set the attribute of point to point link as bandwidth = 10Mbps and Delay = 4ms. The sinkport is 8080.
    For the source node the packet size was 512 in my case and the number of packets were 100 and the data rate was
    100Mbps.
    
**_2. Both PC’s are running, and no other devices are running_**
    I have attached a file named assignment2b.cc where only two PC n1 and n2 are running. The sink node is the n1 device in this case and no other wifi device is running at this time. The simulation bandwidth = 5Mbps and delay = 5ms. Also, the packet size in this case is 1024 and the number of packets = 100 and the data rate is 100Mbps.
    
**_3. Both PC’s are running, and 3 other devices are running_**
     I have attached a file named assignment2c.cc where only two PC n1 and n2 are running and all other 3 wifi devices are also running at the same time. The sink node is the n1 device in this case. The simulation bandwidth = 5Mbps and delay = 5ms.The simulation time is 15sec. Also, the packet size in this case is 1024 and the number of packets = 100 and the data rate is 100Mbps.
 
 **_4. All devices are running_**
     I have attached a file named assignment2d.cc where only two PC n1 and n2 are running and all other 5 wifi devices are also running at the same time. The sink node is the n1 device in this case. The simulation bandwidth = 5Mbps and delay = 5ms.The simulation time is 15sec. Also, the packet size in this case is 1024 and the number of packets = 100 and the data rate is 100Mbps


