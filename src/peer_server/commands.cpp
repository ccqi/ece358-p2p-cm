#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../shared/socket.h"

#include "commands.h"
#include "content.h"
#include "nodes.h"
#include "state.h"

/*
 * client commands
 */
void addcontent(int8_t connectedsock) {
    char *val = strtok(NULL, ":");

    uint8_t key = insert_content(val);

    char *ckey = new char[6];
    sprintf(ckey, "%d", key);
    send_to_socket(connectedsock, ckey);
}

void addpeer() {
    char *ip = strtok(NULL, ":");
    char *port = strtok(NULL, ":");

    add_node(ip, port);
}

void debug() {
    printf("Debug info for %s:%s:\n", self.ip, self.port);
    printf("> storing %lu/%d content across %d peers\n", data.size(),
           total_content, total_peers);
    printf("> left peer is %s:%s\n", left.ip, left.port);
    printf("> right peer is %s:%s\n", right.ip, right.port);
    if (data.size() == 0) {
        printf("> no content\n");
        return;
    }

    printf("> content is:\n");
    for (auto entry : data) {
        printf(">>> %d:%s\n", entry.first, entry.second);
    }
}

void lookupcontent(int8_t connectedsock) {
    uint8_t key = atoi(strtok(NULL, ":"));
    char *value = lookup_content(key, self.ip, self.port);

    send_to_socket(connectedsock, value);
}

void removecontent() {
    uint8_t key = atoi(strtok(NULL, ":"));
    remove_content(key, self.ip, self.port);
}

void removepeer() {
    remove_self();
}

/*
 * internal commands
 */
void clonepeer() {
    int peers = atoi(strtok(NULL, ":"));
    int content = atoi(strtok(NULL, ":"));
    char *lip = strtok(NULL, ":");
    char *lport = strtok(NULL, ":");
    char *rip = strtok(NULL, ":");
    char *rport = strtok(NULL, ":");

    clone_node(peers, content, lip, lport, rip, rport);
}

void connectnewpeer() {
    char *replace_ip = strtok(NULL, ":");
    char *replace_port = strtok(NULL, ":");
    char *ip = strtok(NULL, ":");
    char *port = strtok(NULL, ":");

    connect_node(replace_ip, replace_port, ip, port);
}

void removenode() {
    char *remove_ip = strtok(NULL, ":");
    char *remove_port = strtok(NULL, ":");
    char *lip = strtok(NULL, ":");
    char *lport = strtok(NULL, ":");
    char *rip = strtok(NULL, ":");
    char *rport = strtok(NULL, ":");

    remove_node(remove_ip, remove_port, lip, lport, rip, rport);
}

/*
 * global command responder
 */
void respond(const char *command, int8_t connectedsock) {
    // client commands
    if (strcmp(command, "addcontent") == 0) {
        addcontent(connectedsock);
        return;
    } else if (strcmp(command, "addpeer") == 0) {
        addpeer();
        return;
    } else if (strcmp(command, "debug") == 0) {
        debug();
        return;
    } else if (strcmp(command, "lookupcontent") == 0) {
        lookupcontent(connectedsock);
        return;
    } else if (strcmp(command, "removecontent") == 0) {
        removecontent();
        return;
    } else if (strcmp(command, "removepeer") == 0) {
        removepeer();
        exit(0); // shut down on succesful removal
        return;
    }

    // internal commands
    if (strcmp(command, "clonepeer") == 0) {
        clonepeer();
        return;
    } else if (strcmp(command, "connectnewpeer") == 0) {
        connectnewpeer();
        return;
    } else if (strcmp(command, "removenode") == 0) {
        removenode();
        return;
    }

    // TODO: error handling
}
