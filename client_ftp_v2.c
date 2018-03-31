#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFSIZE 1000

void getargs(char *word);
void put(int s, char *com);
void get(int s, char *com[]);

int main(){
    int s;
    char *ip;
    in_port_t port;
    struct sockaddr_in myskt;
    struct sockaddr_in skt;
    char buf[1000];
    char *com[100];
    int i = 0;
    int t = -1;

    ip = (char *)malloc(sizeof(char)*100);
    for(int k = 0;k < 100; k++){
       com[k] = (char *)malloc(sizeof(char)*20);
    }

    strcpy(ip, "0.0.0.0");
    //ip = "0.0.0.0";
    port = atoi("10001");
    
    if((s = socket(PF_INET, SOCK_STREAM, 0))<0){
        perror("socket");
        exit(1);
    }
    
    printf("%d\n",s);

    memset(&skt, 0, sizeof skt);
    skt.sin_family = AF_INET;
    skt.sin_port = htons(port);
    skt.sin_addr.s_addr = inet_addr(ip);
//inet_addr
    printf("skt.sin_len %c\n",skt.sin_len);
    printf("skt.sin_family %c\n",skt.sin_family);
    printf("skt.sin_port %o\n", skt.sin_port);

    printf("before connect\n");

    if(connect(s, (struct sockaddr *)&skt, sizeof skt)<0){
        perror("connect");
        exit(1);   
    }
    printf("after connect\n"); 
    for(;;){
        //printf("FTP$");
        for(i=0;i<2;i++){
            getargs(com[i]);
            printf("i = %d\n",i);
        }
        printf("com[1] = %s\n",com[1]);
        if(strcmp(com[0], "put") == 0){
            put(s,com[1]);
        }else if(strcmp(com[0], "get") == 0){
            get(s,&com[1]);
        }
        printf("end ok\n");
    }


}

void getargs(char *com){
    char word[10];
    char ch;
    int n = 0;
    for(;;){
        printf("getargs start\n");
        ch = getc(stdin);
        if(ch == ' '||ch =='\n'){
            printf("getargs;ok\n");
            strcpy(com,word);
            break;
        }else{
            word[n] = ch;
            n++;
        }
    }
}

void put(int s, char *com){
    //printf("ok");

    int n;    
    FILE *fp;
    uint8_t *data;
    char buf[BUFSIZE];
    uint8_t *ftp_pay;
    struct ftp_header{
        uint8_t type;
        uint8_t code;
        uint16_t length;
    };
    struct ftp_header *send_header;
    int error;

    fp = fopen(com,"r");
    n = strlen(com)+1;
    
    printf("com %s",com);

    data = (uint8_t*)malloc(sizeof(struct ftp_header)+sizeof(uint8_t)*n);
    send_header = (struct ftp_header *) data;

    ftp_pay = data + sizeof(struct ftp_header);
    memcpy(ftp_pay,(uint8_t*) com, n);  

    send_header->type = 0x06;
    send_header->code = 0x00;
    send_header->length = htons(n);

    send(s, data,sizeof(struct ftp_header)+n ,0);
   // printf("error = %d", error);

    do{
        n = fread(buf, sizeof(char), BUFSIZE, fp);
        data = (uint8_t*)malloc(sizeof(struct ftp_header)+sizeof(uint8_t)*n);
        send_header = (struct ftp_header *) data;
        ftp_pay = data + sizeof(struct ftp_header);

        memcpy(ftp_pay, buf, n);

        send_header->type = 0x20;
        if(n<BUFSIZE){
            send_header->code = 0x01;
        }else{
            send_header->code = 0x00;
        }
        send_header->length = htons(n);

        send(s,data,sizeof(struct ftp_header)+n,0);

        n -= BUFSIZE;
 
    }while(n >= 0);
}

void get(int a, char *com[]){

}

/*
void get_func(int s, char *com[]) {
	if (s < 2 || 3 < s) {
		fprintf(stderr, "unknown format\n");
		return;
	}

	struct myftp_message message;
	struct myftp_message_data message_data;
	int fd;
	char *fname;

	if (s == 2) fname = com[1];
	else		 fname = com[2];

	set_message_data(&message_data, TYPE_RETR, 0, strlen(com[1]), com[1]);
	send_message_data(&message_data, s);
	recv_message(&message, s);

	if (check_message(&message, TYPE_OK, OK_DATA_S_C)) {
		if (print_error(&message)) return;

		print_message(&message);
		return;
	}

	if ((fd = open(fname, O_WRONLY | O_CREAT | O_EXCL, 0644)) < 0) {
		if (errno != EEXIST) {
			perror("open");

			for (;;) {
				recv_message_data(&message_data, s);
				if (!check_message_data(&message_data, TYPE_DATA, DATA_END)) break;
			}

			return;
		}

		if ((fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
			perror("open");

			for (;;) {
				recv_message_data(&message_data, s);
				if (!check_message_data(&message_data, TYPE_DATA, DATA_END)) break;
			}

			return;
		}
	}

	for (;;) {
				recv_message_data(&message_data, s);
				if (!check_message_data(&message_data, TYPE_DATA, DATA_END)) break;
			}

			return;
		}
	}

	for (;;) {
		if (recv(s, &message_data, MESSAGE_DATA_SIZE, 0) < 0) {
			perror("recv");
			exit(1);
		}

		if (check_message_data(&message_data, TYPE_DATA, DATA_CON)) break;

		write(fd, message_data.data, message_data.length);
	}

	if (check_message_data(&message_data, TYPE_DATA, DATA_END)) {
		print_message_data(&message_data);
		close(fd);
		return;
	}

	write(fd, message_data.data, message_data.length);

	close(fd);
}*/


