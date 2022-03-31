#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                        AP
//  *  *   *    *    *    *
//  |  |   |    |    |    |   10.1.1.0
// n3  n4  n5  n6   n7   n0 -----------n1    n2   n3
//                             P2P     |     |    |
//                                    =============
//                                     LAN 10.1.2.0
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Assignment2");

class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

// static void
// CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
// {
//   //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
//   *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
// }
int
main (int argc, char *argv[])
{

  //change these parameters for different simulations
  std::string bandwidth = "5Mbps";
  std::string delay = "5ms";
  //double error_rate = 0.000001;
  // int queuesize = 10; //packets
  int simulation_time = 15; //seconds
  bool verbose = true;
  uint32_t nCsma = 1;
  uint32_t nWifi = 5;
  bool tracing = true;

  //TCP variant set to NewReno
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));

  //set qsize
  //Config::SetDefault ("ns3::PfifoFastQueueDisc::SetMaxSize", UintegerValue(queuesize));
  Config::SetDefault ("ns3::QueueBase::MaxSize", StringValue ("10p"));
  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  // Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  // em->SetAttribute ("ErrorRate", DoubleValue (error_rate));
  // p2pDevices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  NetDeviceContainer csmaDevices;
  CsmaHelper csma;
  csmaDevices = csma.Install (csmaNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  NetDeviceContainer staDevices;
  WifiHelper wifi;
  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());
  staDevices = wifi.Install (phy, mac, wifiStaNodes);
  // Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);

  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices);
  address.Assign (apDevices);

  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (csmaInterfaces.GetAddress (0), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (csmaNodes.Get (0));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (simulation_time));
  
  Ptr<Socket> ns3TcpSocketc1 = Socket::CreateSocket (csmaNodes.Get (1), TcpSocketFactory::GetTypeId ());
  //Ptr<Socket> ns3TcpSocketc2 = Socket::CreateSocket (csmaNodes.Get (1), TcpSocketFactory::GetTypeId ());
  
  Ptr<MyApp> appc1 = CreateObject<MyApp> ();
  //Ptr<MyApp> appc2 = CreateObject<MyApp> ();
  
  appc1->Setup (ns3TcpSocketc1, sinkAddress, 1024, 100, DataRate ("100Mbps"));
  //appc2->Setup (ns3TcpSocketc2, sinkAddress, 1024, 100, DataRate ("100Mbps"));
  
  csmaNodes.Get (1)->AddApplication (appc1);
  //csmaNodes.Get (1)->AddApplication (appc2);
  
  appc1->SetStartTime (Seconds (1.));
  appc1->SetStopTime (Seconds (simulation_time));

  //appc2->SetStartTime (Seconds (1.));
  //appc2->SetStopTime (Seconds (simulation_time));

  
  

  if (tracing == true)
    {
      //pointToPoint.EnablePcapAll ("a2c");
      //phy.EnablePcap ("a2c_wifi_1", staDevices.Get (0), true);
      csma.EnablePcap ("a2b_csma_0", csmaDevices.Get (1), true);
      //csma.EnablePcap ("a2b_csma_1", csmaDevices.Get (1), true);
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //trace cwnd
  // AsciiTraceHelper asciiTraceHelper;
  // Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("tcp-example.cwnd");
  // ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

  // //detailed trace of queue enq/deq packet tx/rx
  // AsciiTraceHelper ascii;
  // pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("tcp-example.tr"));
  // pointToPoint.EnablePcapAll ("tcp-example");

  Simulator::Stop (Seconds (simulation_time));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;





}