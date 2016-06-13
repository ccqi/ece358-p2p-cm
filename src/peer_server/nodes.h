#pragma once

void add_node(char *ip, char *port);

void clone_node(int peers, int content, char *lip, char *lport, char *rip,
                char *rport);

void connect_node(char *replace_ip, char *replace_port, char *ip, char *port);

void init_node(char *ip, char *port);

void remove_self(char *ip, char *port);

void remove_node(char *remove_ip, char *remove_port, char *lip, char *lport,
                 char *rip, char *rport);
