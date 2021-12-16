#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
   
#define BUF_SIZE 1000
#define NAME_SIZE 20
   
void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);
   
char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];
   
int main(int argc, char *argv[])
{
   int sock;
   struct sockaddr_in serv_addr;
   pthread_t snd_thread, rcv_thread;
   void * thread_return;
   if(argc!=2) {
      printf("Usage : %s <name>\n", argv[0]);
      exit(1);
    }
   
   sprintf(name, "[%s]", argv[1]);
   sock=socket(PF_INET, SOCK_STREAM, 0);
   
   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family=AF_INET;
   serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
   serv_addr.sin_port=htons(36007);
     
   if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
      error_handling("connect() error");
   
   pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
   pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
   pthread_join(snd_thread, &thread_return);
   pthread_join(rcv_thread, &thread_return);
   close(sock);  
   return 0;
}
   
void * send_msg(void * arg)   // send thread main
{
   time_t ct;
   struct tm tm;
   
   int sock=*((int*)arg);
   char name_msg[NAME_SIZE+BUF_SIZE];
   while(1) 
   {
      fgets(msg, BUF_SIZE, stdin);
      if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")) 
      {
         close(sock);
         exit(0);
      }
      fprintf(stderr, "\033[1;32m");	//글자색을 녹색으로 변경
      /*ct = time(NULL);	//현재 시간을 받아옴
      tm = *localtime(&ct);
      sprintf(name_msg, "[%02d:%02d:%02d]%s>", tm.tm_hour, tm.tm_min, tm.tm_sec, name);//메시지에 현재시간 추가*/
      sprintf(name_msg,"%s %s", name, msg);
      write(sock, name_msg, strlen(name_msg));
   }
   return NULL;
}
   
void * recv_msg(void * arg)   // read thread main
{
   time_t ct;
   struct tm tm;
   
   int sock=*((int*)arg);
   char name_msg[NAME_SIZE+BUF_SIZE];
   int str_len;
   while(1)
   {
     /*ct = time(NULL);	//현재 시간을 받아옴
     tm = *localtime(&ct);
     sprintf(name_msg, "[%02d:%02d:%02d]%s>", tm.tm_hour, tm.tm_min, tm.tm_sec, name);//메시지에 현재시간 추가*/
      fprintf(stderr, "\033[1;33m"); //글자색을 노란색으로 변경
      str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
      if(str_len==-1) 
         return (void*)-1;
      name_msg[str_len]=0;
      fputs(name_msg, stdout);
   }
   return NULL;
}
   
void error_handling(char *msg)
{
   fputs(msg, stderr);
   fputc('\n', stderr);
   exit(1);
}
