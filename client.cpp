#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include <mutex>

std::mutex m;

int sock = socket(AF_INET, SOCK_STREAM, 0); //Socket.

char buf[4096]; //Buffer.

//Function for receiving messages from server.
void f()
{
	while (true)
	{
		m.lock();
		memset(buf, 0, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		if (bytesReceived == -1) { std::cerr << "There was an error getting response from server\r\n"; break; }
		if (bytesReceived == 0) { std::cout << "Server disconnected!\r\n"; break; }
		else std::cout << "SERVER: " << std::string(buf, bytesReceived) << std::endl << "> ";
		m.unlock();
	}
}

int main()
{
	//  Create a hint structure for the server.
	short int port = 25565;
	std::string ipAddress;
	std::cout << "Enter the ip address: ";
	getline(std::cin, ipAddress);
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//  Connect to the server on the socket
	int connectRes = connect(sock, (sockaddr*)& hint, sizeof(hint));
	if (connectRes == -1)
	{
		return 1;
	}

	//Thread for receiving.
	std::thread th(f);
	th.detach();

	//Send to server.
	std::string msg;
	while (true)
	{
		std::cout << "> ";
		getline(std::cin, msg);
		int sendRes = send(sock, msg.c_str(), msg.size() + 1, 0);
		if (sendRes == -1) std::cout << "Could not send to server! Whoops!\r\n";
	}

	//  Close the socket
	close(sock);
	return 0;
}