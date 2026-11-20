#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include "list.h"

#define PORT 9001
#define ACK "ACK"

static volatile sig_atomic_t running = 1;

// Ctrl-C handler: just flag the main loop to stop. Because the handler is
// installed without SA_RESTART, a blocked accept()/recv() returns -1 (EINTR),
// the loop exits, and main() frees everything. (Don't call free() in here.)
static void handle_sigint(int sig)
{
    (void)sig;
    running = 0;
}

int main(int argc, char const* argv[])
{
    int n, val, idx;
    int clientSocket = -1;

    // string store data to recv/send to/from client
    char buf[1024];
    char sbuf[1024];
    char* token;
    char* str;

    // list
    list_t *mylist = list_alloc();  // create the list

    // install the SIGINT handler (no SA_RESTART so blocking calls get interrupted)
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // create server socket
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    if (servSockD == -1) {
        perror("socket");
        list_free(mylist);
        return 1;
    }

    // allow quick restart of the server (avoids "address already in use")
    int opt = 1;
    setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // define server address
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to the specified IP and port
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        perror("bind");
        close(servSockD);
        list_free(mylist);
        return 1;
    }

    // listen for connections
    listen(servSockD, 1);

    // wait for the client
    clientSocket = accept(servSockD, NULL, NULL);
    if (clientSocket == -1) {
        // Ctrl-C while waiting lands here (errno == EINTR)
        running = 0;
    }

    while (running) {
        // recvs messages from client socket
        n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
        if (n <= 0) {
            // 0 = client closed the connection, -1 = error or Ctrl-C (EINTR)
            break;
        }
        buf[n] = '\0';

        memset(sbuf, '\0', sizeof(sbuf));
        token = strtok(buf, " ");

        if (token == NULL) {
            snprintf(sbuf, sizeof(sbuf), "Unknown command");
        }
        else if (strcmp(token, "exit") == 0) {
            break;  // cleanup happens after the loop
        }
        else if (strcmp(token, "get_length") == 0) {
            val = list_length(mylist);
            snprintf(sbuf, sizeof(sbuf), "Length = %d", val);
        }
        else if (strcmp(token, "print") == 0) {
            str = listToString(mylist);
            snprintf(sbuf, sizeof(sbuf), "%s", str);
            free(str);  // listToString returns a malloc'd string
        }
        else if (strcmp(token, "add_back") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                snprintf(sbuf, sizeof(sbuf), "Usage: add_back <value>");
            } else {
                val = atoi(token);
                list_add_to_back(mylist, val);
                snprintf(sbuf, sizeof(sbuf), "%s", ACK);
            }
        }
        else if (strcmp(token, "add_front") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                snprintf(sbuf, sizeof(sbuf), "Usage: add_front <value>");
            } else {
                val = atoi(token);
                list_add_to_front(mylist, val);
                snprintf(sbuf, sizeof(sbuf), "%s", ACK);
            }
        }
        else if (strcmp(token, "add_position") == 0) {
            char* itok = strtok(NULL, " ");
            char* vtok = strtok(NULL, " ");
            if (itok == NULL || vtok == NULL) {
                snprintf(sbuf, sizeof(sbuf), "Usage: add_position <index> <value>");
            } else {
                idx = atoi(itok);
                val = atoi(vtok);
                list_add_at_index(mylist, val, idx);
                snprintf(sbuf, sizeof(sbuf), "%s", ACK);
            }
        }
        else if (strcmp(token, "remove_back") == 0) {
            val = list_remove_from_back(mylist);
            snprintf(sbuf, sizeof(sbuf), "%d", val);
        }
        else if (strcmp(token, "remove_front") == 0) {
            val = list_remove_from_front(mylist);
            snprintf(sbuf, sizeof(sbuf), "%d", val);
        }
        else if (strcmp(token, "remove_position") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                snprintf(sbuf, sizeof(sbuf), "Usage: remove_position <index>");
            } else {
                idx = atoi(token);
                val = list_remove_at_index(mylist, idx);
                snprintf(sbuf, sizeof(sbuf), "%d", val);
            }
        }
        else if (strcmp(token, "get") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                snprintf(sbuf, sizeof(sbuf), "Usage: get <index>");
            } else {
                idx = atoi(token);
                val = list_get_elem_at(mylist, idx);
                snprintf(sbuf, sizeof(sbuf), "%d", val);
            }
        }
        else {
            // covers "menu" (the client prints it locally but still sends it) and typos
            snprintf(sbuf, sizeof(sbuf), "Unknown command");
        }

        // send's messages to client socket
        send(clientSocket, sbuf, sizeof(sbuf), 0);
        memset(buf, '\0', sizeof(buf));
    }

    // graceful shutdown: runs on "exit", client disconnect, or Ctrl-C
    list_free(mylist);
    if (clientSocket != -1) close(clientSocket);
    close(servSockD);

    return 0;
}