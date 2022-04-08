#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in sAddr;
    fd_set readset, testset;
    int listensock;
    int newsock;
    char buffer[25];
    int result;
    int nread;
    int x;
    int val=1;

    listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    result = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (result < 0)
    {
        perror("Server");
        return 0;
    }

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(5000); // port number set to 5000
    sAddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(listensock, (struct sockaddr *)&sAddr, sizeof(sAddr));
    if (result < 0)
    {
        perror("Server");
        return 0;
    }

    result = listen(listensock, 5);
    if (result < 0)
    {
        perror("Server");
        return 0;
    }

    FD_ZERO(&readset);
    FD_SET(listensock, &readset);

    while (1)
    {
        testset = readset;
        result = select(FD_SETSIZE, &testset, NULL, NULL, NULL);
        if (result < 1)
        {
            perror("Server");
            return 0;
        }
        for (x = 0; x < FD_SETSIZE; x++)
        {
            if (FD_ISSET(x, &testset))
            {
                if (x == listensock)
                {

                    char server_msg[100];
                    // en.wikipedia.org/wiki/C_date_and_time_functions

                    // Generating server message
                    strcpy(server_msg, "\nconnected...\n");
                    strcat(server_msg, "...disconnected..\n\n");

                    newsock = accept(listensock, NULL, NULL);
                    FD_SET(newsock, &readset);

                    // Sending server message
                    send(newsock, server_msg, strlen(server_msg), 0);

                    // Finishing connection
                    close(newsock);
                    FD_CLR(newsock, &readset);
                }
            }
        }
    }
}
