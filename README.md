# Big-Farm
Esame Laboratorio 2B Salvatore D'Amico Mat. 616885

Il progetto prevede lo sviluppo di 3 processi non concorrenti, comunicanti tramite connessione socket TCP, il cui scopo è rispettivamente: analizzare
memorizzare e visualizzzare i dati contenuti in numerosi file di ingresso opportunamente formattati.

Farmer:
  Il processo Farmer ha il compito di comprimere una serie di file i cui percorsi vengono specificati come argomenti in riga di comando
  secondo la seguente formula: Per i da 0 a n-1 SUM(i*file[i]) dove i rappresenta la posizione ordinale del numero nel file e file[i] il numero stesso.
  L'implementazione acquisisce la forma di un problema produttore/consumatori dove un thread produttore carica in un buffer sequenzialmente i percorsi
  ai file mentre i thread consumatori vi accedono concorrendo per poi analizzare ed elaborare in maniera indipendente il contenuto dei file.
  Per implementare il buffer è stata realizzata una struttura FIFO coda a semafori per garantire la sicurezza del flusso di esecuzione in ambiente 
  concorrente. Implementando la coda si astrae la gestione del buffer rendendo facile e immediato il suo accesso sia dal produttore che dal consumatore,
  garantendo quindi una facile manutenzione del codice.
  Per la comunicazione con il server collector si realizza una connessione socket TCP usando un protocollo TCP realizzato appositamente per questo
  progetto, chiamato BigFarm_protocol sottomodello "worker". Dato che dalla sua implementazione è previsto che ogni farmer esegua una singola connessione sulla quale inviare 
  i risultati delle elaborazioni di tutti i file, si realizza un ulteriore semaforo per garantire l'accesso concorrente al socket rispettando il protocollo.
  Infine per l'analisi degli argomenti in ingresso si sceglie di utilizzare una funzione realizzata appositamente piuttosto che la libreria di sistema 
  getotp(3) per avere un maggiore controllo su questi ultimi e sulla loro formattazione e eventuali risposte nei casi quest'ultima sia errata.

Collector:
  Il processo Collector ha il compito di ricevere i dati da tutte le connessioni socket con protocollo TCP BigFarm_protocol, considerando che il protocollo 
  prevede l'in e out di dati conservati in un unica struttura si implementa un oggetto coll con allegato semaforo per garantire la concorrenza tra i vari 
  thread a carico della gestione di ogni connessione instanziata. L'oggetto coll è un hasmap dictionary, struttura nativa di python, ideale considerata la 
  natura dei dati (a ogni percorso file corrisponde il suo output).


Client:
  Il processo Client ha il compito di interrogare il server collector, tramite protocollo TCP BigFarm_protocol sottomodello "client", in modo da
  comunicare all'utente l'esito dell'elaborazione dei file. Questa interrogazione può essere generica o filtrata per risultato, nel caso si voglia
  individuare i file che hanno generato un determinato output, per l'elaborazione del raw restituito dal BigFarm_protocol si inseriscono i dati in ingresso
  progressivamente in una lista collegata, in maniera che una volta terminata la connessione è possibile mostrarli a schermo, o eventualmente in seguito
  a future implementazioni, gestirli in maniera diversa.

BigFarm_protocol:
  Questo è un protocollo di tipo TCP/IP client/server realizzato appositamente per questo progetto. Le parti della connessione inviano una stringa 
  opportunamente formattata per facilitare l'estrazione dei dati da essa. Si notano subito la scelta di una serie di caratteri speciali che i sistemi 
  operativi comuni impediscono di usare nel filesystem come escape keys utili per analizzare la formattazione del testo inviato (da ora in avanti 
  chiamato raw). Il carattere '\n' ha lo scopo di formattare internamente i dati, separando 2 informazioni che altrimenti sarebbero colluse, 
  il carattere '\r' invece ha lo scopo di segnalare il termine dello streming dati e la conseguente possibilità a proseguire nello stage della 
  comunicazione. La comunicazione comincia con una connessione socket effettuata da un client o un farmer verso il server collector. Come primo stage di
  comunicazione il client deve specificare la sua natura: "worker" se è un Farmer, "client" se è un Client. una volta specificata la natura il server affida
  la comunicazione ad un thread apposito nel primo o nel secondo caso.
  
  Sottomodello "farmer":
    Una volta che il server è pronto a ricevere le informazioni dal Farmer ogni thread di quest'ultimo appena finito di analizzare un file invierà 
    (in modo thread safe) una stringa di questo formato "filename[\n]sum[\n]pid[\n]tid[\n]". Dove filename è il percorso del file che ha generato questo
    output, sum è l'output in questione, pid è il Process ID del Farmer e tid è il Thread ID che ha analizzato questo file. Le ultime 2 informazioni sono
    superflue ma vengono implementate ugualmente per consentire una futura realizzazione di apparato di data analytics. a questo punto il server itera su
    tutte le stringhe del formato sopracitato che arrivano dal Farmer, inserendo in maniera concorrente filename e sum all'interno di coll, finchè non 
    riceve l'Escape Key "\r". A questo punto il server chiude la connessione e termina il thread e il Farmer chiude la connessione e termina il processo.
    
   Sottomodello "client":
    Il client invia immediatamente la natura della sua richiesta, "all[\n]" per una richiesta di tipo get_all_data (dove all è costante) oppure "sum[\n]"
    per ottenere soltanto i file che hanno generato un determinato output sum. Nel primo caso il server invia tutti i record letti da coll in maniera 
    thread safe nel formato "filename[\n]sum[\n]" in maniera molto simile a come avveniva nel sottomodello "farmer", terminando con l'Escape Key '\r'. 
    Il Client riceve i dati, vi itera sopra e li analizza fino all'arrivo dell'Escape Key. Il server termina la connessione e il thread associato, il client
    termina il processo.
    Se invece il client è stato avviato con una serie di argomenti sums, per ognuno di essi viene instanziata una connessione del secondo tipo. Il
    server si comporta allo stesso modo, filtrando prima però gli elementi letti da coll affinchè i valori coincidano col dato sum inviato dal client.
    Il client invece eseguirà numerose connessioni indipentendi, una per ogni sum appartente a sums.
