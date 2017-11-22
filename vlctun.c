#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/if_tun.h>

int tun_attach(char *dev)
{
	struct ifreq ifr;
	int fd,err;
	
	if((fd=open("/dev/net/tun",O_RDWR))<0){
		fprintf(stderr,"tun_attach(): failed to open the clone device\n");
		return fd;
	}
	
	memset(&ifr,0,sizeof(ifr));
	ifr.ifr_flags=IFF_TUN;
	if(dev[0])strncpy(ifr.ifr_name,dev,IFNAMSIZ);
	
	if((err=ioctl(fd,TUNSETIFF,(void*)&ifr))<0){
		fprintf(stderr,"tun_attach(): failed to attach to %s\n",dev);
		close(fd);
		return err;
	}
	strcpy(dev,ifr.ifr_name);
	return fd;
}

int vlc_create()
{
	return socket(AF_INET,SOCK_DGRAM,0);
}

int vlc_close(int fd)
{
	close(fd);
}

int tx_init(int fd,char *ip,int port)
{
	struct sockaddr_in sa;
	memset(&sa,0,sizeof(sa));
	sa.sin_family=AF_INET;
	inet_pton(AF_INET,ip,&sa.sin_addr);
	sa.sin_port=htons(port);
	return connect(fd,(struct sockaddr*)&sa,sizeof(sa));
}

int tx_send(int fd,char *buf,int size)
{
	return write(fd,buf,size);
}

int rx_init(int fd,int port)
{
	struct sockaddr_in sa;
	memset(&sa,0,sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=htonl(INADDR_ANY);
	sa.sin_port=htons(port);
	return bind(fd,(struct sockaddr*)&sa,sizeof(sa));
}

int rx_recv(int fd,char *buf,int size)
{
	return read(fd,buf,size);
}
