//
//  ipc.c
//  AM2Discord
//
//  Created by peter-rit on 5/12/20.
//
//
/*
 OKAY I MADE IF HOPEFULLY VERY SIMPLE CTRL-F FOR "HERE" EACH INSTANCE IS A SPOT THAT NEEDS 
 TO BE FILLED IN AND EACH SPOT SPECIFIES WHAT NEEDS TO BE PUT THERE ONCE THAT IS DONE YOU CAN COMPILE 
 PROCEED WITH THE FOLLOWING INSTRUCTION IN THE GITHUB TO CREATE THE .APP
*/

#include <json-c/json.h>
#include "ipc.h"

#if defined (_WIN32)
    #define PLATFORM_NAME "Windows"
#elif defined (__linux__)
    #define PLATFORM_NAME "Linux"
#elif defined (__unix__)
    #define PLATFORM_NAME "Unix"
#elif defined (__APPLE__)
    #define PLATFORM_NAME "Apple"
#endif


//Define turns on decode printing if set to 1
//Any other value will prevent decode printing
#define DEBUG_DECODE 0

struct DiscordIPC discordIPC;

//Error message printing
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//Gets the platform type, not useful now but maybe later on if this file is extended for other applications
const char* get_platform_name() {
    return PLATFORM_NAME;
}

//Packer function which packs opcode and payload into big endian unsigned long long
unsigned long long packer(int opcode, int payloadLength)
{
    unsigned long long packed = 0;
    unsigned long longboi = payloadLength;

    packed = packed ^ longboi;
    packed = packed << 32;
    packed = packed ^ opcode;
    
    
    return packed;
}

//Gets the IPC Path of the discord and returns it as a c string
char* get_ipc_path() {
    char* path = getenv("TMPDIR");
    char* discord = "discord-ipc-0";
    char *fullPath = calloc(strlen(path)+strlen(discord), sizeof(char));
    
    strcpy(fullPath, path);
    strcat(fullPath, discord);

    
    return fullPath;
}


union opcodeUnion
{
    unsigned long long opcodeLong;
    unsigned char hex[8];
    
};


//Encode function
void encode(union opcodeUnion *opcodeAndLength, unsigned char *payload, unsigned char *encodedPayload)
{
    for (int i = 0; i < sizeof(opcodeAndLength->hex); i++)
    {
        encodedPayload[i] = opcodeAndLength->hex[i];
    }
    
    for (int i = 0; i < strlen((char*)payload); i++)
    {
        encodedPayload[sizeof(opcodeAndLength->hex)+i] = payload[i];
    }
}


//Send function takes in an opcode and a payload. It then uses a helper function to pack the opcode and length
//of payload into a big endian hex value it is then sent over the Unix socket to discord
void appSend(int opcode, unsigned char *payload)
{
    unsigned char encodedPayload[8+strlen((char*)payload)];
    union opcodeUnion opcodeU;
    opcodeU.opcodeLong = packer(opcode, (int) strlen((char*)payload));
    encode(&opcodeU, payload, encodedPayload);
    
    if((send(discordIPC.appSocket, encodedPayload, sizeof(encodedPayload), 0)) == -1)
    {
        perror("Send error ");
        exit(0);
    }
    
}

// Decode Function which takes data from IPC port and decodes it into printed string
void decode()
{
    char buffer[500];
    recv(discordIPC.appSocket, buffer, sizeof(buffer), 0);
    
    if (DEBUG_DECODE == 1)
    {
        for (int i = 0; i < sizeof(buffer); i++)
        {
            printf("%c", buffer[i]);
        }
        
        printf("\n\n\n");
    }

}

//Creates the json object that is sent to the ipc port
void updateActivity(struct songInfo song)
{
    unsigned char* rawUUID = malloc(37);
    uuid_generate(rawUUID);
    json_object *payload = json_object_new_object();
    json_object *jactivity = json_object_new_object();
    json_object *timestamps = json_object_new_object();
    json_object *assets = json_object_new_object();
    json_object *args = json_object_new_object();
    
    json_object *cmd = json_object_new_string("SET_ACTIVITY");
        
    json_object *state = json_object_new_string(song.artistName);
    json_object *details = json_object_new_string(song.songName);
    json_object *large_image = json_object_new_string("LARGE IMAGE NAME HERE");
    json_object *large_text = json_object_new_string("LARGE IMAGE TEXT HERE");
    json_object_object_add(assets,"large_image", large_image);
    json_object_object_add(assets,"large_text", large_text);

    json_object *pid = json_object_new_int(discordIPC.appPid);
    
    json_object *uuid = json_object_new_string((char *)rawUUID);
    free(rawUUID);
    
    const char playing[8] = "PLAYING";
    if (strcmp(song.state, playing) == 0)
    {
        int startTimeCalcuated = (int) (time(NULL) - song.startTime);
        json_object *startTime = json_object_new_int(startTimeCalcuated);
        json_object_object_add(timestamps, "start", startTime);
        json_object *small_image = json_object_new_string("SMALL IMAGE NAME HERE");
        json_object *small_text = json_object_new_string("SMALL TEXT HERE");
        json_object_object_add(assets,"small_image", small_image);
        json_object_object_add(assets,"small_text", small_text);
        json_object_object_add(jactivity, "timestamps", timestamps);
        json_object_object_add(jactivity,"state", state);
        json_object_object_add(jactivity,"details", details);
    }
    else if (strcmp(song.state, "PAUSED") == 0)
    {
        json_object *small_image = json_object_new_string("SMALL IMAGE NAME AGAIN HERE");
        json_object *small_text = json_object_new_string("SMALL IMAGE TEXT AGAIN HERE");
        json_object_object_add(assets,"small_image", small_image);
        json_object_object_add(assets,"small_text", small_text);
        json_object_object_add(jactivity, "timestamps", timestamps);
        json_object_object_add(jactivity,"state", state);
        json_object_object_add(jactivity,"details", details);

    }
    else if (strcmp(song.state, "\0") == 0)
    {
        json_object *nullState = json_object_new_string("vibing on hold");
        json_object *nulldetails = json_object_new_string("🎧");

        json_object_object_add(jactivity,"state", nullState);
        json_object_object_add(jactivity,"details", nulldetails);
        
    }
    
    
    json_object_object_add(jactivity,"assets", assets);
    
    json_object_object_add(args,"activity", jactivity);
    json_object_object_add(args, "pid", pid);
    
    json_object_object_add(payload, "cmd", cmd);
    json_object_object_add(payload, "args", args);
    json_object_object_add(payload, "nonce", uuid);
    
    unsigned char *fullPayload = (unsigned char *) json_object_to_json_string(payload);
    
    appSend(1, fullPayload);
    
    json_object_put(payload);
    
    decode();
    
}


// This starts the connection process to the discord Unix Socket
void handshake()
{
    if (discordIPC.connected != true)
    {

        
        json_object *handshakeMessage = json_object_new_object();
        json_object *json_client_id = json_object_new_string((char*) discordIPC.client_id);
        json_object *json_v = json_object_new_int(1);
        
        json_object_object_add(handshakeMessage, "client_id", json_client_id);
        json_object_object_add(handshakeMessage, "v", json_v);
        
        unsigned char* handShakeMessagePayload = (unsigned char *) json_object_to_json_string(handshakeMessage);
        
        appSend(0, handShakeMessagePayload);
        
        decode();
        discordIPC.connected = true;
    }

}

//used to disconnect from the ipc port
void disconnect()
{
    unsigned char jDisconnect[3] = "{}";
    appSend(2, jDisconnect);
    
    close(discordIPC.appSocket);
    discordIPC.appSocket = 0;
    
    discordIPC.connected = false;
    
}

//starts the connection with the ipc port with the handshake message
int startConnection()
{
    //Defining and building discordIPC struct
    discordIPC.address.sun_family = AF_UNIX;
    discordIPC.platform = get_platform_name();
    discordIPC.ipc_path = get_ipc_path();
    discordIPC.client_id = (unsigned char*) "CLIENT ID HERE";
    discordIPC.appPid = getpid();
    discordIPC.connected = false;
    discordIPC.activity = "";
    
    strncpy(discordIPC.address.sun_path, discordIPC.ipc_path, strlen(discordIPC.ipc_path));
    discordIPC.address.sun_path[strlen(discordIPC.ipc_path)] = '\0';
    
    
    if((discordIPC.appSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }
    
    
    if ((connect(discordIPC.appSocket, (struct sockaddr *) &discordIPC.address, sizeof(discordIPC.address))) < 0)
    {
        perror("Socket connection failed");
        return -1;
    }
    
    
    handshake();
    
    return 0;
}

void mainFunction(struct songInfo song)
{
    
    updateActivity(song);
}


