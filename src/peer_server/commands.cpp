#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utility>

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

void allkeys(int8_t connectedsock) {
    std::stringstream ss;

    for (auto entry : data) {
        char *key = new char[6];
        sprintf(key, "%d", entry.first);
        ss << key << ",";
    }
    ss << "0";
    send_to_socket(connectedsock, ss.str().c_str());
}

void debug() {
    fprintf(stderr, "Debug info for %s:%s:\n", self.ip, self.port);
    fprintf(stderr, "> storing %zu/%d content across %d peers\n", data.size(),
           total_content, total_peers);
    fprintf(stderr, "> floor and ceiling are %d and %d\n", floor(), ceiling());
    fprintf(stderr, "> left peer is %s:%s\n", left.ip, left.port);
    fprintf(stderr, "> right peer is %s:%s\n", right.ip, right.port);
    if (data.empty()) {
        fprintf(stderr, "> no content\n");
        return;
    }

    fprintf(stderr, "> content is:\n");
    for (auto entry : data) {
        fprintf(stderr, ">>> %d:%s\n", entry.first, entry.second);
    }
}

void lookupcontent(int8_t connectedsock) {
    uint8_t key = atoi(strtok(NULL, ":"));
    char *ip = strtok(NULL, ":");
    char *port = strtok(NULL, ":");
    char *value = lookup_content(key, ip, port);

    send_to_socket(connectedsock, value);
}

void removecontent(int8_t connectedsock) {
    uint8_t key = atoi(strtok(NULL, ":"));
    char *ip = strtok(NULL, ":");
    char *port = strtok(NULL, ":");
    char *response = remove_content(key, ip, port);

    send_to_socket(connectedsock, response);
}

void removepeer() {
    remove_self();
    if (total_peers == 1) {
        exit(0); // kill self if no one else can
    }

    total_content = 0; // allow peers to take all data, will be killed later
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

void decrementcontent() {
    char *ip = strtok(NULL, ":");
    char *port = strtok(NULL, ":");

    decrement_content(ip, port);
}

void givecontent() {
    uint8_t key = atoi(strtok(NULL, ":"));
    char *value = strdup(strtok(NULL, ":"));

    data.insert(std::make_pair(key, value));

    validate_content();
}

void incrementcontent() {
    char *ip = strtok(NULL, ":");
    char *port = strtok(NULL, ":");

    increment_content(ip, port);
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

void takecontent(int8_t connectedsock) {
    std::pair<uint8_t, char *> content = *data.begin();
    data.erase(content.first);

    char *key = new char[sizeof(content.first) + 1];
    sprintf(key, "%d", content.first);

    std::stringstream ss;
    ss << key << ":" << content.second;
    send_to_socket(connectedsock, ss.str().c_str());

    validate_content();
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
    } else if (strcmp(command, "allkeys") == 0) {
        allkeys(connectedsock);
        return;
    } else if (strcmp(command, "debug") == 0) {
        debug();
        return;
    } else if (strcmp(command, "lookupcontent") == 0) {
        lookupcontent(connectedsock);
        return;
    } else if (strcmp(command, "removecontent") == 0) {
        removecontent(connectedsock);
        return;
    } else if (strcmp(command, "removepeer") == 0) {
        removepeer();
        return;
    }

    // internal commands
    if (strcmp(command, "clonepeer") == 0) {
        clonepeer();
        return;
    } else if (strcmp(command, "connectnewpeer") == 0) {
        connectnewpeer();
        return;
    } else if (strcmp(command, "decrementcontent") == 0) {
        decrementcontent();
        return;
    } else if (strcmp(command, "givecontent") == 0) {
        givecontent();
        return;
    } else if (strcmp(command, "incrementcontent") == 0) {
        incrementcontent();
        return;
    } else if (strcmp(command, "removenode") == 0) {
        removenode();
        return;
    } else if (strcmp(command, "takecontent") == 0) {
        takecontent(connectedsock);
        return;
    }

    if (strcmp(command, "die") == 0) {
        exit(0); // shut down gracefully
    }

    // TODO: error handling
}
