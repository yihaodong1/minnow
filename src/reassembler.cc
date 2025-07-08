#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  debug( "insert({}, {}, {}) called", first_index, data, is_last_substring );
  uint64_t orig_len = data.size();
  uint64_t capacity = output_.writer().available_capacity();
  uint64_t left_bound = max( first_index, current );
  uint64_t right_bound = min( first_index + data.size(), current + capacity );
  // use ready_close to record if it is to be close,
  // when is_last_substring and the last substr right
  ready_close |= ( is_last_substring && first_index + orig_len == right_bound );
  if ( left_bound >= right_bound ) {
    if ( ready_close && pend_ == 0 )
      output_.writer().close();
    return;
  }
  data = data.substr( left_bound - first_index, right_bound - left_bound );

  auto it = m.lower_bound( left_bound );
  // find the overlap between the next one
  if ( it != m.end() && it->first <= right_bound ) {
    while ( it != m.end() && it->first <= right_bound ) {
      // use loop to combine all the substr, like insert b, d, abcd
      if ( it->first + it->second.size() > right_bound ) {
        data.replace( data.begin() + it->first - left_bound, data.end(), it->second );
        right_bound = it->first + it->second.size();
      }
      pend_ -= it->second.size();
      m.erase( it );
      it = m.lower_bound( left_bound );
    }
  }
  // find the overlap between the previous one
  if ( it != m.begin() ) {
    auto prev_it = prev( it );
    if ( prev_it->first + prev_it->second.size() >= left_bound ) {
      if ( prev_it->first + prev_it->second.size() < right_bound ) {
        pend_ += right_bound - prev_it->first - prev_it->second.size();
        prev_it->second.replace(
          prev_it->second.begin() + left_bound - prev_it->first, prev_it->second.end(), data );
      }
    } else {
      // no overlap between the previous one
      m.insert( make_pair( left_bound, data ) );
      pend_ += data.size();
    }

  } else {
    // there is no previous one
    m.insert( make_pair( left_bound, data ) );
    pend_ += data.size();
  }

  it = m.lower_bound( current );
  while ( it != m.end() && it->first == current ) {
    // push the Reassembled byte into writer
    output_.writer().push( it->second );
    current += it->second.size();
    pend_ -= it->second.size();
    m.erase( it );
    it = m.lower_bound( current );
  }
  if ( ready_close && pend_ == 0 )
    output_.writer().close();
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  // debug( "unimplemented count_bytes_pending() called" );
  return pend_;
}
