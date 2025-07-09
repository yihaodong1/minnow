#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  // debug( "unimplemented wrap( {}, {} ) called", n, zero_point.raw_value_ );
  return zero_point + n;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  debug( "unwrap( {}, {} ) called", zero_point.raw_value_, checkpoint );
  uint64_t raw = raw_value_;
  if(raw_value_ < zero_point.raw_value_){
    raw += ((uint64_t)1 << 32);
  }
  raw -= zero_point.raw_value_;
  uint64_t high = checkpoint & ~(uint64_t)0xffffffff;
  raw |= high;
  // debug("{}", raw);
  if(raw < checkpoint){
    if(checkpoint - raw > 0x80000000)
      raw += 0x100000000;
  }else{
    if(raw - checkpoint > 0x80000000 && raw > 0x100000000)
      raw -= 0x100000000;
  }
  // debug("{}", raw);
  return raw;
}
