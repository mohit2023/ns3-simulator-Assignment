#include "TcpNewRenoCSE.h"
#include "ns3/log.h"
#include "ns3/tcp-socket-base.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpNewRenoCSE");
NS_OBJECT_ENSURE_REGISTERED (TcpNewRenoCSE);

TypeId
TcpNewRenoCSE::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpNewRenoCSE")
    .SetParent<TcpNewReno> ()
    .AddConstructor<TcpNewRenoCSE> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TcpNewRenoCSE::TcpNewRenoCSE ()
  : TcpNewReno ()
{
  NS_LOG_FUNCTION (this);
}

TcpNewRenoCSE::TcpNewRenoCSE (const TcpNewRenoCSE &sock)
  : TcpNewReno (sock)
{
  NS_LOG_FUNCTION (this);
}

TcpNewRenoCSE::~TcpNewRenoCSE ()
{
  NS_LOG_FUNCTION (this);
}

Ptr<TcpCongestionOps>
TcpNewRenoCSE::Fork (void)
{
  return CopyObject<TcpNewRenoCSE> (this);
}

std::string
TcpNewRenoCSE::GetName () const
{
  return "TcpNewRenoCSE";
}

uint32_t
TcpNewRenoCSE::SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  // double add = static_cast<uint32_t> (std::pow (static_cast<double>(tcb->m_segmentSize), 1.9) / static_cast<double> (tcb->m_cWnd.Get()));
  // tcb->m_cWnd += static_cast<uint32_t> (add);
  // NS_LOG_INFO ("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh " << tcb->m_ssThresh);

  if (segmentsAcked >= 1)
    {
      // tcb->m_cWnd += tcb->m_segmentSize;

      double add = static_cast<uint32_t> (std::pow (static_cast<double>(tcb->m_segmentSize), 1.9) / static_cast<double> (tcb->m_cWnd.Get()));
      tcb->m_cWnd += static_cast<uint32_t> (add);
      NS_LOG_INFO ("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh " << tcb->m_ssThresh);
      return segmentsAcked - 1;
    }

  return 0;
}

void
TcpNewRenoCSE::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  // double add = 0.5 * static_cast<double> (tcb->m_segmentSize);
  // tcb->m_cWnd += static_cast<uint32_t> (add);
  // NS_LOG_INFO ("In CongAvoid, updated to cwnd " << tcb->m_cWnd <<
  //                  " ssthresh " << tcb->m_ssThresh);

  if (segmentsAcked > 0)
    {
      // double adder = static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get ();
      // adder = std::max (1.0, adder);
      // tcb->m_cWnd += static_cast<uint32_t> (adder);
  
      double add = 0.5 * static_cast<double> (tcb->m_segmentSize);
      tcb->m_cWnd += static_cast<uint32_t> (add);
      NS_LOG_INFO ("In CongAvoid, updated to cwnd " << tcb->m_cWnd <<
                   " ssthresh " << tcb->m_ssThresh);
    }
}

} // namespace ns3
