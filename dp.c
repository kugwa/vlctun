#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include "vlctun.h"

typedef struct{
	int quit; //set this to 1 to tell all threads to stop
	char name[IFNAMSIZ]; //the name of the tun device
	int fd; //use this fd to access the tun device
	int vfd; //this fd simulates vlc
}vt;  

void *tx(void *arg) //read from tunX and deliver to vlc
{
	vt *a=(vt*)arg;
	int nread;
	char pkt[MTU];
	
	while(a->quit==0 && (nread=read(a->fd,pkt,MTU))>0){
		tx_send(a->vfd,pkt,nread);
		fprintf(stderr,"tx(): %d bytes sent\n",nread);
	}
}

void *rx(void *arg) //receive from vlc and deliver to tunX
{
	vt *a=(vt*)arg;
	int nrecv;
	char pkt[MTU];
	
	while(a->quit==0 && (nrecv=rx_recv(a->vfd,pkt,MTU))>0){
		fprintf(stderr,"rx(): %d bytes received\n",nrecv);
		write(a->fd,pkt,nrecv);
	}
}


int main(int argc,char **argv)
{
	pthread_t ttid,rtid;
	vt arg;
	
	//argc == 5 if duplex, 4 if tx, 3 if rx
	if(argc<3 || argc>5){
		puts("Usage: vlc LOCAL_DEV LOCAL_PORT REMOTE_ADDR REMOTE_PORT");
		puts("       vlc LOCAL_DEV REMOTE_ADDR REMOTE_PORT");
		puts("       vlc LOCAL_DEV LOCAL_PORT");
		return -1;
	}
	
	//attach to the given tun device
	strncpy(arg.name,argv[1],IFNAMSIZ);
	arg.fd=tun_attach(arg.name);
	if(arg.fd<0 || strcmp(argv[1],arg.name)){
		fprintf(stderr,"main(): failed to attach to %s\n",argv[1]);
		return -1;
	}
	
	//vlc initialization
	arg.vfd=vlc_create();
	if(argc==5){
		if(rx_init(arg.vfd,atoi(argv[2]))<0){
			fprintf(stderr,"main(): rx_init failed\n");
			return -1;
		}
		if(tx_init(arg.vfd,argv[3],atoi(argv[4]))<0){
			fprintf(stderr,"main(): tx_init failed\n");
			return -1;
		}
	}
	else if(argc==4){
		if(tx_init(arg.vfd,argv[2],atoi(argv[3]))<0){
			fprintf(stderr,"main(): tx_init failed\n");
			return -1;
		}
	}
	else if(argc==3){
		if(rx_init(arg.vfd,atoi(argv[2]))<0){
			fprintf(stderr,"main(): rx_init failed\n");
			return -1;
		}
	}
	
	//start tx and rx
	arg.quit=0;
	if(argc==5 || argc==4)pthread_create(&ttid,NULL,tx,&arg);
	if(argc==5 || argc==3)pthread_create(&rtid,NULL,rx,&arg);
	
	//press Enter to quit
	getchar();
	arg.quit=1;
	
	//cleanup
	pthread_join(ttid,NULL);
	pthread_join(rtid,NULL);
	vlc_close(arg.vfd);
	close(arg.fd);
	return 0;
}
