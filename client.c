#include "xerrori.h"

#define QUI __LINE__,__FILE__

int client_fd = 0, sock;
sem_t sem_client;


struct entry{
  char* filename;
  long sum;
};

struct entry_list{
  struct entry* value;
  struct entry_list* next;
};

struct entry_list* insert_entry(struct entry_list* list, char* filename, long sum){
  if(list == NULL){
    list = (struct entry_list*) malloc(sizeof(struct entry_list));
    list->next = NULL;
    list->value = (struct entry*)malloc(sizeof(struct entry));
    list->value->filename = filename;
    list->value->sum = sum;
    return list;
  }
  struct entry_list* tmp = list;
  while(tmp->next != NULL)
    tmp = tmp->next;
  
  tmp->next = insert_entry(tmp->next, filename, sum);

  return list;
}

void free_entry_list(struct entry_list* list){
  if(list == NULL) return;
  free_entry_list(list->next);
  free(list->value->filename);
  free(list->value);
  free(list);
}

void print_entry_list(struct entry_list* list){
  if(list == NULL){
    return;
  }
  printf("%s >> %ld\n", list->value->filename, list->value->sum);

  print_entry_list(list->next);
}

int sock_init_connect(){
  struct sockaddr_in address;

  sock = xsocket(AF_INET, SOCK_STREAM, 0, QUI);
  address.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(address.sin_addr));
	address.sin_port = htons( 8888 );

  //printf("[Client] Connecting...\n");
  client_fd = connect(sock, (struct sockaddr*)&address, sizeof(address));
  send(sock, "client\n", sizeof("client\n"), 0);
  //printf("[Client] Connected!\n");


  xsem_init(&sem_client, 0, 1, QUI);

  return client_fd;
}

char* read_raw(){

    int raw_size = 1024;
    char* raw = (char*)calloc(sizeof(char), raw_size);

    char buf[1];
    buf[0] = '\x0';
    raw[0] = buf[0];
    int i = 0;
    while(buf[0] != '\r'){
      read(sock, buf, sizeof(buf));
      
      
      if(buf[0]=='\r') break;
      if(i >= raw_size-2){
        raw_size*=2;
        raw = (char*)realloc(raw,sizeof(raw)*raw_size);
      }

      raw[i] = buf[0];
      raw[i+1] = '\x0';
      i++;
    }

  /*printf("\n\n\n");
  for(int j = 0; j <= i; j++)
    if(raw[j] == '\n')
      printf("[\\n]");
    else if(raw[j] == '\x0')
      printf("[\\x0]");
    else printf("%c",raw[j]);
  printf("\n\n\n");*/
  
  return raw;
}

struct entry_list* read_entries(){
  struct entry_list* list = NULL;
  char* raw = read_raw();
  if(strcmp(raw, "Nessun file\n\x0") == 0)
    return NULL;
  short pendulum = 0;
  int i = 0;
  int old_i = i;
  int tmp_size=255;
  char* tmp = (char*) calloc(sizeof(char), tmp_size);

  tmp[0] = '\x0';
  char* filename;
  long sum;


  while(raw[i] != '\x0'){
    while(raw[i] != '\n'){
      if(i-old_i >= tmp_size-2){
        tmp_size*=2;
        tmp = (char*) realloc(tmp, sizeof(char)*tmp_size);
      }
      tmp[i-old_i] = raw[i];
      i++;
    }
    tmp[i-old_i] = '\x0';
    if(pendulum == 0){
      filename = (char*)calloc(sizeof(char),(i-old_i)+1);
      strcpy(filename, tmp);
    } else{
      sum = atol(tmp);
      list = insert_entry(list, filename, sum);
    }
    i++;
    old_i = i;

    pendulum = (pendulum+1)%2;
  }

  free(raw);
  free(tmp);
  return list;
}

int main(int argc, char *argv[])
{
  bool askall = argc < 2;
  long* sums;
  if(!askall) {
      sums = (long*)calloc(sizeof(long),argc-1);
      for(int i = 0; i < argc-1; i++){
        sums[i] = atol(argv[i+1]);
      }
  }
  
  struct entry_list* list = NULL;
  if(askall){
    sock_init_connect();
    send(sock, "all\n", sizeof("all\n"), 0);
    list = read_entries();
    if(list == NULL)
      printf("Nessun file\n");
    else{
      print_entry_list(list);
      free_entry_list(list);
    }
    close(sock);
  } else {
    char slong[16];
    for(int i = 0; i < argc-1; i++){
      sock_init_connect();
      sprintf(slong, "%ld\n", sums[i]);
      send(sock, slong, sizeof(slong), 0);
      list = read_entries();
      if(list == NULL)
        printf("Nessun file con somma: %ld\n", sums[i]);
      else{
        print_entry_list(list);
        free_entry_list(list);
      } 
      close(sock);
    }
  }
  if(!askall)
    free(sums);
  //free_entry_list(list);
  close(client_fd);
  
  
	return 0;
}