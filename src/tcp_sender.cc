#include "tcp_sender.hh"
#include "debug.hh"
#include "tcp_config.hh"

using namespace std;

// This function is for testing only; don't add extra state to support it.
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // debug( "unimplemented sequence_numbers_in_flight() called" );
  return bytes_fly_;
}

// This function is for testing only; don't add extra state to support it.
uint64_t TCPSender::consecutive_retransmissions() const
{
  // debug( "unimplemented consecutive_retransmissions() called" );
  return transmissions_;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  // debug( "unimplemented push() called" );
  while(window_size_ > bytes_fly_){
    TCPSenderMessage msg;
    msg.seqno = isn_ + bytes_send_;
    msg.SYN = bytes_send_ == 0;
    uint64_t cap = min((size_t)window_size_ - bytes_fly_, TCPConfig::MAX_PAYLOAD_SIZE);
    uint64_t len = cap > (size_t)msg.SYN ? cap - msg.SYN:0;
    debug("cap {} len {}", cap, len);
    bool flag = false;

    flag |= (msg.SYN && cap);

    if(reader().bytes_buffered() && len > 0){
      flag |= 1;
      msg.payload = reader().peek().substr(0, len);
      reader().pop(len);
    }

    if(reader().is_finished() && cap > msg.sequence_length()){
      msg.FIN = true;
      flag |= 1;
    }

    if(flag && !fin_){
      fin_ = msg.FIN;
      bytes_send_ += msg.sequence_length();
      bytes_fly_ += msg.sequence_length();
      queue_.push_back(msg);
      transmit(msg);
      timer_start_ = true;
    }else {
      break;
    }

  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  // debug( "make_empty_message() called" );
  TCPSenderMessage msg;
  msg.seqno = isn_ + bytes_send_;
  return msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // debug( "receive() called" );
  if(msg.ackno <= isn_ + bytes_send_){
    window_size_ = msg.window_size;
    while(!queue_.empty()){
      auto it = queue_.front();
      if(msg.ackno.has_value()&&
          it.seqno.unwrap(isn_, bytes_acked_) + it.sequence_length() > msg.ackno.value().unwrap(isn_, bytes_acked_)){
        break;
      }
      bytes_fly_ -= it.sequence_length();
      bytes_acked_ += it.sequence_length();
      queue_.pop_front();

      timer_start_ = bytes_fly_ != 0;
      RTO_ms_ = initial_RTO_ms_;
      transmissions_ = 0;
      current_time_ = 0;
    }
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // debug( "unimplemented tick({}, ...) called", ms_since_last_tick );
  if(timer_start_)
    current_time_ += ms_since_last_tick;
  if(current_time_  >= RTO_ms_){
    if(!queue_.empty()){
      auto it = queue_.front();
      transmit(it);
    }
    if(window_size_){
      RTO_ms_ = RTO_ms_ << 1;
      ++transmissions_;
    }
    current_time_ = 0;
  }
}

uint64_t TCPSender::unwrap(Wrap32 n){
  return n.unwrap(isn_, ack_);
}
