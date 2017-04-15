 //VIGINTI SERVER MODULE//
//Version 0.2a//

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <signal.h>

//Define section for short command with server//
#define CMD_HOST 0x03
#define CMD_JOIN 0x04
#define CMD_START 0x05
#define CMD_TURN 0x06
#define CMD_WAIT 0x07
#define CMD_WIN 0x08
#define CMD_LOSE 0x09
#define GAME_ONE 0x10
#define GAME_TWO 0x11
#define GAME_THREE 0x12
#define CMD_READY 0x13
#define CMD_EXIT 0x14

//Global Variable//
struct sockaddr_in ser; //Struct of server information
struct sockaddr cli[4]; //Struct of client information
int addr_size = sizeof (struct sockaddr); //Size of address depend on player = 4
int num_players = 0; //this int use to indicate player total numbers
int maxnum;
int max_player;
int sum_net;
int maxnum_net;
char readycmd[2];


SOCKET soc[5]; //Socket for player 4+1

int handler_serverready(int player){

    int handler = send(soc[player], readycmd, 2, 0);
    if((handler == 0) || (handler == SOCKET_ERROR)){
        system("cls");
        printf("\n[Error: Player %d disconnected, Server will restart soon.]\n", player);
        WSACleanup();
        Sleep(7500);
        system("cls");
        main();
    }
    return 0;

}

int handler_serverexit(){

    char cmd[2];
    sprintf(cmd,"%c%d", CMD_EXIT, 0);
    int i;
    for(i = 0;i < 6;i++){
        send(soc[i],cmd,2,0);
    }
    exit(0);

}

int main(){

    sprintf(readycmd,"%c%d",CMD_READY,0);
    socketstart();
    header();
    hostmode();
    waitmode();
    gamemode();
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
			exit_handler(1);
            while(recv(soc[1], &sum_net, sizeof(sum_net), 0) > 0)
            {
                max_player = ntohl(sum_net);
                printf("- Player required: %d\n", max_player);
                break;
            }
            exit_handler(1);
            while(recv(soc[1], &maxnum_net, sizeof(maxnum_net), 0) > 0)
            {
                maxnum = ntohl(maxnum_net);
                printf("- Ending number: %d\n", maxnum);
                break;
            }

        }
	}
}

int waitmode(){

	printf("\n[Waiting for player...]\n\n");
	printf("Player 1: Joined (Host)\n");
	signal(SIGBREAK, &handler_serverexit);

	while(num_players < max_player ){
		soc[num_players+1] = accept(soc[0],&cli[num_players],&addr_size); //bind incoming connection to socket

		if (soc[num_players+1]==INVALID_SOCKET) //Error checking
		{
			printf("Error:  Unable to accept connection!\n");
			WSACleanup ();
			return 0;
		}
		else
		{
            char cmd[2]; //char for command to communicate with client
			sprintf(cmd,"%c%d",CMD_JOIN,0);
			send(soc[num_players+1],cmd,2,0);

            printf("Player %d: Joined\n", num_players+1);
			int pnum_net = htonl(num_players+1);

			maxnum_net = htonl(max_player);
			sum_net = htonl(maxnum);

			send(soc[num_players+1], (const char*)&sum_net, sizeof(sum_net), 0);
			send(soc[num_players+1], (const char*)&maxnum_net, sizeof(maxnum_net), 0);
            send(soc[num_players+1], (const char*)&pnum_net, sizeof(pnum_net), 0);

            num_players++;
        }
	}

	printf("[All player joined!, Starting game...]\n");
	closesocket (soc[0]);
}

int gamemode(){

    printf("\n--------------------------------------------------\n");
    printf("[Game is starting...]\n\n");
    int i;
    char startcmd[2];
    sprintf(startcmd,"%c%d",CMD_START,0);
    for (i = 1 ; i < max_player+1 ; i++){
        printf("[Player %d starting...]\n", i);
        send(soc[i], startcmd, 2, 0);
    }

    int totalnumber = 0;
    int turn = 1;
    int totalturn = 1;

   while(1){
    char cmd[2];
    char recvcmd[2];

    sprintf(cmd,"%c%d",CMD_TURN,0);
    send(soc[turn], cmd, 2, 0);
    distri_waitmode(turn);

    printf("[Turn %d | Total Number %d / %d | Player %d - Waiting for input...]\n", totalturn, totalnumber, maxnum, turn);
    while(recv(soc[turn], recvcmd, 2, 0) > 0){
        if(recvcmd[0] == GAME_ONE){
            totalnumber +=1;
            printf("-> Player %d | Input 1 | Total %d\n", turn, totalnumber);
        }
        if(recvcmd[0] == GAME_TWO){
            totalnumber +=2;
            printf("-> Player %d | Input 2 | Total %d\n", turn, totalnumber);

        }
        if(recvcmd[0] == GAME_THREE){
            totalnumber +=3;
            printf("-> Player %d | Input 3 | Total %d\n", turn, totalnumber);

        }
    printf("[Distro mode]\n");
    int i;
    for(i = 1; i < max_player+1 ; i++){
        printf("Tell Player %d last input\n", i);
        send(soc[i], recvcmd, 2, 0);
        }
    turn++;
    totalturn++;
    if(turn > max_player){
        turn = 1;
    }

    break;
    }


}
}

int distri_waitmode(int turn){

    //This function use for distribute wait mode for user

    char waitcmd[2];
    sprintf(waitcmd,"%c%d",CMD_WAIT,0);
    printf("%d", turn);

    if(turn == 1){
        int i;
        printf("Least Distro mode!\n");
        for(i = 2 ; i < max_player+1 ; i++){
            send(soc[i], waitcmd, 2, 0);
            printf("Tell Player %d to wait\n", i);
        }
        goto ending;
    }

    if(turn == max_player){

        int i;
        printf("Max Distro mode!\n");
        for(i = max_player-1 ; i > 0 ; i--){
            send(soc[i], waitcmd, 2, 0);
            printf("Tell Player %d to wait\n", i);
        }
        goto ending;
    }

    else{

        int i,j;
        printf("Between Distro mode!\n");

        for(i = turn+1 ; i < max_player+1 ; i++){
            send(soc[i], waitcmd, 2, 0);
            printf("Tell Player %d to wait\n", i);
        }

        for (j = turn-1 ; j > 0 ; j--){
            send(soc[i], waitcmd, 2, 0);
            printf("Tell Player %d to wait\n", j);
        }
        goto ending;
    }

    ending:
    return 0;

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

int exit_handler(int turn){

    char cmd[2];
    while(recv(soc[turn], cmd, 2, 0) > 0){
        if(cmd[0] == CMD_EXIT){
            printf("\n[Player %d exit!, Initiate Winsock Cleanup...]\n", turn);
            WSACleanup();
            printf("- Unload Winsock 2.2");
            int i = 0;
            for(i = 0 ; i < 6 ; i++){
                printf("- Close socket %d\n", i);
                closesocket(i);
                num_players = 0;
            }
            printf("[Process successfully restart!, Rebooting server]");
            Sleep(3000);
            system("cls");
            main();
        }
        if(cmd[0] == CMD_READY){
            return 0;
        }
    }
}
