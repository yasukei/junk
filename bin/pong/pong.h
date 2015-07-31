#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#define BUFSIZE 1500

char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];

char* host;
int nsent;
pid_t pid;
int sockfd;
int verbose;

void proc_v4(char*, ssize_t, struct timeval*);
void proc_v6(char*, ssize_t, struct timeval*);
void send_v4(void);
void send_v6(void);
void readloop(void);
void sig_alrm(int);
void tv_sub(struct timeval*, struct timeval*);

struct proto {
	void (*fproc)(char*, ssize_t, struct timeval*);
	void (*fsend)(void);
	struct sockaddr* sasend;
	struct sockaddr* sarecv;
	socklen_t salen;
	int icmpproto;
}*pr;

#ifdef IPV6
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#endif
