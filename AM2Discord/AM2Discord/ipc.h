//
//  ipc.h
//  AM2Discord
//
//  Created by peter-rit on 5/12/20.
//

#ifndef ipc_h
#define ipc_h

#include <uuid/uuid.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <uuid/uuid.h>
#include <stdio.h>
#include <stdbool.h>

struct songInfo
{
    const char* songName;
    const char *artistName;
    const char *albumName;
    const char *state;
    int startTime;
};

struct showInfo
{
    const char* showName;
    bool state;
    int startTime;
};

struct DiscordIPC {
    struct sockaddr_un address;
    const char *platform;
    char *ipc_path;
    unsigned char *client_id;
    pid_t appPid;
    bool connected;
    char *activity;
    int appSocket;
};


int startConnection(void);
void mainFunction(struct songInfo song);
void disconnect(void);
void starterino(void);

#endif /* ipc_h */
