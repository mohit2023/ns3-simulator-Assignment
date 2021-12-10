#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;
using namespace std;

int Packets_Dropped = 0;

NS_LOG_COMPONENT_DEFINE ("SixthScriptExample");

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off
// application is not created until Application Start time, so we wouldn't be
// able to hook the socket (now) at configuration time.  Second, even if we
// could arrange a call after start time, the socket is not public so we
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass
// this socket into the constructor of our simple application which we then
// install in the source node.
// ===========================================================================
//
class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
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

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
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

  ++m_packetsSent;
  ScheduleTx ();
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

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  // NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << " " << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd << std::endl;
}

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{
  // NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  file->Write (Simulator::Now (), p);
  Packets_Dropped++;
}

int
main (int argc, char *argv[])
{ 
  int conf;
  CommandLine cmd;
  cmd.AddValue ("conf", "Configuration", conf);
  cmd.Parse (argc, argv);
  // Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (protocol));
  
  string protocol1 = "ns3::TcpNewReno";
  string protocol2 = "ns3::TcpNewReno";
  string protocol3 = "ns3::TcpNewReno";

  if (conf == 2) {
    protocol3 += "CSE";
  }

  if (conf == 3) {
    protocol1 += "CSE";
    protocol2 += "CSE";
    protocol3 += "CSE";
  }

  NodeContainer nodes;
  nodes.Create (3);

  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("3ms"));

  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("9Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("3ms"));

  NetDeviceContainer devices1;
  devices1 = pointToPoint1.Install (nodes.Get (0), nodes.Get(2));

  NetDeviceContainer devices2;
  devices2 = pointToPoint2.Install (nodes.Get (1), nodes.Get(2));

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices1.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  devices2.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address1;
  address1.SetBase ("10.1.1.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
  Ipv4AddressHelper address2;
  address2.SetBase ("10.1.2.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);

  uint16_t sinkPort = 8080;
  Address sinkAddress1 (InetSocketAddress (interfaces1.GetAddress (1), sinkPort));
  Address sinkAddress2 (InetSocketAddress (interfaces2.GetAddress (1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (2));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (30.));

  TypeId tid1 = TypeId::LookupByName (protocol1);
  std::stringstream nodeId1;
  nodeId1 << nodes.Get (0)->GetId ();
  std::string specificNode1 = "/NodeList/" + nodeId1.str () + "/$ns3::TcpL4Protocol/SocketType";
  Config::Set (specificNode1, TypeIdValue (tid1));
  Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());

  TypeId tid2 = TypeId::LookupByName (protocol2);
  std::stringstream nodeId2;
  nodeId2 << nodes.Get (0)->GetId ();
  std::string specificNode2 = "/NodeList/" + nodeId2.str () + "/$ns3::TcpL4Protocol/SocketType";
  Config::Set (specificNode2, TypeIdValue (tid2));
  Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());

  TypeId tid3 = TypeId::LookupByName (protocol3);
  std::stringstream nodeId3;
  nodeId3 << nodes.Get (1)->GetId ();
  std::string specificNode3 = "/NodeList/" + nodeId3.str () + "/$ns3::TcpL4Protocol/SocketType";
  Config::Set (specificNode3, TypeIdValue (tid3));
  Ptr<Socket> ns3TcpSocket3 = Socket::CreateSocket (nodes.Get (1), TcpSocketFactory::GetTypeId ());

  // TypeId tid1 = TypeId::LookupByName (protocol1);
  // Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid1));
  // Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());

  // TypeId tid2 = TypeId::LookupByName (protocol2);
  // Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid2));
  // Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());

  // TypeId tid3 = TypeId::LookupByName (protocol3);
  // Config::Set ("/NodeList/*/$ns3::TcpL4Protocol/SocketType", TypeIdValue (tid3));
  // Ptr<Socket> ns3TcpSocket3 = Socket::CreateSocket (nodes.Get (1), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> app1 = CreateObject<MyApp> ();
  app1->Setup (ns3TcpSocket1, sinkAddress1, 3000, 50000, DataRate ("1.5Mbps"));
  nodes.Get (0)->AddApplication (app1);
  app1->SetStartTime (Seconds (1.));
  app1->SetStopTime (Seconds (20.));

  Ptr<MyApp> app2 = CreateObject<MyApp> ();
  app2->Setup (ns3TcpSocket2, sinkAddress1, 3000, 50000, DataRate ("1.5Mbps"));
  nodes.Get (0)->AddApplication (app2);
  app2->SetStartTime (Seconds (5.));
  app2->SetStopTime (Seconds (25.));

  Ptr<MyApp> app3 = CreateObject<MyApp> ();
  app3->Setup (ns3TcpSocket3, sinkAddress2, 3000, 50000, DataRate ("1.5Mbps"));
  nodes.Get (1)->AddApplication (app3);
  app3->SetStartTime (Seconds (15.));
  app3->SetStopTime (Seconds (30.));

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream ("third_1.cwnd");
  ns3TcpSocket1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream1));
  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("third_2.cwnd");
  ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream2));
  Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper.CreateFileStream ("third_3.cwnd");
  ns3TcpSocket3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream3));

  PcapHelper pcapHelper;
  Ptr<PcapFileWrapper> file1 = pcapHelper.CreateFile ("third_1.pcap", std::ios::out, PcapHelper::DLT_PPP);
  devices1.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file1));
  Ptr<PcapFileWrapper> file2 = pcapHelper.CreateFile ("third_2.pcap", std::ios::out, PcapHelper::DLT_PPP);
  devices2.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file2));

  Simulator::Stop (Seconds (30));
  Simulator::Run ();
  Simulator::Destroy ();

  cout<<"Packets Dropped: "<<Packets_Dropped<<"\n";

  return 0;
}

