//virtual device
#define MTU 1500
int tun_attach(char*);

//vlc
int vlc_create();
int vlc_close(int);
int tx_init(int,char*,int);
int tx_send(int,char*,int);
int rx_init(int,int);
int rx_recv(int,char*,int);
