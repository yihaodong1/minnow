#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  if ( message.SYN ) {
    // use start_ to enable reassembler_
    start_ = true;
    zero_point_ = message.seqno;
    message.seqno = message.seqno + 1;
  }
  if ( message.RST ) {
    start_ = false;
    // set ByteStream to be error
    reader().set_error();
  }
  if ( start_ ) {
    message.seqno = message.seqno - 1;
    debug( "receive:{}", reassembler_.acknum() );
    // use the first unassembled byte as checkpoint
    uint64_t checkpoint = reassembler_.acknum();
    reassembler_.insert( message.seqno.unwrap( zero_point_, checkpoint ), message.payload, message.FIN );
    // use fin_ to record whether the last byte has been sent
    fin_ |= message.FIN;
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage t;
  debug( "send:{}", reassembler_.acknum() );
  // if not start_, the optional t.ackno has no value
  if ( start_ )
    // fin_ && reassembler_.isempty() means all the bytes pending
    // in reassembler_ has been reassembled and the FIN has benn sent
    // and FIN occupy one seqno
    t.ackno = Wrap32::wrap( reassembler_.acknum() + 1 + ( fin_ && reassembler_.isempty() ), zero_point_ );
  // t.window_size is uint16_t, so the actual window_size can be greater
  t.window_size = writer().available_capacity() > UINT16_MAX ? UINT16_MAX : writer().available_capacity();
  // if ByteStream has error, has to be reset
  t.RST |= reader().has_error();
  return t;
}
