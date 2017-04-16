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
#define CMD_ACTIVE 0x15


SOCKET soc;
struct sockaddr_in ser;
int playerno;
int numplaynet;
int maxnum;
int pmax;
int totalnumber = 0;
char hostcmd[2];
char readycmd[2];
int totalturn = 1;
int turn = 1;

void home_start();
int clear_stdin();
int check_num_player();
int num_for_game_over();
void playgame(int sum_players, int num_over);
int choose_topic();
int open_topic(int num_topic);
//----------------------------------------------------------------------------------------Part Multi

int handler_checkactive(){

    char cmd[2];
    while(recv(soc, cmd, 2, 0) > 0){
        if (cmd[0] == CMD_EXIT){
            restart();
        }
        break;
    }

}

int restart(){

    printf("\n[Server disconnect!, Initiate Winsock Cleanup...]\n");
    WSACleanup();
    printf(">> Unload Winsock 2.2");
    int playerno = 0;
    int maxnum = 0;
    int pmax = 0;
    int totalnumber = 0;
    int totalturn = 1;
    int turn = 1;
    printf("[Process successfully restart!, Restart...]");
    Sleep(3000);
    system("cls");
    main();

}

int exit_handler(){

    char cmd[2];
    while(recv(soc, cmd, 2, 0) > 0){
        if(cmd[0] == CMD_EXIT){
            printf("\n[Server disconnect!, Initiate Winsock Cleanup...]\n");
            WSACleanup();
            printf(">> Unload Winsock 2.2\n\n");
            printf("[Process successfully restart!, Rebooting server]");
            Sleep(3000);
            system("cls");
            main();
        }
        if(cmd[0] == CMD_READY){
            break;
        }
        if(cmd[0] == CMD_ACTIVE){
            break;
        }
        if(cmd[0] == CMD_WAIT){
        }
    }
}

int handler_clientexit(){

    char cmd[2];
    sprintf(cmd,"%c%d", CMD_EXIT, 0);
    send(soc,cmd,2,0);
    exit(0);

}

int sock_start(){

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
    printf(" \n\n\t\t####################################################################\n");
    printf(" \t\t##  ____    ____      _______        ___    __   _________        ##\n");
    printf(" \t\t##  \\   \\  /   / __  | ______|  __  |   \\  |  | |         |  __   ##\n");
    printf(" \t\t##   \\   \\/   / |  | | | ____  |  | |    \\ |  | |__     __| |  |  ##\n");
    printf(" \t\t##    \\      /  |  | | | |__ | |  | |  |\\ \\|  |    |   |    |  |  ##\n");
    printf(" \t\t##     \\    /   |  | | |___| | |  | |  | \\    |    |   |    |  |  ##\n");
    printf(" \t\t##      \\__/    |__| |_______| |__| |__|  \\___|    |___|    |__|  ##\n");
    printf(" \t\t##                                                                ##\n");
    printf(" \t\t####################################################################\n");
    printf(" \n                                          - Online Mode -");
    printf("\n\n                                [ Initialize Winsock 2.2... ]");

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
    printf(" \n                           >> Please enter server IP (XXX.XXX.XXX.XXX) <<\n >> ");
    scanf("%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
    char ipaddr[16];
    sprintf(ipaddr,"%d.%d.%d.%d",(int)ip1,(int)ip2,(int)ip3,(int)ip4);

	soc = socket (AF_INET,SOCK_STREAM,0);

	printf("\n\n                                 [ Connecting to server... ]");

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
    printf("                          [Server successfully connect! - Starting game...]\n\n");
    printf("----------------------------------------------------------------------------------------------------\n\n");

}

int hostmode(){
    char pname[20];
    char max_player[20];
    signal(SIGBREAK, &handler_clientexit);

    printf("                                [You're host player!...Creating lobby]\n");
    printf("                                    Please configure lobby setting\n\n");
    printf("                                   Press any key to start configure");
    getch();


    int sum_players = check_num_player();
    pmax = sum_players;
    int sum_net = htonl(sum_players);

    handler_clientready();
    send(soc, (const char*)&sum_net, sizeof(sum_players),0);


    int max_num = num_for_game_over();
    maxnum = max_num;
    int max_net = htonl(max_num);
    handler_clientready();
    send(soc, (const char*)&max_net, sizeof(max_num), 0);

    playerno = 1;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
    printf("\n>> Lobby successfully configured! \n\n");

    }

int joinmode(){

    printf(">> Welcome to Viginti : Multiplayer Counting Game\n");
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
            printf("\n>> You're player %d", playerno);
            break;
    }
}

int waitmode(){

    char cmd[2];
    printf(">> Waiting for other players...\n");
    handler_checkactive();
    exit_handler();
    while(recv(soc, cmd, 2, 0) > 0){
        if(cmd[0] == CMD_START){
            break;
        }
    }
}

int waitinturn(){

    printf(">> Waiting for other players turn...\n");
    char recvcmd[2];
    handler_checkactive();
    exit_handler();
    while(recv(soc, recvcmd, 2, 0) > 0){
        if(recvcmd[0] == GAME_ONE){
                totalnumber += 1;
                printf(">> Player %d input 1 | Total %d/%d\n\n",turn,totalnumber,maxnum);
            }
        if(recvcmd[0] == GAME_TWO){
                totalnumber += 2;
                printf(">> Player %d input 2 | Total %d/%d\n\n",turn,totalnumber,maxnum);
            }
        if(recvcmd[0] == GAME_THREE){
                totalnumber += 3;
                printf(">> Player %d input 3 | Total %d/%d\n\n",turn,totalnumber,maxnum);
            }
        if(recvcmd[0] == CMD_WIN){
                endmode_win();
            }
        if(recvcmd[0] == CMD_LOSE){
                endmode_lose();
            }
        break;
    }
}


int game_start(){

    printf("\n\nGame is starting...\n");
    char cmd[2];
    restart:
    if(turn > pmax){
        turn = 1;
    }
    handler_checkactive();
    exit_handler();
    while(recv(soc, cmd, 2, 0) > 0){
        if(cmd[0] == CMD_TURN){
            printf("  [Turn %d | Total number %d/%d]\n\n",totalturn, totalnumber, maxnum);
            int feedback = check_gamenum();
            handler_clientready();
            if(feedback == 1){
                char feedbackcmd[2];
                sprintf(feedbackcmd,"%c%d",GAME_ONE,0);
                send(soc, feedbackcmd, 2, 0);
                waitinturn();
                turn ++;
                totalturn ++;
                goto restart;
            }
            if(feedback == 2){
                char feedbackcmd[2];
                sprintf(feedbackcmd,"%c%d",GAME_TWO,0);
                send(soc, feedbackcmd, 2, 0);
                waitinturn();
                turn ++;
                totalturn ++;
                goto restart;
            }
            if(feedback == 3){
                char feedbackcmd[2];
                sprintf(feedbackcmd,"%c%d",GAME_THREE,0);
                send(soc, feedbackcmd, 2, 0);
                waitinturn();
                turn ++;
                totalturn ++;
                goto restart;
            }
        }

        if(cmd[0] == CMD_WAIT){
            printf(">> Waiting for other players turn...\n\n");
            handler_checkactive();
            exit_handler();
            char recvcmd[2];
            while(recv(soc, recvcmd, 2, 0) > 0){
                if(recvcmd[0] == GAME_ONE){
                        totalnumber += 1;
                        printf(">> Player %d input 1 | Total %d/%d\n\n",turn,totalnumber,maxnum);
                        turn++;
                        totalturn++;
                        goto restart;
                }
                if(recvcmd[0] == GAME_TWO){
                        totalnumber += 2;
                        printf(">> Player %d input 2 | Total %d/%d\n\n",turn,totalnumber,maxnum);
                        turn++;
                        totalturn++;
                        goto restart;
                }
                if(recvcmd[0] == GAME_THREE){
                        totalnumber += 3;
                        printf(">> Player %d input 3 | Total %d/%d\n\n",turn,totalnumber,maxnum);
                        turn++;
                        totalturn++;
                        goto restart;
                }
                if(recvcmd[0] == CMD_WIN){
                        endmode_win();
                }
                if(recvcmd[0] == CMD_LOSE){
                        endmode_lose();
                }
            }
        }

    }


}

int check_gamenum(){

        int range_input = 0;
        char check_char;

        printf(">> |Player %d| Please Enter your range number (1-3) : ", playerno);
        while(((scanf("%d%c", &range_input, &check_char)!=2 || check_char!='\n') && clear_stdin()))
        {
            printf("\n\t\t!Notice! : Type of input ERROR\n");
            //printf("\n  NUMBER : %d / %d\n\n", check, num_over);
            printf(">> |Player %d| Please try again Enter your number type integer (1-3) : ", playerno);
        }
        printf("\n-------------------------------------------------------------------\n");
        while(range_input < 1 || range_input > 3)
        {
            printf("\n\t\t!Notice! : Out of range number\n");
            //printf("\n  NUMBER : %d / %d\n\n", check, num_over);
            printf(">> |Player %d| Please try again enter range number (1-3) : ", playerno);
            while(((scanf("%d%c", &range_input, &check_char)!=2 || check_char!='\n') && clear_stdin()))
            {
                printf("\n\t\t!Notice! : Type of input ERROR\n");
                //printf("\n  NUMBER : %d / %d\n\n", check, num_over);
                printf(">> |Player %d| Please try again Enter your number type integer (1-3) : ", playerno);
            }
            printf("\n-------------------------------------------------------------------\n");
        }

        return range_input;

}

int endmode_lose(){

    system("cls");
    system("MODE 100, 30");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0C);
    printf("\n");
    printf("        _______                                      ___    ___                                     \n");
    printf("       /   __  \\                                    |   \\  /   |                               \n");
    printf("       |  |__|  | ====== ====== ==  == ==   ======  |  |\\\\//|  | ====== ====== ====== ==   ==  \n");
    printf("       |      __| ||     ||     ||  || ||     ||    |  | \\/ |  | ||  ||   ||   ||     ||   ||  \n");
    printf("       |      \\   ||==== ====== ||  || ||     ||    |  |    |  | ||==||   ||   ||     ||===||  \n");
    printf("       |  |\\   \\  ||         || ||__|| ||     ||    |  |    |  | ||  ||   ||   ||     ||   || \n");
    printf("       |__| \\___| ||==== ====== |====| ||==== ||    |__|    |__| ||  ||   ||   ||==== ==   ==  \n\n\n\n");
    printf(" \t\t\t\t=====================================================\n");
    printf(" \t\t\t\t=                                                   =\n");
    printf(" \t\t\t\t=    Player %d you're lose!, Better luck next time   =\n", playerno);
    printf(" \t\t\t\t=                                                   =\n");
    printf(" \t\t\t\t=====================================================\n\n\n\n");
    printf("----------------------------------------------------------------------------------------------------\n\n\n");
    Sleep(2000);
    restart();

}

int endmode_win(){

    system("cls");
    system("MODE 100, 30");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
    printf("\n");
    printf("        _______                                      ___    ___                                     \n");
    printf("       /   __  \\                                    |   \\  /   |                               \n");
    printf("       |  |__|  | ====== ====== ==  == ==   ======  |  |\\\\//|  | ====== ====== ====== ==   ==  \n");
    printf("       |      __| ||     ||     ||  || ||     ||    |  | \\/ |  | ||  ||   ||   ||     ||   ||  \n");
    printf("       |      \\   ||==== ====== ||  || ||     ||    |  |    |  | ||==||   ||   ||     ||===||  \n");
    printf("       |  |\\   \\  ||         || ||__|| ||     ||    |  |    |  | ||  ||   ||   ||     ||   || \n");
    printf("       |__| \\___| ||==== ====== |====| ||==== ||    |__|    |__| ||  ||   ||   ||==== ==   ==  \n\n\n\n");
    printf(" \t\t\t=========================================================\n");
    printf(" \t\t\t=                                                       =\n");
    printf(" \t\t\t=    Congratulations Player %d !, You won the Viginti.   =\n", playerno);
    printf(" \t\t\t=                                                       =\n");
    printf(" \t\t\t=========================================================\n\n\n\n");
    printf("----------------------------------------------------------------------------------------------------\n\n\n");
    Sleep(2000);
    restart();

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

//----------------------------------------------------------------------------------------Part Multi

int choose_topic()
{
    int chose = 0;
    char check_char;
    re_chose:
    system("MODE 100,30");
    printf("\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
    printf(" \t\t####################################################################\n");
    printf(" \t\t##  ____    ____      _______        ___    __   _________        ##\n");
    printf(" \t\t##  \\   \\  /   / __  | ______|  __  |   \\  |  | |         |  __   ##\n");
    printf(" \t\t##   \\   \\/   / |  | | | ____  |  | |    \\ |  | |__     __| |  |  ##\n");
    printf(" \t\t##    \\      /  |  | | | |__ | |  | |  |\\ \\|  |    |   |    |  |  ##\n");
    printf(" \t\t##     \\    /   |  | | |___| | |  | |  | \\    |    |   |    |  |  ##\n");
    printf(" \t\t##      \\__/    |__| |_______| |__| |__|  \\___|    |___|    |__|  ##\n");
    printf(" \t\t##                                                                ##\n");
    printf(" \t\t####################################################################\n\n\n\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x09);
    printf("\t\t\t\t        ####################\n");
    printf("\t\t\t\t        #   1. PLAY GAME   #\n");
    printf("\t\t\t\t        #   2. HOWTO       #\n");
    printf("\t\t\t\t        #   3. CREDIT      #\n");
    printf("\t\t\t\t        #   4. EXIT        #\n");
    printf("\t\t\t\t        ####################\n\n");
    printf("\t\t\t\t\tSELECT TOPIC : ");
    while(((scanf("%d%c", &chose, &check_char) != 2 || check_char != '\n') && clear_stdin()))
    {
        system("cls");
        goto re_chose;
    }
    if(chose == 1 || chose == 2 || chose == 3 || chose == 4)
        return chose;
    else
        system("cls");
        goto re_chose;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x01);
}

int open_topic(int num_topic)
{
    char check[10];
    if(num_topic == 1)
    {
        system("cls");
        re_mode:
        system("MODE 100, 30");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
        printf(" \n\n\t\t#####################################################################\n");
        printf(" \t\t##      _______________    _________   _______     _________       ##\n");
        printf(" \t\t##     /               \\  /   ___   | /   ___ \\   /  ______/       ##\n");
        printf(" \t\t##     |    __    __    | |  |   |  | |  |   | \\  |  |___          ##\n");
        printf(" \t\t##     |   |  |  |  |   | |  |   |  | |  |   |  \\ |   ___|         ##\n");
        printf(" \t\t##     |   |  |  |  |   | |  |___|  | |  |___|  / |  |______       ##\n");
        printf(" \t\t##     |   |  |__|  |   | |         | |        /  |         |      ##\n");
        printf(" \t\t##     |___|        |___| |_________/ |_______/   |_________/      ##\n");
        printf(" \t\t##                                                                 ##\n");
        printf(" \t\t#####################################################################\n\n\n");
        printf(" \t\t\t\t\t 1. Couch Mode\n\n");
        printf(" \t\t\t\t\t 2. Online Mode\n\n");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x01);
        printf(" \t\t\t\t Please Select Mode : ");
        scanf("%s", check);
        if((strcmp(check, "1") == 0))
        {
            main_offline();
        }
        else if((strcmp(check, "2") == 0)){
            system("cls");
            main_online();
        }
        else
        {
            system("cls");
            goto re_mode;
        }
    }

    else if(num_topic == 2)
    {
        re_howto:
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
        printf(" \n\n\t\t#####################################################################\n");
        printf(" \t\t##  ___    ___   _________   ___        ___    ___________ ______  ##\n");
        printf(" \t\t## /   |  /   | /   ___   | /   |  __  /   |  /          //      | ##\n");
        printf(" \t\t## |   |__|   | |  |   |  | |   | /  | |   | /___    ___//   __  | ##\n");
        printf(" \t\t## |          | |  |   |  | |   | |  | |   | ___ |  |    |  |  | | ##\n");
        printf(" \t\t## |    __    | |  |___|  | |   |_|  |_|   ||___||  |    |  |  | | ##\n");
        printf(" \t\t## |   |  |   | |         | |              |     |  |    |  |__| | ##\n");
        printf(" \t\t## |___/  |___/ |_________/ |______________|     |__/    |_______/ ##\n");
        printf(" \t\t##                                                                 ##\n");
        printf(" \t\t#####################################################################\n\n");
        // TEXTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT//
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x01);
        printf("\n\t\t\tEnter \"H\" to home or enter \"X\" to close game : ");
        scanf("%s", check);
        if((strcmp(check, "h") == 0) || (strcmp(check, "H") == 0))
        {
            main();
        }
        else if((strcmp(check, "x") == 0) || (strcmp(check, "X") == 0))
            return 0;
        else
        {
            system("cls");
            goto re_howto;
        }
    }
    else if(num_topic == 3)
    {
        re_credit:
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0A);
        printf(" \n\n\t\t######################################################################\n");
        printf(" \t\t##  ________  _______    _______ ______     ________ _________      ##\n");
        printf(" \t\t## /   ____/ /   __  \\  /   ___/|   _  \\   /       //        /_____ ##\n");
        printf(" \t\t## |  |      |  |__|  | |  |__  |  | \\  \\ /__   __//__    __//  __/ ##\n");
        printf(" \t\t## |  |      |      __| |   __| |  |  |  \\   | |      |  |  /   \\   ##\n");
        printf(" \t\t## |  |____  |  |\\  \\   |  |___ |  |  |  / __| |___   |  |   \\   \\  ##\n");
        printf(" \t\t## |       | |  | \\  \\  |      ||  |__| / /       /   |  |  __\\  /  ##\n");
        printf(" \t\t## |_______| |__|  |__| |______||______/ /_______/    |__| /____/   ##\n");
        printf(" \t\t##                                                                  ##\n");
        printf(" \t\t######################################################################\n\n");
        printf(" \t\t\t\t 1. Thanaset Limtharakorn 59070042\n\n");
        printf(" \t\t\t\t 2. Nuttapong Phantong 59070052\n\n");
        printf(" \t\t\t\t 3. Nattawut Mahma 59070057\n\n");
        printf(" \t\t\t\t 4. Thanathep Thaithae 59070066\n\n");
        printf(" \t\t\t\t 5. NUttttt   xxxxxxx 59070xxx\n\n");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x01);
        printf("\n\t\t\tEnter \"H\" to home or enter \"X\" to close game : ");
        scanf("%s", check);
        if((strcmp(check, "h") == 0) || (strcmp(check, "H") == 0))
        {
            main();
        }
        else if((strcmp(check, "x") == 0) || (strcmp(check, "X") == 0))
            return 0;
        else
        {
            system("cls");
            goto re_credit;
        }
    }
    else if(num_topic == 4)
        {
            return 0;
        }
}

void playgame (int sum_players, int num_over)
{
    system("cls");
    printf(" \n\n\t\t\t =========== ======= =========  ======== ======= \n");
    printf(" \t\t\t |||           |||   |||   |||  |||   ||   |||   \n");
    printf(" \t\t\t |||========   |||   |||===|||  |||===/    |||   \n");
    printf(" \t\t\t         |||   |||   |||   |||  |||   \\\\   ||| \n");
    printf(" \t\t\t ========|||   |||   |||   |||  |||    \\\\  ||| \n");
    int cround = 0, check = 0, sequence = 1;
    char check_ReExit[10];
    while(1)
    {
        int range_input = 0, sequence;
        char check_char;
        if(cround == 0)
            sequence = 1;
        printf("\n\n\n\n\n\t\t\t\t\tTURN PLAYER %d\n\n\n\n", sequence);
        printf("\t|Player %d| Please Enter your range number (1-3) : ", sequence);
        //scanf("%d", &range_input);
        while(((scanf("%d%c", &range_input, &check_char)!=2 || check_char!='\n') && clear_stdin()))
        {
            system("cls");
            printf("\n  NUMBER : %d / %d\n\n", check, num_over);
            printf("\n\n\n\n\t\t\t\t!Notice! : Type of input ERROR\n\n\n");
            printf("\t\t\t\t\tTURN PLAYER %d\n\n\n\n", sequence);
            printf("\t|Player %d| Please try again Enter your number type integer (1-3) : ", sequence);
        }
        while(range_input < 1 || range_input > 3)
        {
            system("cls");
            printf("\n  NUMBER : %d / %d\n\n", check, num_over);
            printf("\n\n\n\n\t\t\t\t!Notice! : Out of range number\n\n\n");
            printf("\t\t\t\t\tTURN PLAYER %d\n\n\n\n", sequence);
            printf("\t|Player %d| Please try again enter range number (1-3) : ", sequence);
            while(((scanf("%d%c", &range_input, &check_char)!=2 || check_char!='\n') && clear_stdin()))
            {
                system("cls");
                printf("\n  NUMBER : %d / %d\n\n", check, num_over);
                printf("\n\n\n\n\t\t\t\t!Notice! : Type of input ERROR\n\n\n");
                printf("\t\t\t\t\tTURN PLAYER %d\n\n\n\n", sequence);
                printf("\t|Player %d| Please try again Enter your number type integer (1-3) : ", sequence);
            }
        }
        check += range_input;
        if(check >= num_over)
        {
            system("cls");
            printf("\n\n\n\n  NUMBER : %d / %d\n\n", check, num_over);
            system("MODE 100, 30");
            printf("\n");
            printf("        _______                                      ___    ___                                \n");
            printf("       /   __  \\                                    |   \\  /   |                               \n");
            printf("       |  |__|  | ====== ====== ==  == ==   ======  |  |\\\\//|  | ====== ====== ====== ==   ==  \n");
            printf("       |      __| ||     ||     ||  || ||     ||    |  | \\/ |  | ||  ||   ||   ||     ||   ||  \n");
            printf("       |      \\   ||==== ====== ||  || ||     ||    |  |    |  | ||==||   ||   ||     ||===||  \n");
            printf("       |  |\\   \\  ||         || ||__|| ||     ||    |  |    |  | ||  ||   ||   ||     ||   || \n");
            printf("       |__| \\___| ||==== ====== |====| ||==== ||    |__|    |__| ||  ||   ||   ||==== ==   ==  \n\n\n\n");
            printf("     \t\t\t=====================================================\n");
            printf("     \t\t\t=                                                   =\n");
            printf("     \t\t\t=    Player %d you're lose!, Better luck next time   =\n", sequence);
            printf("     \t\t\t=                                                   =\n");
            printf("     \t\t\t=====================================================\n\n\n\n");
            printf("----------------------------------------------------------------------------------------------------\n\n\n");
            break;
        }
        else
        {
            sequence += 1;
            if(sequence > sum_players)
                sequence = 1;
        }
        system("cls");
        printf("\n  NUMBER : %d / %d\n\n", check, num_over);
        cround += 1;
    }
    re_check:
    printf("\n\n\t\tEnter \"R\" to Play Again , Enter \"H\" to Home Menu or Enter \"X\" to Exit\n\n\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x01);
    printf("\t\t\t\t\tPlease Enter : ");
    scanf("%s", check_ReExit);
    if((strcmp(check_ReExit, "h") == 0) || (strcmp(check_ReExit, "H") == 0))
        main();
    else if((strcmp(check_ReExit, "x") == 0) || (strcmp(check_ReExit, "X") == 0))
        return 0;
    else if((strcmp(check_ReExit, "R") == 0) || (strcmp(check_ReExit, "r") == 0))
        main_offline();
    else
    {
        system("cls");
        goto re_check;
    }
}

int clear_stdin()
{
    while(getchar()!='\n')
    {
        return 1;
    }
}

int check_num_player()
{
    int sum_players = 0;
    char check_char;
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x09);
    printf("\n\n\n\n\n\n\n\n\n\t\t\t\t\t+++++++++++++++++++++++++++++++\n");
    printf("\t\t\t\t\t+                             +\n");
    printf("\t\t\t\t\t+  How many players required? +\n");
    printf("\t\t\t\t\t+                             +\n");
    printf("\t\t\t\t\t++++++++++++++++++++++++++++++\n\n\n\n");
    printf("\t\t\tEnter 2-4 Player : ");
    re_Intplayer:
    while(((scanf("%d%c", &sum_players, &check_char)!=2 || check_char!='\n') && clear_stdin()))
    {
        system("cls");
        printf("\n\n\n\n\n\n\n\n\t\t\t    !Notice! : Input Type Of Sum Players ERROR\n\n\n");
        printf("\n\n\n\n\n\n\n\n\n\t\t\t\t\t+++++++++++++++++++++++++++++++\n");
        printf("\t\t\t\t\t+                             +\n");
        printf("\t\t\t\t\t+  How many players required? +\n");
        printf("\t\t\t\t\t+                             +\n");
        printf("\t\t\t\t\t++++++++++++++++++++++++++++++\n\n\n\n");
        printf("\t\t\tPlease Try again enter 2-4 player : ");
        goto re_Intplayer;
    }

    if(sum_players < 2 || sum_players > 4)
    {
        system("cls");
        printf("\n\n\n\n\n\n\n\n\t\t\t    !Notice! : Number For Sum Players Out Of Range\n\n\n");
        printf("\n\n\n\n\n\n\n\n\n\t\t\t\t\t+++++++++++++++++++++++++++++++\n");
        printf("\t\t\t\t\t+                             +\n");
        printf("\t\t\t\t\t+  How many players required? +\n");
        printf("\t\t\t\t\t+                             +\n");
        printf("\t\t\t\t\t++++++++++++++++++++++++++++++\n\n\n\n");
        printf("\t\t\tPlease Try again enter 2-4 player : ");
        goto re_Intplayer;
    }
    return sum_players;
}

int num_for_game_over()
{
    system("cls");
    int num_over = 0;
    char check_char;
    printf("\n\n\n\n\n\n\n\n\n\t\t\t\t+++++++++++++++++++++++++++++++++++++\n");
    printf("\t\t\t\t+                                   +\n");
    printf("\t\t\t\t+      Enter your ending number     +\n");
    printf("\t\t\t\t+                                   +\n");
    printf("\t\t\t\t+++++++++++++++++++++++++++++++++++++\n\n\n\n");
    printf("\t\t\tPlease Enter Integer For Check Game Over : ");
    re_checkIntNover:
    while(((scanf("%d%c", &num_over, &check_char) != 2 || check_char != '\n') && clear_stdin()))
    {
        system("cls");
        printf("\n\n\n\n\n\n\n\n\t\t\t!Notice! : Number For Check Game over is not correct\n\n\n");
        printf("\n\n\n\n\n\n\n\n\n\t\t\t\t+++++++++++++++++++++++++++++++++++++\n");
        printf("\t\t\t\t+                                   +\n");
        printf("\t\t\t\t+      Enter your ending number     +\n");
        printf("\t\t\t\t+                                   +\n");
        printf("\t\t\t\t+++++++++++++++++++++++++++++++++++++\n\n\n\n");
        printf("\t\tPlease Try Again Enter Integer For Check Game Over : ");
        goto re_checkIntNover;
    }

    if(num_over < 20)
    {
        system("cls");
        printf("\n\n\n\n\n\n\n\n\t\t\t!Notice! : Number For Check Game over Out Of Range\n\n\n");
        printf("\n\n\n\n\n\n\n\n\n\t\t\t\t+++++++++++++++++++++++++++++++++++++\n");
        printf("\t\t\t\t+                                   +\n");
        printf("\t\t\t\t+      Enter your ending number     +\n");
        printf("\t\t\t\t+                                   +\n");
        printf("\t\t\t\t+++++++++++++++++++++++++++++++++++++\n\n\n\n");
        printf("\tPlease Try Again Enter Integer For Check Game Over (Number >= 20) : ");
        goto re_checkIntNover;
    }
    system("cls");
    return num_over;
}

int main_offline()
{
    int sum_players, num_over;
    sum_players = check_num_player();
    num_over = num_for_game_over();
    playgame(sum_players, num_over);
}

int main_online()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0B);
    sprintf(readycmd,"%c%d",CMD_READY,0);
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

int main()
{
    int check_topic;
    check_topic = choose_topic();
    open_topic(check_topic);
    return 0;
}
