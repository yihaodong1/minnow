#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  if(message.SYN){
    start_ = true;
    zero_point_ = message.seqno;
    message.seqno = message.seqno + 1;
  }
  if(message.RST){
    start_ = false;
    reader().set_error();
  }
  if(start_){
    message.seqno = message.seqno - 1;
    debug("receive:{}", reassembler_.acknum());
    uint64_t checkpoint = reassembler_.acknum();
    reassembler_.insert(message.seqno.unwrap(zero_point_, checkpoint), 
        message.payload, message.FIN);
    fin_ |= message.FIN;
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage t;
  debug("send:{}", reassembler_.acknum());
  if(start_)
    t.ackno = Wrap32::wrap(reassembler_.acknum() + 1 + (fin_ && reassembler_.isempty())
        , zero_point_);
  t.window_size = writer().available_capacity() > UINT16_MAX? 
    UINT16_MAX: writer().available_capacity();
  t.RST |= reader().has_error();
  return t;
}
