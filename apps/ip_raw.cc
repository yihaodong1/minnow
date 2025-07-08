#include "socket.hh"

using namespace std;

class RawSocket : public DatagramSocket
{
public:
  RawSocket() : DatagramSocket( AF_INET, SOCK_RAW, IPPROTO_RAW ) {}
};

int main()
{
  // construct an Internet or user datagram here, and send using the RawSocket as in the Jan. 10 lecture
  RawSocket sock;
  string datagram;
  datagram += "Hello from CS144!!";
  sock.sendto( Address( "127.0.0.1" ), datagram );

  return 0;
}
