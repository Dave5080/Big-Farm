#include "xerrori.h"


#define QUI __LINE__,__FILE__

#define PORT 6969

bool qflag = false;
bool tflag = false;
unsigned int nthread = 1;

int main(int argc, char *argv[])
{
  // controlla numero argomenti
  unsigned int file_counter = 0;
  unsigned int args_counter = 0;
  if(argc<2) {
      printf("Uso: %s file [-q -t] [-n threads] <file ...> \n",argv[0]);
      return 1;
  } else{
    for(unsigned int i = 1; i < argc; i++)
      if(argv[i][0] == '-'){
        for(unsigned int j = 1; j < strlen(argv[i]); j++){
          switch(argv[i][j]){
            case 'q':
              args_counter++;
              qflag = true;
            break;

            case 't':
              args_counter++;
              tflag = true;
            break;

            case 'n':
              if(j < strlen(argv[i])-1){
                printf("Errore: When concatenating arguments n must be last");
                exit(1);
              }
              args_counter++;
              nthread = atoi(argv[i+1]);
              i++;

              if(nthread < 1) {
                printf("Uso: %s file [-q -t] [-n threads] <file ...> \n",argv[0]);
              }
          }
        }
      } else {
        file_counter++;
      }

    if(file_counter < 1){
      printf("Errore: When concatenating arguments n must be last");
      exit(1);
    }  
  }
  char** files = (char**)calloc(sizeof(char*), file_counter);
  for(int i = args_counter + (nthread >= 1); i < argc; i++)
    files[i - args_counter - (nthread >= 1)] = argv[i];

  int server_fd = xsocket(AF_LOCAL, SOCK_STREAM, 0, QUI);
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  address.sin_family = AF_LOCAL;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  xbind(server_fd, (struct sockaddr *) &address, addrlen, QUI);

  xlisten(server_fd, (struct sockaddr *) &address, addrlen, QUI);


	
	return 0;
}