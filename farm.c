#include "xerrori.h"
#include <sys/types.h>
#include <sys/socket.h>

#define QUI __LINE__,__FILE__

#define PORT 3490

unsigned int qlen = 8;
unsigned int delay = 0;
unsigned int nthread = 1;


void args_checker(unsigned int *file_counter, unsigned int *args_counter, int argc, char *argv[]){
    if(argc<2) {
      printf("Uso: %s file [-q qlen] [-t delay] [-n threads] <file ...> \n",argv[0]);
      exit(1);
  } else{
    for(unsigned int i = 1; i < argc; i++)
      if(argv[i][0] == '-'){
        for(unsigned int j = 1; j < strlen(argv[i]); j++){
          switch(argv[i][j]){
            case 'q':
              if(j < strlen(argv[i])-1){
                printf("Uso: %s file [-q qlen] [-t delay] [-n threads] <file ...> \n", argv[0]);
                exit(1);
              }
              (*args_counter)++;
              qlen = atoi(argv[i+1]);
              i++;

              if(qlen < 1) {
                printf("Uso: %s file [-q qlen] [-t delay] [-n threads] <file ...> \n",argv[0]);
                exit(1);
              }
            break;

            case 't':
              if(j < strlen(argv[i])-1){
                printf("Uso: %s file [-q qlen] [-t delay] [-n threads] <file ...> \n",argv[0]);
                exit(1);
              }
              (*args_counter)++;
              delay = atoi(argv[i+1]);
              i++;
            break;

            case 'n':
              if(j < strlen(argv[i])-1){
                printf("Uso: %s file [-q qlen] [-t delay] [-n threads] <file ...> \n",argv[0]);
                exit(1);
              }
              (*args_counter)++;
              nthread = atoi(argv[i+1]);
              i++;

              if(nthread < 1) {
                printf("Uso: %s file [-q qlen] [-t delay] [-n threads] <file ...> \n",argv[0]);
                exit(1);
              }
          }
        }
      } else {
        (*file_counter)++;
      }

    if((*file_counter) < 1){
      printf("Uso: %s file [-q qlen] [-t delay] [-n threads] <file ...> \n",argv[0]);
      exit(1);
    }  
  }
}

int sock_init_listen(){
  int server_fd;
  struct sockaddr_in address;

  //memset(&address, 0, sizeof(address));

  server_fd = xsocket(AF_INET, SOCK_STREAM, 0, QUI);

  address.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(address.sin_addr));
	address.sin_port = htons( 8888 );

  
  xbind(server_fd, (struct sockaddr *)&address, sizeof address, QUI);

  xlisten(server_fd, 1, QUI);

  return server_fd;
}

int main(int argc, char *argv[])
{
  // controlla numero argomenti
  unsigned int file_counter = 0;
  unsigned int args_counter = 0;
  
  args_checker(&file_counter, &args_counter, argc, argv);

  char** files = (char**)calloc(sizeof(char*), file_counter);
  for(int i = (2*args_counter+1); i < argc; i++)
    files[i - (2*args_counter+1)] = argv[i];

  int server_fd = sock_init_listen();

	
	return 0;

}