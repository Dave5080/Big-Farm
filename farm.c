#include "xerrori.h"
#include "simple_queue.h"



#define PORT 3490

unsigned int qlen = 8;
unsigned int delay = 0;
unsigned int nthread = 1;

pthread_t* workers; 

struct Queue* files;

int client_fd = 0, sock;
sem_t sem_client;



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

int sock_init_connect(){
  struct sockaddr_in address;

  sock = xsocket(AF_INET, SOCK_STREAM, 0, QUI);

  address.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(address.sin_addr));
	address.sin_port = htons( 8888 );

  printf("[Farm] Connecting...\n");
  client_fd = connect(sock, (struct sockaddr*)&address, sizeof(address))
    ;
  printf("[Farm] Connected\n");
  //xbind(server_fd, (struct sockaddr *)&address, sizeof address, QUI);

  //xlisten(server_fd, 1, QUI);

  xsem_init(&sem_client, 0, 1, QUI);

  return client_fd;
}

int sock_send_couple(char* filename, long sum, int pid, int tid){

  xsem_wait(&sem_client, QUI);

  char ssum[15] = "", spid[15] = "", stid[15] = "";


  sprintf(ssum, "%ld\n", sum);
  sprintf(spid, "%d\n", pid);
  sprintf(stid, "%d\n", tid);

  printf("Sending sum: %ld -> %s", sum, ssum);
  printf("Sending pid: %d -> %s", pid, spid);
  printf("Sending tid: %d -> %s", tid, stid);

  send(sock, filename, strlen(filename), 0);
  send(sock, "\n", strlen("\n"), 0);
  printf("Sent: %s \t\t\t", filename);

  send(sock, ssum, sizeof(ssum), 0);
  printf("%ld \t\t\t", sum);


  send(sock, spid, sizeof(spid), 0);
  printf(" %d/", pid);


  send(sock, stid, sizeof(stid), 0);
  printf("%d\n", tid);

  xsem_post(&sem_client, QUI);
  return 0;
}

void * runworker(void* tid){
  FILE* file;
  char* filename;

  long val;
  long sum = 0;
  int n;
  int j;
    
  long buffer[64];
  do{
    filename = (char*) dequeue(files);
    
    if(!strcmp(filename, "")) break;
    file = xfopen(filename, "r", QUI);
    sum = 0;
    j=0;
    while(!feof(file)) {
      n = fread(buffer, sizeof(long), 64, file);
      for(int i = 0; i < n; i++){
        sum+= j*buffer[i];
        j++;
      }
    }
    sock_send_couple(filename, sum, getpid(), *(int*)tid);
    fclose(file);
  }while(strcmp(filename, ""));
  pthread_exit(NULL);
}

int main(int argc, char *argv[]){

  // Argument checking
  unsigned int file_counter = 0;
  unsigned int args_counter = 0;
  args_checker(&file_counter, &args_counter, argc, argv);


  sock_init_connect();

  // Thread initialization
  workers = (pthread_t*)calloc(sizeof(pthread_t), nthread);
  files = newQueue(qlen);
  for(int t = 0; t < nthread; t++)
    xpthread_create(&(workers[t]), NULL, runworker, &(workers[t]), QUI);
	
  // Producers distributes name files
  for(int i = (2*args_counter+1); i < argc; i++){
    enqueue(files, argv[i]);
    sleep(delay/1000);
  }

  // Close threads
  for(int t = 0; t <= nthread; t++) enqueue(files, "");
  for(int t = 0; t < nthread; t++)
    pthread_join(workers[t], NULL);
  // Free memory
  freeQueue(files);
  free(workers);

	return 0;

}