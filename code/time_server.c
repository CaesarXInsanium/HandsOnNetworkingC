
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
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
  printf("Configuring Local Address...\n");
  // describes the sort of connection we want
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family =
      AF_INET6; // we prefer IPv6, but when we try access localhost from our
                // browser using ipv4 it maps directly to an ipv6 address
  hints.ai_socktype = SOCK_STREAM; // open TCP connection
  hints.ai_flags = AI_PASSIVE;     // listen to any avalable network device
  struct addrinfo *bind_address;
  int status = getaddrinfo(0, "8080", &hints, &bind_address);
  printf("getaddrinfo() status: %d\n", status);
  printf("Creating Socket\n");

  // file descripter for the adapter connection we receive
  // is abtraction over end point in network communication
  SOCKET socket_listen;
  // loads the socket, this is actualy a file descriptor
  socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype,
                         bind_address->ai_protocol);

  if (!IsValidSocket(socket_listen)) {
    fprintf(stderr, "socket() failed. (%d)\n", GetSocketErrno());
    return 1;
  }
  printf("Binding Socket to local Address...\n");

  // disables IPV6 only and allows for usage of both versions
  if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, NULL,
                 sizeof(NULL))) {
    fprintf(stderr, "setsockopt() failed. (%d)\n", GetSocketErrno());
  }
  // assigns socket ip address and port number
  if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
    fprintf(stderr, "bind() failed. (%d)\n", GetSocketErrno());
    return EXIT_FAILURE;
  }
  // bind address is no longer needed
  freeaddrinfo(bind_address);
  printf("Listening...\n");
  if (listen(socket_listen, 10) <
      0) { // listens for 10 different connections to queue up
    fprintf(stderr, "listen() failed, (%d)\n", GetSocketErrno());
    return EXIT_FAILURE;
  }
  printf("Waiting for connection...\n");
  struct sockaddr_storage client_address;
  // represents connection to first client to extablish connefction with
  socklen_t client_len = sizeof(client_address);

  // block program until connection is established
  // gives file descriptor in order to read and write data in the stream
  SOCKET socket_client =
      accept(socket_listen, (struct sockaddr *)&client_address, &client_len);
  if (!IsValidSocket(socket_client)) {
    fprintf(stderr, "accept() failed: (%d)\n", GetSocketErrno());
    return EXIT_FAILURE;
  }

  printf("Client is connected...\n");
  char address_buffer[100];
  // takes client address and address length and places result into char buffer
  getnameinfo((struct sockaddr *)&client_address, client_len, address_buffer,
              sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
  printf("%s\n", address_buffer);

  printf("Reading Request...\n");
  char request[1024];
  // copies bytes received from the client request
  int bytes_received = recv(socket_client, request, 1024, 0);
  printf("Received %d bytes.\n", bytes_received);
  printf("Request Received:\n%s\n", request);

  printf("Sending Response...\n");
  const char *response = "HTTP/1.1 200 OK\r\n"
                         "Connection: close\r\n"
                         "Content-type: text/plain\r\n\r\n"
                         "Local time is: ";
  int bytes_sent = send(socket_client, response, strlen(response), 0);
  printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

  time_t timer;
  time(&timer);
  char *time_msg = ctime(&timer);
  bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
  printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

  printf("Closing Connection..\n");
  CloseSocket(socket_client);
  printf("Finished.\n");
}
