#include <stdio.h>
#include <string.h>	//strlen
#include <stdlib.h>	//strlen
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>	//write
#include <serial/serial.h>
#include <cstring>
#include <string>
#include <cstdlib>

#include<pthread.h> //for threading , link with lpthread



void *connection_handler(void *);
serial::Serial serial_conn_;

int main(int argc , char *argv[]) 
{
	int socket_desc , new_socket , c , *new_sock;
	struct sockaddr_in server , client;
	std::string message;
	std::string serial_device = "/dev/ttyACM0";
    int32_t baud_rate = 57600;
    int32_t timeout_ms = 2000;
    //  serial::Serial serial_conn_;

    // if (argc < 3) {
    //     printf(" parameters serial_device, baud_rate and tomeout_ms are required");
    //     return 1;

    // }

    // serial_device = (char *)argv[0];
    // baud_rate = std::stoi(argv[1]);
    // timeout_ms = std::stoi(argv[2]);

    serial_conn_.setPort(serial_device);
    serial_conn_.setBaudrate(baud_rate);
    serial::Timeout tt = serial::Timeout::simpleTimeout(timeout_ms);
    serial_conn_.setTimeout(tt); // This should be inline except setTimeout takes a reference and so needs a variable
    serial_conn_.open();
	
    // printf("Serial connection to %s opened", serial_device);
    //  serial_conn_.close();
    // printf("Serial connection to %s Closed", serial_device);


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
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
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
		printf("Client Message = [%s]\n", client_message);
	
		serial_conn_.write(client_message);
		serial_conn_.write("\n");
		std::string response = serial_conn_.readline();

		printf("Response from arduino = [%s]\n", response.c_str());
		write(sock , response.c_str() , response.length());
		
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