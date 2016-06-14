#include <sstream>

#include "communication.h"
#include "content.h"
#include "nodes.h"
#include "state.h"

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
    forward(ss.str().c_str(), self.ip, self.port);

    validate_content();
}

void connect_node(char *replace_ip, char *replace_port, char *ip, char *port) {
    if (strcmp(left.ip, replace_ip) == 0 &&
        strcmp(left.port, replace_port) == 0) {
        // replace left with the new peer
        left = addr_info(ip, port);
    }

    ++total_peers;

    std::stringstream ss;
    ss << "connectnewpeer:" << replace_ip << ":" << replace_port << ":" << ip
       << ":" << port;
    forward(ss.str().c_str(), ip, port);

    validate_content();
}

void remove_self() {
    std::stringstream ss;
    ss << "removenode:" << self.ip << ":" << self.port << ":" << left.ip << ":"
       << left.port << ":" << right.ip << ":" << right.port;
    forward(ss.str().c_str(), self.ip, self.port);
}

void remove_node(char *remove_ip, char *remove_port, char *lip, char *lport,
                 char *rip, char *rport) {
    --total_peers;

    if (strcmp(left.ip, remove_ip) == 0 &&
        strcmp(left.port, remove_port) == 0) {
        left = addr_info(lip, lport);
    }

    validate_content();

    if (strcmp(right.ip, remove_ip) == 0 &&
        strcmp(right.port, remove_port) == 0) {
        std::stringstream ssdie;
        ssdie << "die";
        forward(ssdie.str().c_str());

        right = addr_info(rip, rport);
    }

    std::stringstream ss;
    ss << "removenode:" << remove_ip << ":" << remove_port;
    forward(ss.str().c_str(), rip, rport);
}
