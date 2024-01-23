
#include <stdio.h>
#include <string.h>	//strlen
#include <stdlib.h>	//strlen
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>	//write
#include <cstring>
#include <string>
#include <cstdlib>

#include <pthread.h> //for threading , link with lpthread
#include <iostream>
#include <errno.h>
#include <wiringPiSPI.h>
#include <unistd.h>

using namespace std;

// channel is the wiringPi name for the chip select (or chip enable) pin.
// Set this to 0 or 1, depending on how it's connected.
static const int CHANNEL = 1;

void *connection_handler(void *);

void send_receive(unsigned char *data, int len) {
    // cout << "CHANNEL:" << CHANNEL << endl;
    // cout << "The data:" << (char *)data << endl;
    for (int n = 0; n < len; n++) {
      unsigned char *c = &data[n];
      wiringPiSPIDataRW(CHANNEL, c, 1);
      cout << c;
    }
    cout << "In send_receive data:" << (char *)data << endl;
}

int main()
{
 	int socket_desc , new_socket , c , *new_sock;
	struct sockaddr_in server , client;
	std::string message;
    int fd, result;
    unsigned char buffer[100];
    unsigned char read_buffer[100];
    sprintf((char *)read_buffer, "For Getting data requested");
    cout << "Initializing" << endl ;

    // Configure the interface.
    // CHANNEL insicates chip select,
    // 500000 indicates bus speed.
    fd = wiringPiSPISetup(CHANNEL, 57600);

    cout << "Init result: " << fd << endl;

    sleep(1);

 

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		
		//Reply to the client
		message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
		write(new_socket , message.c_str() , message.length());
		
		pthread_t sniffer_thread;
		new_sock = (int *)malloc(1);
		*new_sock = new_socket;

        connection_handler(new_sock);
		
		// if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		// {
		// 	perror("could not create thread");
		// 	return 1;
		// }
		
		// //Now join the thread , so that we dont terminate before the thread
		// //pthread_join( sniffer_thread , NULL);
		// puts("Handler assigned");
	}
	
	if (new_socket<0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char client_message[2000];
	std::string message;
    unsigned char buffer[100];

	//Send some messages to the client
	message = "Greetings! I am your connection handler\n";
	write(sock , message.c_str() , message.length());
	sleep(1);
	message = "Now type something and i shall send it to serial port \n";
	write(sock ,message.c_str() , message.length());
	sleep(1);
	message = "PI_READY\n";
	write(sock ,message.c_str() , message.length());
	sleep(1);

	//Receive a message from client
	while( (read_size = recv(sock, client_message , 2000 , 0)) > 0 )
	{
		//Send the message back to client
		//  write(sock , client_message , strlen(client_message));
		client_message[read_size] = '\0';
        // cout << "read_size = " << read_size << endl;
        // cout << "Client Message: " << client_message << endl;
		// printf("Client Message = [%s]\n", client_message);

        sprintf((char *)buffer, "Speed l:+00350 r:+00150\n");
        send_receive(buffer, 24);
		sleep(1);
		// sprintf((char *)buffer, "%s\n", client_message);
        // send_receive(buffer, 24);
        // cout << "from arduino :" << buffer << endl;
        // write(sock , buffer , 24);
	   
	
		// printf("Response from arduino = [%s]\n", response.c_str());
		// write(sock , response.c_str() , response.length());
		
	}
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
		
	//Free the socket pointer
	free(socket_desc);
	
	return 0;
}