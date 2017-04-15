#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <winsock2.h>
#include <signal.h>

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

SOCKET soc;
struct sockaddr_in ser;
int playerno;
int numplaynet;
int maxnum;
int pmax;
int totalnumber = 0;
char hostcmd[2];
char readycmd[2];

int handler_checkactive(){

    char cmd[2];
    while(recv(soc, cmd, 2, 0) > 0){
        if (cmd[0] == CMD_EXIT){
            restart();
        }
    }

}

int restart(){

    printf("\n[Server disconnect!, Initiate Winsock Cleanup...]\n");
    WSACleanup();
    printf("- Unload Winsock 2.2");
    printf("[Process successfully restart!, Rebooting server]");
    Sleep(3000);
    system("cls");
    main();

}

int exit_handler(){

    printf("using handler\n");
    char cmd[2];
    while(recv(soc, cmd, 2, 0) > 0){
        if(cmd[0] == CMD_EXIT){
            printf("\n[Server disconnect!, Initiate Winsock Cleanup...]\n");
            WSACleanup();
            printf("- Unload Winsock 2.2");
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

int handler_clientexit(){

    char cmd[2];
    sprintf(cmd,"%c%d", CMD_EXIT, 0);
    send(soc,cmd,2,0);
    exit(0);

}

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

    int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
    printf("Enter server IP\n");
    scanf("%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
    char ipaddr[16];
    sprintf(ipaddr,"%d.%d.%d.%d",(int)ip1,(int)ip2,(int)ip3,(int)ip4);
    printf("%s", ipaddr);

	soc = socket (AF_INET,SOCK_STREAM,0);

	printf("- Connecting to server...");

    ser.sin_family = AF_INET;
    ser.sin_port = htons (5555);	// RPSS port is 5555
    ser.sin_addr.s_addr = inet_addr (ipaddr);

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
    signal(SIGBREAK, &handler_clientexit);

    printf("[You're host player!...Creating lobby]\n");
    printf("Please configure lobby setting\n\n");

    int sum_players = check_num_player();
    int sum_net = htonl(sum_players);
    handler_clientready();
    send(soc, (const char*)&sum_net, sizeof(sum_players),0);


    int max_num = num_for_game_over();
    int max_net = htonl(max_num);
    handler_clientready();
    send(soc, (const char*)&max_net, sizeof(max_num), 0);


    printf("\n[Lobby successfully configured!] \n\n");

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
            playerno = ntohl(pnum_net);
            printf("\nYou're player %d", playerno);
            break;
    }
}

int waitmode(){

    char cmd[2];
    printf("\n\n[Waiting for other players...]");
    handler_checkactive();
    exit_handler();
    while(recv(soc, cmd, 2, 0) > 0){
        if(cmd[0] == CMD_START){
            break;
        }
    }
}

int check_num_player(){
    int sum_players = 0;
    char check_char;
    printf("   Enter Integer 2-4 Player : ");
    re_Intplayer:
    while(((scanf("%d%c", &sum_players, &check_char)!=2 || check_char!='\n') && clear_stdin()))
    {
        system("cls");
        printf("\n\t\t!Notice! : Input Type Of Sum Players ERROR\n");
        printf("\n  Please Try again Enter Integer 2-4 player : ");
        goto re_Intplayer;
    }

    if(sum_players < 2 || sum_players > 4)
    {
        system("cls");
        printf("\n\t\t!Notice! : Number For Sum Players Out Of Range\n");
        printf("\n  Please Try Again Enter Integer 2-4 player : ");
        goto re_Intplayer;
    }
    return sum_players;
}

int clear_stdin(){
    while(getchar()!='\n')
    {
        return 1;
    }
}

int waitinturn(){

    printf("Wait in turn...\n");
    char recvcmd[2];
    while(recv(soc, recvcmd, 2, 0) > 0){
        printf("Hello from server!\n");
        if(recvcmd[0] == GAME_ONE){
                totalnumber += 1;
            }
        if(recvcmd[0] == GAME_TWO){
                totalnumber += 2;
            }
        if(recvcmd[0] == GAME_ONE){
                totalnumber += 3;
            }
    break;
    }
}


int num_for_game_over(){
    int num_over = 0;
    char check_char;
    printf("  Please Enter integer for check game over : ");
    re_checkIntNover:
    while(((scanf("%d%c", &num_over, &check_char) != 2 || check_char != '\n') && clear_stdin()))
    {
        system("cls");
        printf("\n\t\t!Notice! : Number For Check Game over is not correct\n");
        printf("\n  Please try again Enter integer for check game over : ");
        goto re_checkIntNover;
    }

    if(num_over < 20)
    {
        system("cls");
        printf("\n\t\t!Notice! : Number For Check Game over Out Of Range\n");
        printf("\n  Please Try Again Enter Integer For Check Game Over (Number >= 20) : ");
        goto re_checkIntNover;
    }
    return num_over;
}

int game_start(){

    printf("\n\nGame is starting...");
    char cmd[2];
    restart:
    printf("Process Restart!\n");
    while(recv(soc, cmd, 2, 0) > 0){
        if(cmd[0] == CMD_TURN){
            int feedback = check_gamenum();
            if(feedback == 1){
                char feedbackcmd[2];
                sprintf(feedbackcmd,"%c%d",GAME_ONE,0);
                send(soc, feedbackcmd, 2, 0);
                waitinturn();
                goto restart;
            }
            if(feedback == 2){
                char feedbackcmd[2];
                sprintf(feedbackcmd,"%c%d",GAME_TWO,0);
                send(soc, feedbackcmd, 2, 0);
                waitinturn();
                goto restart;
            }
            if(feedback == 3){
                char feedbackcmd[2];
                sprintf(feedbackcmd,"%c%d",GAME_THREE,0);
                send(soc, feedbackcmd, 2, 0);
                waitinturn();
                goto restart;
            }
        }

        if(cmd[0] == CMD_WAIT){
            printf("Waiting input from server\n");
            char recvcmd[2];
            while(recv(soc, recvcmd, 2, 0) > 0){
                printf("Hello from server\n");
                if(recvcmd[0] == GAME_ONE){
                        totalnumber += 1;
                        goto restart;
                }
                if(recvcmd[0] == GAME_TWO){
                        totalnumber += 2;
                        goto restart;
                }
                if(recvcmd[0] == GAME_THREE){
                        totalnumber += 3;
                        goto restart;
                }
            }
        }

        if(cmd[0] == CMD_WIN){
            if (hostcmd[0] == CMD_HOST){
                endmode_host();
                break;
            }
            if (hostcmd[0] == CMD_JOIN){
                endmode_client();
                break;
            }
        }
        if (cmd[0] == CMD_LOSE){
            if (hostcmd[0] == CMD_HOST){
                endmode_hostlose();
                break;
            }
            if (hostcmd[0] == CMD_JOIN){
                endmode_lose();
                break;
            }
        }
    }


}

int check_gamenum(){

        int range_input = 0;
        char check_char;

        printf("  |Player %d| Please Enter your range number (1-3) : ", playerno);
        while(((scanf("%d%c", &range_input, &check_char)!=2 || check_char!='\n') && clear_stdin()))
        {
            system("cls");
            printf("\n\t\t!Notice! : Type of input ERROR\n");
            //printf("\n  NUMBER : %d / %d\n\n", check, num_over);
            printf("  |Player %d| Please try again Enter your number type integer (1-3) : ", playerno);
        }
        printf("\n-------------------------------------------------------------------\n");
        while(range_input < 1 || range_input > 3)
        {
            printf("\n\t\t!Notice! : Out of range number\n");
            //printf("\n  NUMBER : %d / %d\n\n", check, num_over);
            printf("  |Player %d| Please try again enter range number (1-3) : ", playerno);
            while(((scanf("%d%c", &range_input, &check_char)!=2 || check_char!='\n') && clear_stdin()))
            {
                system("cls");
                printf("\n\t\t!Notice! : Type of input ERROR\n");
                //printf("\n  NUMBER : %d / %d\n\n", check, num_over);
                printf("  |Player %d| Please try again Enter your number type integer (1-3) : ", playerno);
            }
            printf("\n-------------------------------------------------------------------\n");
        }

        return range_input;

}

int endmode_lose(){

    printf("You lose! Player %d, Better luck next time.", playerno);
    printf("[Waiting for host to select...]");

}

int endmode_hostlose(){

    printf("You lose! Player %d, Better luck next time.", playerno);
    printf("[Waiting for host to select...]");

}

int endmode_client(){

    printf("Congratulations! Player %d, You win the Viginti.\n", playerno);
    printf("[Waiting for host to select...]");

}

int endmode_host(){

    printf("Congratulations! Player 1, You win the Viginti.\n");
    printf("Do you want to play again?");

}

int handler_clientready(){

    int handler = send(soc, readycmd, 2, 0);
    if((handler == 0) || (handler == SOCKET_ERROR)){
        system("cls");
        printf("\n[Error: Server Disconnected, Game will restart soon.]\n");
        WSACleanup();
        Sleep(7500);
        system("cls");
        main();
    }
    return 0;

}

int main(){

    sprintf(readycmd,"%c%d",CMD_READY,0);
    home_start();
    sock_start();
    while(recv(soc,hostcmd,2,0) > 0)
    {
        if(hostcmd[0] == CMD_HOST)
        {
            hostmode();
        }
        if(hostcmd[0] == CMD_JOIN)
        {
            joinmode();
        }
        break;
    }
    waitmode();
    game_start();
    getch();

}

