#ifndef TCPNEWRENOCSE_H
#define TCPNEWRENOCSE_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/traced-value.h"

namespace ns3 {

/**
 * \ingroup congestionOps
 *
 * \brief Implementation of the TCP NewRenoCSE algorithm
 *
 * given in assignment
 *
 */
class TcpNewRenoCSE : public TcpNewReno
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Create an unbound tcp socket.
   */
  TcpNewRenoCSE (void);

  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpNewRenoCSE (const TcpNewRenoCSE& sock);

  virtual ~TcpNewRenoCSE (void) override;

  virtual std::string GetName () const override;
  virtual Ptr<TcpCongestionOps> Fork () override;

protected:
  virtual uint32_t SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) override;
  virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) override;

private: // members

private: // attributes

};

} // namespace ns3

#endif // TCPNEWRENOCSE_H
