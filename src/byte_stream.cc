#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  uint64_t a = available_capacity();
  if ( a > data.size() ) {
    buffer += data;
    bytes_pushed_ += data.size();
  } else {
    buffer += data.substr( 0, a );
    bytes_pushed_ += a;
  }
}

void Writer::close()
{
  // Your code here.
  close_ = true;
}

bool Writer::is_closed() const
{
  return close_; // Your code here.
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buffer.size(); // Your code here.
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_; // Your code here.
}

string_view Reader::peek() const
{
  return string_view( buffer ); // Your code here.
}

void Reader::pop( uint64_t len )
{
  uint64_t buffered = bytes_buffered();
  if ( buffered > len ) {
    buffer = buffer.substr( len );
    bytes_popped_ += len;
  } else {
    buffer.erase();
    bytes_popped_ += buffered;
  }
}

bool Reader::is_finished() const
{
  return close_ && bytes_buffered() == 0; // Your code here.
}

uint64_t Reader::bytes_buffered() const
{
  return buffer.size(); // Your code here.
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_; // Your code here.
}
