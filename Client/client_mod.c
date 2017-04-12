#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <winsock2.h>

#define CMD_HOST 0x03
#define CMD_JOIN 0x04
#define CMD_START 0x05



SOCKET soc;
struct sockaddr_in ser;
int playerno;
int numplaynet;
int maxnum;
int pmax;


void home_start(){
    printf("\n");
    printf(" ####################################################################\n");
    printf(" ##  ____    ____      _______        ___    __   _________        ##\n");
    printf(" ##  \\   \\  /   / __  | ______|  __  |   \\  |  | |         |  __   ##\n");
    printf(" ##   \\   \\/   / |  | | | ____  |  | |    \\ |  | |__     __| |  |  ##\n");
    printf(" ##    \\      /  |  | | | |__ | |  | |  |\\ \\|  |    |   |    |  |  ##\n");
    printf(" ##     \\    /   |  | | |___| | |  | |  | \\    |    |   |    |  |  ##\n");
    printf(" ##      \\__/    |__| |_______| |__| |__|  \\___|    |___|    |__|  ##\n");
    printf(" ##                                                                ##\n");
    printf(" ####################################################################\n\n");
    printf(" Viginti [Multiplayer Version] - Client Module 0.1A (Debug build)\n\n");
    printf("[Client Module Initializing...]\n\n");
}

int sock_start(){

    printf("- Initialize Winsock 2.2...");

    WSADATA w;
	int error = WSAStartup(0x0202,&w);

	if (error)
	{
		printf("Error:  You need WinSock 2.2!\n");
		return 0;
	}

	printf(" [PASS]\n");

	if (w.wVersion!=0x0202)
	{
		printf("Error:  Wrong WinSock version!\n");
		WSACleanup();
		return 0;
	}

	soc = socket (AF_INET,SOCK_STREAM,0);

	printf("- Connecting to server...");

    ser.sin_family = AF_INET;
    ser.sin_port = htons (5555);	// RPSS port is 5555
    ser.sin_addr.s_addr = inet_addr ("127.0.0.1");

    if (connect(soc, (LPSOCKADDR)&ser, sizeof(ser))==SOCKET_ERROR)
    {
        printf(" [ERROR: Can't connect to server!]\n\n");
        WSACleanup();

        printf("----------------------------------------\n");
        printf("\nPlease restart and try again\n\n");
        printf("[Press any key to exit]");
        getch();
        exit(0);
        return 0;
    }

    printf(" [PASS]\n\n");
    printf("[Server successfully connect! - Starting game...]\n\n");
    printf("----------------------------------------\n\n");

}

int hostmode(){
    char pname[20];
    char max_player[20];

    printf("[You're host player!...Creating lobby]\n");
    printf("Please configure lobby setting\n\n");
    printf("[How many player in this game? (2-4 Person)] \n: ");

    int sum_players = 0;
    scanf("%d", &sum_players);
    while(sum_players <= 1 || sum_players > 4)
    {
        printf("\n###Number of player is not correct###\n");
        printf("Please try again enter  (2 to 4 players) : ");
        scanf("%d", &sum_players);
    }

    int sum_net = htonl(sum_players);
    send(soc, (const char*)&sum_net, sizeof(sum_players),0);

    printf("\n[Please enter your game ending number] \n: ");
    int max_num = 0;
    scanf("%d", &max_num);
    while(max_num < 20)
    {
        printf("\n###Number over is not correct###\n");
        printf("Please try again enter Num over (num >= 20) : ");
        scanf("%d", &max_num);
    }

    int max_net = htonl(max_num);
    send(soc, (const char*)&max_net, sizeof(max_num), 0);


    printf("[Lobby successfully configured!] \n\n");


    }

int joinmode(){

    printf("     Welcome to Viginti : Multiplayer Counting Game\n");
    printf("[Lobby setting |");
    int maxnum_net;
    int maxp_net;
    int pnum_net;
    while(recv(soc, &maxnum_net, sizeof(maxnum_net), 0) > 0){
            maxnum = ntohl(maxnum_net);
            printf(" Ending number : %d |", maxnum);
            break;
    }
    while(recv(soc, &maxp_net, sizeof(maxp_net), 0) > 0){
            pmax = ntohl(maxp_net);
            printf(" Player Required : %d]\n", pmax);
            break;
    }
    while(recv(soc, &pnum_net, sizeof(pnum_net), 0) > 0){
            int pnum = ntohl(pnum_net);
            printf("\nYou're player %d", pnum);
            break;
    }
}

int waitmode(){

    printf("\n\n[Waiting for other players...]");
    return 0;
}


int main(){
    home_start();
    sock_start();
    char cmd[2];
    while(recv(soc,cmd,2,0) > 0)
    {
        if(cmd[0] == CMD_HOST)
        {
            hostmode();
        }
        if(cmd[0] == CMD_JOIN)
        {
            joinmode();
        }
        break;
    }
    waitmode();
    getch();

}

