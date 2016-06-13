#include <sstream>
#include <stdint.h>
#include <stdio.h>

#include "../shared/socket.h"

#include "nodes.h"
#include "state.h"

void forward(const char *message) {
    if (strcmp(right.ip, self.ip) == 0 && strcmp(right.port, self.port) == 0) {
        return;
    }

    int8_t sockfd = -1;
    connect_to_server(&sockfd, right.ip, right.port);
    send_to_socket(sockfd, message);
    disconnect_from_server(sockfd);
}

void add_node(char *ip, char *port) {
    addr_info old_right;
    if (strcmp(right.ip, self.ip) == 0 && strcmp(right.port, self.port) == 0) {
        left = addr_info(ip, port);
        old_right = self;
    } else {
        old_right = right;
    }

    right = addr_info(ip, port);

    std::stringstream ss;
    ss << "clonepeer:" << total_peers << ":" << total_content << ":" << self.ip
       << ":" << self.port << ":" << old_right.ip << ":" << old_right.port;
    forward(ss.str().c_str());
}

void clone_node(int peers, int content, char *lip, char *lport, char *rip,
                char *rport) {
    total_peers = ++peers;
    total_content = content;

    left = addr_info(lip, lport);
    right = addr_info(rip, rport);

    std::stringstream ss;
    ss << "connectnewpeer:" << left.ip << ":" << left.port << ":" << self.ip
       << ":" << self.port;
    forward(ss.str().c_str());
}

void connect_node(char *replace_ip, char *replace_port, char *ip, char *port) {
    if (strcmp(self.ip, ip) == 0 && strcmp(self.port, port) == 0) {
        // completed full round of circle
        return;
    }

    if (strcmp(left.ip, replace_ip) == 0 &&
        strcmp(left.port, replace_port) == 0) {
        // replace left with the new peer
        left = addr_info(ip, port);
    }

    ++total_peers;

    std::stringstream ss;
    ss << "connectnewpeer:" << replace_ip << ":" << replace_port << ":" << ip
       << ":" << port;
    forward(ss.str().c_str());
}

void debug_node() {
    printf("Debug info for %s:%s:\n", self.ip, self.port);
    printf("> storing %d/%d content across %d peers\n", 0 /*TODO: my_content*/,
           total_content, total_peers);
    printf("> left peer is %s:%s\n", left.ip, left.port);
    printf("> right peer is %s:%s\n", right.ip, right.port);
}

void init_node(char *ip, char *port) {
    self = left = right = addr_info(ip, port);

    total_peers = 1;
    total_content = 0;
}

void remove_self(char *ip, char *port) {
    if (strcmp(self.ip, ip) != 0 || strcmp(self.port, port) != 0) {
        // error
        return;
    }

    std::stringstream ss;
    ss << "removenode:" << self.ip << ":" << self.port << ":" << left.ip << ":"
       << left.port << ":" << right.ip << ":" << right.port;
    forward(ss.str().c_str());
}

void remove_node(char *remove_ip, char *remove_port, char *lip, char *lport,
                 char *rip, char *rport) {
    if (strcmp(self.ip, remove_ip) == 0 &&
        strcmp(self.port, remove_port) == 0) {
        // completed full round of circle, not
        return;
    }

    --total_peers;

    if (strcmp(right.ip, remove_ip) == 0 &&
        strcmp(right.port, remove_port) == 0) {
        right = addr_info(rip, rport);
    }
    if (strcmp(left.ip, remove_ip) == 0 &&
        strcmp(left.port, remove_port) == 0) {
        left = addr_info(lip, lport);
    }

    std::stringstream ss;
    ss << "removenode:" << remove_ip << ":" << remove_port;
    forward(ss.str().c_str());
}
