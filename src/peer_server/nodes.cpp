#include <sstream>
#include <stdint.h>
#include <stdio.h>

#include "../shared/socket.h"

#include "nodes.h"

addr_info self, left, right;

int total_peers;
int total_content;

void forward(const char *message) {
    if (right.ip == self.ip && right.port == self.port) {
        return;
    }

    int8_t sockfd = -1;
    connect_to_server(&sockfd, right.ip, right.port);
    send_to_socket(sockfd, message);
    disconnect_from_server(sockfd);
}

void add_node(char *ip, char *port) {
    printf("%s:%s adding peer %s:%s\n", self.ip, self.port, ip, port);
    printf("%s:%s right is currently %s:%s\n", self.ip, self.port, right.ip,
           right.port);

    addr_info old_right;
    if (right.ip == self.ip && right.port == self.port) {
        left = addr_info(ip, port);
        old_right = self;
    } else {
        old_right = right;
    }

    right = addr_info(ip, port);
    printf("%s:%s right is now %s:%s\n", self.ip, self.port, right.ip,
           right.port);

    std::stringstream ss;
    ss << "clonepeer:" << total_peers << ":" << total_content << ":" << self.ip
       << ":" << self.port << ":" << old_right.ip << ":" << old_right.port;
    forward(ss.str().c_str());
    printf("%s:%s told %s:%s to clone with right %s:%s\n", self.ip, self.port,
           right.ip, right.port, old_right.ip, old_right.port);
}

void clone_node(int peers, int content, char *lip, char *lport, char *rip,
                char *rport) {
    total_peers = ++peers;
    printf("incremented peers to %d\n", total_peers);
    total_content = content;

    left = addr_info(lip, lport);
    right = addr_info(rip, rport);

    printf("created %s with left %s and right %s\n", self.port, left.port,
           right.port);

    printf("sending connectnewpeer from %s:%s to %s:%s\n", self.ip, self.port,
           right.ip, right.port);
    std::stringstream ss;
    ss << "connectnewpeer:" << left.ip << ":" << left.port << ":" << self.ip
       << ":" << self.port;
    forward(ss.str().c_str());
}

void connect_node(char *replace_ip, char *replace_port, char *ip, char *port) {
    if (self.ip == ip && self.port == port) {
        // completed full round of circle
        return;
    }

    if (left.ip == replace_ip && left.port == replace_port) {
        // replace left with the new peer
        left = addr_info(ip, port);
        printf("%s left is now %s\n", self.port, left.port);
    }

    ++total_peers;
    printf("incremented peers to %d\n", total_peers);
    printf("sending connectnewpeer from %s:%s to %s:%s\n", self.ip, self.port,
           right.ip, right.port);

    std::stringstream ss;
    ss << "connectnewpeer:" << replace_ip << ":" << replace_port << ":" << ip
       << ":" << port;
    forward(ss.str().c_str());
}

void debug_node() {
    printf("Debugging node:\n");
    printf(" node data: %s:%s, with %d/%d content, and %d total peers\n",
           self.ip, self.port, 0/*TODO: my_content*/, total_content,
           total_peers);
    printf(" left data: %s:%s\n", left.ip, left.port);
    printf(" right data: %s:%s\n", right.ip, right.port);
}

void init_node(char *ip, char *port) {
    self = left = right = addr_info(ip, port);
    total_peers = 1;
    total_content = 0;
}

void remove_self(char *ip, char *port) {
    if (self.ip != ip || self.port != port) {
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
    if (self.ip == remove_ip && self.port == remove_port) {
        // completed full round of circle, not
        return;
    }

    --total_peers;
    printf("decremented peers to %d\n", total_peers);

    if (right.ip == remove_ip && right.port == remove_port) {
        right = addr_info(rip, rport);
    }
    if (left.ip == remove_ip && left.port == remove_port) {
        left = addr_info(lip, lport);
    }

    std::stringstream ss;
    ss << "removenode:" << remove_ip << ":" << remove_port;
    forward(ss.str().c_str());
}
