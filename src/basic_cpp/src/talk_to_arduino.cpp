#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>  // close

#include <iostream>
#include <cstring>

int socket_desc;
char server_reply[2000];

std::string getFromPi(std::string message) {
	// printf("messge to send <%s>\n", message.c_str());
    // send(socket_desc , message.c_str() ,  message.length() , 0);
	printf("messge to send <%s>\n", message.c_str());
	write(socket_desc , message.c_str() ,  message.length());
    int n = read(socket_desc, server_reply , 2000);

    if( n >= 0) {
        return std::string(server_reply, server_reply + n);

    }
    return "NO DATA";
}

bool waitForPi() {
	int n;
	bool ready = false;
	int msgNum = 0;
	while (!ready) {
 		n = read(socket_desc, server_reply , 2000);
		if (n > 0) {
			msgNum++;
			server_reply[n-1] = '\0';
			printf("%d RESPONSE [%s]\n", msgNum, server_reply);
			if (!strcasecmp(server_reply, "PI_READY"))
				ready = true;
		}
	}
	return ready;
}

int main(int argc , char *argv[])
{ 
	int steps;
	std::string cmds[10];
	// int socket_desc;
	struct sockaddr_in server;
	std::string message;
	
	printf("argc %d\n", argc);
	if (argc > 1) {
		steps = argc - 1;
		for (int n = 0; n < steps; n++) {
			cmds[n] = argv[n + 1];
			// printf("step %d, cmd <%s>\n", n, cmds[n].c_str());
		}
	}
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
		
	server.sin_addr.s_addr = inet_addr("192.168.1.103");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected\n");
	waitForPi();
	if (steps > 0) {
		for (int n = 0; n < steps; n++) {
			printf("step %d/%d action %s\n", n + 1, steps, getFromPi(cmds[n]).c_str());
			// usleep(100000);
		}
	}
	else {
		puts(getFromPi("e").c_str());
		puts(getFromPi("m 200 0").c_str());	


		sleep(2);
		puts(getFromPi("e").c_str());
		puts(getFromPi("m 0 200").c_str());
		sleep(2);
		puts(getFromPi("e").c_str());
		puts(getFromPi("m 0 0").c_str());
		sleep(1);
		puts(getFromPi("r").c_str());
	}
	// sleep(1);
    // puts(getFromPi("e ").c_str());

	// puts("Reply received\n");
	
	close(socket_desc);
    puts("Socket closed\n");
	return 0;
}