#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SOCKET int // file descriptors are just integer IDs
#define IsValidSocket(s) ((s) >= 0)
#define CloseSocket(s) (close(s))
#define GetSocketErrno() (errno)

// There are two types of sockets.
// Connection Oriented which is TCP where both sides ensure that connection is
// optimat Connectionless is UDP where is does not matter if one side receives
// the packets or not.

int main(void) {
  printf("Ready to use Socket API\n");
}
