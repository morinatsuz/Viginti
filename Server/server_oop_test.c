//VIGINTI SERVER MODULE//
//Version 0.1a//

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>

//Define section for short command with server//
#define CMD_HOST 0x03
#define CMD_PTWO 0x04
#define CMD_PTHREE 0x05
#define CMD_PFOUR 0x06




//Global Variable//
struct sockaddr_in ser; //Struct of server information
struct sockaddr cli[4]; //Struct of client information
int addr_size = sizeof (struct sockaddr); //Size of address depend on player = 4
int num_players = 0; //this int use to indicate player total numbers
int maxnum;
int max_player;
SOCKET soc[5]; //Socket for player 4+1

int main(){

    socketstart();
    header();
    waitplayer();
    getch();

}

void header(){

    //Welcome Message//
    printf("[                      Viginti Server Module                     ]\n");
    printf("[        Version 0.1A  Developed by Viginti Development Team     ]\n\n\n");
    printf("As part of Computer Programming Project 2017\n\n");
    printf("Faculty of Information Technology\n");
    printf("King Mongkut's Institute of Technology Ladkrabang\n\n");
	printf("----------------------------------\n\n");

}

int hostmode(){

	printf("Waiting for Host Player........");

	while(num_players != 1){
		soc[1] = accept(soc[0],&cli[num_players],&addr_size); //bind incoming connection to socket

		if (soc[num_players+1]==INVALID_SOCKET) //Error checking
		{
			printf("Error:  Unable to accept connection!\n");
			WSACleanup ();
			return 0;
		}
		else
		{
			printf("Host Player joined!\n\n");
			char cmd[2]; //char for command to communicate with client
			sprintf(cmd,"%c%d",CMD_HOST,0);
			send(soc[1],cmd,2,0);
			num_players++;

			//room setting progress
			int sum_net;
			int maxnum_net;
			printf("[Lobby Setting]\n\n");
            while(recv(soc[1], &sum_net, sizeof(sum_net), 0) > 0)
            {
                max_player = ntohl(sum_net);
                printf("- Player required: %d\n", max_player);
                break;
            }
            while(recv(soc[1], &maxnum_net, sizeof(maxnum_net), 0) > 0)
            {
                maxnum = ntohl(maxnum_net);
                printf("- Ending number: %d\n", maxnum);
                break;
            }
            break;
        }
	}
}

int waitplayer(){

    num_players = 0;
    max_player = 3;
    char shortcmd = "W";
    while (num_players < max_player);
	{
	    printf("\n\n Waiting for player...");
		soc[num_players+1] = accept(soc[0],&cli[num_players],&addr_size);

		if (soc[num_players+1]==INVALID_SOCKET)
		{
			printf("Error:  Unable to accept connection!\n");
			WSACleanup ();
		}
		else
		{
			printf("Player joined!\n");
			char cmd[1];
			sprintf (cmd,"%c",shortcmd+1);
			send (soc[num_players+1],cmd,1,0);
			num_players++;
		}
	}


}
int socketstart(){

	printf("\n\n[Viginti Server Module Initializing...]\n\n");

	WSADATA w; //Initialize Winsock Libraly

	printf("- Initialize Winsock 2.2...");

	int error = WSAStartup(0x0202,&w); //Winsock error check
	if (error)
	{
		printf(" [ERROR: You need WinSock 2.2!]\n");
		return 0;
	}
	printf(" [PASS]\n");

	if (w.wVersion!=0x0202)
	{
		printf(" [ERROR:  Wrong WinSock version!]\n");
		WSACleanup();
		return 0;
	}

    //Listening Socket Structure
    soc[0] = socket (AF_INET,SOCK_STREAM,0);
	ser.sin_family = AF_INET;
	ser.sin_port = htons (5555);
	ser.sin_addr.s_addr = htonl (INADDR_ANY);

	//Server socket binding
	printf("- Binding Socket...");
	if (bind(soc[0],(LPSOCKADDR)&ser,sizeof(ser))==SOCKET_ERROR)
	{
		printf("Error:  Unable to bind socket!\n");
		WSACleanup ();
		return 0;
	}
	printf(" [PASS]\n");

    //Server socket listening
	printf("- Listening Check...");
	if (listen(soc[0],1)==SOCKET_ERROR)
	{
		printf("Error:  Unable to listen!\n");
		WSACleanup ();
		return 0;
	}
	printf(" [PASS]\n\n");
	printf("----------------------------------");
    printf("Server Successfully Initialize!\n\n");

    }
