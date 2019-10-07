#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <mutex>

std::mutex m;

//Listening socket.
int ListeningSocket = socket(AF_INET,SOCK_STREAM,0);
sockaddr_in client;
socklen_t clientSize = sizeof(client);
char buf[4096];


//Function for receiving messages from client.
void f(int sock)
{
  while (true)
  {
  m.lock();
  memset (buf,0,4096);
  int bytesReceived = recv(sock, buf, 4096,0);
  if (bytesReceived == -1) {std::cerr << "Error Getting A Response From The Client\r\n"; break;}
  if (bytesReceived == 0) {std::cout << "Client disconnected!\r\n"; break;}
  else std::cout << "CLIENT:  " << std::string(buf, bytesReceived) << std::endl;
  m.unlock();
  }
}



int main()
{
  //Ip & Port.
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(25565);
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

  //Binding
  bind(ListeningSocket, (sockaddr*)&hint, sizeof(hint));


  listen(ListeningSocket, SOMAXCONN);
  int clientSocket = accept(ListeningSocket, (sockaddr*)&client, &clientSize);

  char host[NI_MAXHOST];
  char service[NI_MAXSERV];
  memset(host, 0, NI_MAXHOST);
  memset(service, 0, NI_MAXSERV);

  //Connection messages.
  if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
  {
      std::cout << host << " Connected On Port:  " << service << std::endl;
  }
  else
  {
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    std::cout << host << " Connected On Port:  " << ntohs(client.sin_port) << std::endl;
  }

  //Thread for receiving.
  std::thread th(f,clientSocket);
  th.detach();


  //Sending message to the client.
  std::string msg;
  while(true)
    {
        std::cout << "> ";
        getline(std::cin, msg);
        int sendRes = send(clientSocket, msg.c_str(), msg.size() + 1, 0);
        if (sendRes == -1) std::cout << "Could Not Send The Message.\r\n";
    }
  close(clientSocket);
  close(ListeningSocket);
}
