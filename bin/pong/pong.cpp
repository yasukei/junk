#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>

#include "pong.h"

int datalen = 56;

void proc_v4(char* ptr, ssize_t len, struct timeval* tvrecv)
{
	int hlenl, icmplen;
	double rtt;
	struct ip* ip;
	struct icmp* icmp;
	struct timeval* tvsend;

	ip = (struct ip*)ptr;
	hlenl = ip->ip_hl << 2;

	icmp = (struct icmp*)(ptr + hlenl);
	if((icmplen = len - hlenl) < 8)
	{
		printf("icmplen (%d) < 8\n", icmplen);
		exit(-1);
	}
	if(icmp->icmp_type == ICMP_ECHOREPLY)
	{
		if(icmp->icmp_id != pid)
		{
			return;
		}
		if(icmplen < 16)
		{
			printf("icmplen (%d) < 16\n", icmplen);
			exit(-1);
		}

		tvsend = (struct timeval*)icmp->icmp_data;
		rtt = (tvrecv->tv_sec - tvsend->tv_sec) * 1000.0 + (tvrecv->tv_usec - tvsend->tv_usec) / 1000.0;

		printf("%d bytes from %s: seq%u, ttl=%d, rtt=%.3f ms\n",
				icmplen, "", icmp->icmp_seq, ip->ip_ttl, rtt);
	}
	else if(verbose)
	{
		printf("%d bytes from %s: type = %d, code = %d\n",
				icmplen, "", icmp->icmp_type, icmp->icmp_code);
	}
}

void proc_v6(char* ptr, ssize_t len, struct timeval* tvrecv)
{
	// TODO
}

void sig_alrm(int signo)
{
	pr->fsend();
	alarm(1);
	return;
}

unsigned short in_cksum(unsigned short* addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;
	
	while(nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}
	
	if(nleft == 1)
	{
		*(unsigned char*)(&answer) = *(unsigned char*)w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}

void send_v4(void)
{
	int len;
	struct icmp* icmp;

	icmp = (struct icmp*)sendbuf;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = pid;
	icmp->icmp_seq = nsent++;
	gettimeofday((struct timeval*)icmp->icmp_data, NULL);
	len = 8 + datalen;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = in_cksum((u_short*)icmp, len);
	sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
}

void send_v6(void)
{
	// TODO
}

void readloop(void)
{
	int size;
	char recvbuf[BUFSIZE];
	socklen_t len;
	ssize_t n;
	struct timeval tval;

	sockfd = socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
	setuid(getuid());
	size = 60 * 1024;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	sig_alrm(SIGALRM);

	while(true)
	{
		len = pr->salen;
		n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, pr->sarecv, &len);
		if(n < 0)
		{
			if(errno = EINTR)
			{
				continue;
			}
			else
			{
				printf("recvfrom error\n");
			}
		}
		gettimeofday(&tval, NULL);
		pr->fproc(recvbuf, n, &tval);
	}
}

struct proto proto_v4 = { proc_v4, send_v4, NULL, NULL, 0, IPPROTO_ICMP };
#ifdef IPV6
struct proto proto_v6 = { proc_v6, send_v6, NULL, NULL, 0, IPPROTO_ICMPV6 };
#endif


int main(int argc, char* argv[])
{
	int c;
	int result;
	struct addrinfo* ai;

	opterr = 0;
	while((c = getopt(argc, argv, "v")) != -1)
	{
		switch(c)
		{
			case 'v': 
				verbose++;
				break;
			case '?':
				printf("unrecognized option: %c\n", c);
				exit(-1);
		}
	}
	if(optind != argc - 1)
	{
		printf("usage: pong [-v] <hostname>\n");
		exit(-1);
	}
	host = argv[optind];
	pid = getpid();
	signal(SIGALRM, sig_alrm);

	result = getaddrinfo(host, NULL, NULL, &ai);
	if(result != 0)
	{
		printf("%s", gai_strerror(result));
		return -1;
	}
	printf("PONG %s (%s) : %d data bytes\n", ai->ai_canonname, "", datalen);

	if(ai->ai_family == AF_INET)
	{
		pr = &proto_v4;
	}
#ifdef IPV6
	else if( ai->ai_family == AF_INET6)
	{
		pr = &proto_v6;
	}
#endif
	else
	{
		printf("cannot pong IPv4-mapped IPv6 address\n");
		exit(-1);
	}
	pr->sasend = ai->ai_addr;
	pr->sarecv = (struct sockaddr*)calloc(1, ai->ai_addrlen);
	pr->salen = ai->ai_addrlen;
	readloop();

	return 0;
}

