#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define POS(A,B) if(A=='_') B = 62;\
else if(A == '-') B = 63;\
else if(A <= '9') B = A - '0';\
else if(A <= 'Z') B = A - 'A' + 10;\
else B = A - 'a' +36;

#define LIST_SIZE 100

static int len = 0;
static int filtrate_counter = 0;
static unsigned int Global_counter[64];
static uint8_t vincoli_aggiornati[64];
static uint64_t Global_sbagliate = 0ULL;
static uint8_t end=2; //end = 2 indica la prima partita

char get_input(FILE* fp, char temp[]){
    char c;
    char flag = 0;
    int i = 0;
    c = fgetc(fp);
    if(c == EOF){
        temp[0] = c;
        return 1;
    }
    while(c != '\n'){
        if(c == '+'){
            c = fgetc(fp);
            flag = c; //il flag è la prima lettera del comando dopo il +
        }
        else if(flag == 0){
            temp[i] = c;
            i++;
        }
        c = fgetc(fp);
    }
    return flag;
}

void confronta(char p[], char r[], char out[], uint64_t accettabili[]){ //genera l'output del confronto tra parole (r corretta, p da verificare)
    unsigned int temp_counter[64], i=0, counter[64];
    uint64_t j = 1ULL;
    uint64_t sbagliate = 0ULL;
    uint64_t aggiorna = 0ULL;
    memset(counter,0,256);
    memset(temp_counter,0,256);
    memset(vincoli_aggiornati,0,64);
    uint v = 0;
    end = 1;
    uint8_t pos;
    for(i=0;i<len;i++){
        POS(p[i],pos);
        j = 1ULL << pos;
        if(p[i] == r[i]){
            out[i] = '+'; //se è corretta lo segno subito
            counter[pos]++; //conto le lettere di p in posizione corretta
            if(counter[pos] > Global_counter[pos]){ //ho un nuovo massimo di apparizioni legali di una lettera
                Global_counter[pos] = counter[pos];
                if((aggiorna & j) == 0){
                    vincoli_aggiornati[v] = p[i];
                    v++;
                    aggiorna |= j;
                }
            }
            accettabili[i] = j; //imposto vincolo globale per accettare solo una lettera nella i-esima posizione
        }
        else{
            end = 0;
            out[i] = 0;
            POS(r[i],pos);
            temp_counter[pos]++; //segno le lettere di r con un mismatch in p, poi le uso per le |
            accettabili[i] &= ~j; //apprendo che la lettera non è ammessa in quella posizione NB j è ancora flaggato da p[i]
        }
    }
    for(i=0;i<len;i++){ //scorro p e assegno le caselle / e |
        if(out[i] == 0){
            POS(p[i],pos);
            j = 1ULL << pos;
            if(temp_counter[pos] > 0){ //assegno le giuste in pos sbagliata
                out[i] = '|';
                counter[pos]++; //conto quelle giuste in pos sbagliata
                temp_counter[pos]--;
                if(counter[pos] > Global_counter[pos]){ //ho un nuovo massimo di apparizioni legali di una lettera
                    Global_counter[pos] = counter[pos];
                    if((aggiorna & j) == 0){
                        vincoli_aggiornati[v] = p[i];
                        v++;
                        aggiorna |= j;
                    }
                }
            }
            else{
                out[i] = '/';
                //non conto le lettere sbagliate
                if((Global_sbagliate & j) == 0){ //se non era già una sbagliata
                    sbagliate |= j;
                    if((aggiorna & j) == 0){
                        vincoli_aggiornati[v] = p[i];
                        v++;
                        aggiorna |= j;
                    }
                }
            }
        }
    }
    //aggiorno globale
    Global_sbagliate |= sbagliate;
    return;
}

uint8_t scarta_update(char p[], uint64_t accettabili[]){
    uint i=0, k=0;
    uint16_t pos;
    uint64_t j = 0;
    unsigned int counter[64];
    memset(counter,0,256);
    for(i=0;i<len;i++){
        POS(p[i],pos);
        j = 1ULL << pos;
        if((j & accettabili[i]) == 0){
            return 1;
        }
        counter[pos]++;
    }
    while(vincoli_aggiornati[k] != 0){
        POS(vincoli_aggiornati[k],pos);
        j = 1ULL << pos;
        if((j & Global_sbagliate) > 0){ //il flag sbagliate mi garantisce di conoscere il numero esatto
            if(counter[pos] != Global_counter[pos]){
                return 1;
            }
        }
        else if(counter[pos] < Global_counter[pos]){ //la lettera non appare abbastanza volte
            return 1;
        }
        k++;
    }

    return 0;
}

uint8_t scarta(char p[], char r[], uint64_t accettabili[]){
    uint64_t j = 0;
    uint64_t checklist = 0ULL;
    char to_check[64];
    unsigned int counter[64];
    unsigned int i = 0, k = 0;
    memset(counter,0,256);
    memset(to_check,0,64);
    uint8_t pos;
    for(i=0; i<len;i++){
        POS(r[i],pos);
        j = 1ULL << pos;
        if((checklist & j) == 0){
            to_check[k] = r[i];
            k++;
        }
        POS(p[i],pos);
        j = 1ULL << pos;
        if((checklist & j) == 0){
            to_check[k] = p[i];
            k++;
        }
        if((j & accettabili[i]) == 0){ //mi assicuro che la lettera possa andare nella i-esima posizione
            return 1;
        }
        counter[pos]++;
    }
    k=0;
    while(to_check[k] != 0){
        POS(to_check[k],i);
        j = 1ULL << i;
        if((j & Global_sbagliate) > 0){ //il flag sbagliate mi garantisce di conoscere il numero esatto
            if(counter[i] != Global_counter[i]){
                return 1;
            }
        }
        else if(counter[i] < Global_counter[i]){ //la lettera non appare abbastanza volte
            return 1;
        }
        k++;
    }
    return 0;
}

int8_t string_compare(char a[], char b[]){ //date due str in input dice quale viene prima in ordine lessicografico
    int i = 0;
    while(i<len && a[i] == b[i]){
        i++;
    }
    if(i>=len){ //sono uguali
        return 0;
    }
    else if(a[i]<b[i]){ // a viene prima di b
        return -1;
    }
    else{ //b viene prima di a
        return 1;
    }
}

//questa parte del codice implementa gli Alberi Rosso Neri necessari per la creazione del dizionario
//è seguita l'implementazione del Cormen

#define RED 0
#define BLACK 1

typedef struct nodo{
    struct nodo* p;
    struct nodo* l;
    struct nodo* r;
    uint8_t color:1;
    uint8_t scartata:1;
}node;

static node* root = NULL;

void inorder(node* visit){
    if(visit == NULL)
        return;
    if(visit->l != NULL){
        inorder(visit->l);
    }
    if(visit->scartata == 0){
        fwrite((char*) visit+sizeof(node),1,len,stdout);
        printf("\n");
    }
    if(visit->r != NULL){
        inorder(visit->r);
    }
    return;
}

void left_rotate(node* x){
    node* y = x->r; //imposto y come figlio destro di x
    if(y == NULL){
        //printf("non e` stato possibile eseguire la funzione");
        return;
    }
    x->r = y->l; //il sotto-albero sx di y diventa destro di x
    if(y->l != NULL){
        y->l->p = x; //se y ha un figlio sx, x diventa i padre del sx di y
    }
    y->p = x->p; //assegno al padre di y il padre di x
    if(x->p == NULL){ //se x e` root
        root = y;
    }
    else if(x == x->p->l){ //se x e` figlio sinistro di suo padre
        x->p->l = y;
    }
    else{ //se x e` figlio destro di suo padre
        x->p->r = y;
    }
    y->l = x;
    x->p = y;
}

void right_rotate(node* x){
    node* y = x->l; //iposto y come figlio sinistro di x
    if(y == NULL){
        //printf("non e` stato possibile eseguire la funzione");
        return;
    }
    x->l = y->r; //il sottoalbero dx di y diventa sx di x
    if(y->r != NULL){
        y->r->p = x; //se y ha un figlio dx, x diventa i padre del dx di y
    }
    y->p = x->p; //assegno al padre di y il padre di x
    if(x->p == NULL){ //se x e` root
        root = y;
    }
    else if(x == x->p->r){ //se x e` figlio dx di suo padre
        x->p->r = y;
    }
    else{ //se x e` figlio sx di suo padre
        x->p->l = y;
    }
    y->r = x;
    x->p = y;
}

void RB_Insert_Fixup(node* x){
    node* y;
    while(x->p!=NULL && x->p->color==RED){ //per funzionare devo avere x.p e x.p.p validi, basta x.p rosso
        if(x->p == x->p->p->l) { //x.p non puo essere radice, esiste x.p.p se esiste valido x.p
            y = x->p->p->r;
            if (y != NULL && y->color == RED) { //se y e NULL vuol dire che e una figlia (nera)
                x->p->color = BLACK;
                y->color = BLACK;
                x->p->p->color = RED;
                x = x->p->p;
            }
            else {
                if (x == x->p->r) {
                    x = x->p;
                    left_rotate(x);
                }
                x->p->color = BLACK;
                x->p->p->color = RED;
                right_rotate(x->p->p);
            }
        }
        else{ //x.p e figlio destro di x.p.p
            y = x->p->p->l;
            if(y != NULL && y->color == RED){ //se y e NULL vuol dire che e una figlia (nera)
                x->p->color = BLACK;
                y->color = BLACK;
                x->p->p->color = RED;
                x = x->p->p;
            }
            else {
                if (x == x->p->l) {
                    x = x->p;
                    right_rotate(x);
                }
                x->p->color = BLACK;
                x->p->p->color = RED;
                left_rotate(x->p->p);
            }
        }
    }
    root->color = BLACK;
    return;
}

void RB_Insert(node* z){
    node* y = NULL;
    node* x;
    x = root;
    while(x != NULL) {
        y = x;
        if (string_compare((char*) z+sizeof(node),(char*) x+sizeof(node))==-1) {
            x = x->l;
        }
        else {
            x = x->r;
        }
    }
    z->p = y;
    if(y == NULL){
        root = z;
    }
    else if(string_compare((char*) z+sizeof(node),(char*) y+sizeof(node))==-1){
        y->l = z;
    }
    else{
        y->r = z;
    }
    RB_Insert_Fixup(z);
}

node* create_and_insert(char* str, uint8_t scartata){ //si potrebbe accorpare con insert
    node *a;
    a = (node*)malloc(sizeof(node)+len);
    char* b = (char*) a + sizeof(node);
    for(int i=0; i<len; i++){
        b[i] = str[i];
    }
    a->p = NULL;
    a->l = NULL;
    a->r = NULL;
    a->scartata = scartata;
    if(root == NULL){
        a->color = BLACK;
        root = a; //se sto creando il primo nodo assegno a alla radice
    }
    else{
        a->color = RED;
        RB_Insert(a);
    }
    return a;
}

uint8_t binary_search(char* target){//ricerca binaria in BST, restituisce 1 se trovato, 0 altrimenti
    node* temp = root;
    int c = 1 ;
    while(temp != NULL && c != 0){
        c = string_compare(target,(char*) temp+sizeof(node));
        if(c == -1){
            temp = temp->l;
        }
        else if(c == 1) {
            temp = temp->r;
        }
    }
    if(c == 0){
        return 1;
    }
    else{
        return 0;
    }
}

typedef struct list{
    node* ptr[LIST_SIZE];
    uint8_t valid;
    struct list* next;
}list_type;

struct list* head = NULL;
struct list* curr = NULL;
static uint16_t list_index = 0;

void aggiorna_vincoli_recursive(node* x, uint64_t accettabili[]){
    int i =0;
    if(x == NULL){
        return;
    }
    if(x->scartata == 0 && scarta_update((char*) x+sizeof(node),accettabili) == 1){
        x->scartata = 1;
    }
    if(x->scartata == 0){
        filtrate_counter++;
        if(head == NULL){
            head = curr = malloc(sizeof(struct list));
            for(i=0;i<LIST_SIZE;i++){
                curr->ptr[i] = NULL;
            }
            curr->valid = 0;
            list_index = 0;
            curr->next = NULL;
        }
        else if(list_index == LIST_SIZE){
            curr->next = malloc(sizeof(struct list));
            curr = curr->next;
            for(i=0;i<LIST_SIZE;i++){
                curr->ptr[i] = NULL;
            }
            curr->valid = 0;
            list_index = 0;
            curr->next = NULL;
        }
        curr->valid++;
        curr->ptr[list_index] = x;
        list_index++;
    }
    if(x->l != NULL){
        aggiorna_vincoli_recursive(x->l, accettabili);
    }
    if(x->r != NULL){
        aggiorna_vincoli_recursive(x->r, accettabili);
    }
    return;
}

void aggiorna_vincoli_list(uint64_t accettabili[]){
    struct list* a = head;
    struct list* b;
    struct list* p = head;
    u_int16_t i;
    while(a != NULL){
        b = a->next;
        for(i=0;i<LIST_SIZE && a->valid >0;i++){
            if(a->ptr[i] != NULL){
                if(scarta_update((char*) a->ptr[i]+sizeof(node),accettabili) == 1){
                    a->ptr[i]->scartata = 1;
                    a->ptr[i] = NULL;
                    a->valid--;
                }
                else{
                    filtrate_counter++;
                }
                if(a->valid == 0){
                    i = LIST_SIZE;
                }
            }
        }
        if(a->valid == 0){
            if(a == head){
                head = b;
            }
            else{
                p->next = b;
            }
            free(a);
        }
        else{
            p = a;
        }
        a = b;
    }
    curr = p;
}

void empty_list(){
    struct list* a = head;
    struct list* b;
    while(a!=NULL){
        b = a->next;
        free(a);
        a = b;
    }
    head = curr = NULL;
}

void ripristina_vincoli(node* x){
    if(x == NULL){
        return;
    }
    x->scartata = 0;
    if(x->l != NULL){
        ripristina_vincoli(x->l);
    }
    if(x->r != NULL){
        ripristina_vincoli(x->r);
    }
    return;
}

int main(){
    //questa parte del main funge da parser
    //FILE* fp= fopen("/home/giacomo/Scaricati/n128000_k5_g200_test1.txt","r"); //test in locale
    FILE* fp = stdin;
    if(fp == NULL){
        printf("NULL FILE");
        return 1;
    }
    node* insert = NULL;
    int tries,s=0, i=0;
    uint8_t sc = 0, primo_confronto = 0;
    char command;
    s = fscanf(fp,"%d",&len);
    if(s == 0){
        return 1;
    }
    //stringa letta, stringa di riferimento della partita ed output da stampare
    char temp[len];
    char reference[len];
    char output[len];
    uint64_t accettabili[len]; //vettore di bitmask per segnare le lettere accettate/vietate/obbligatorie in ogni pos
    temp[0] = fgetc(fp); //leggo \n che scanf ignora
    command = get_input(fp,temp);
    while(command == 0){ //inserimento iniziale di parole
        insert = create_and_insert(temp,0);
        command = get_input(fp,temp);
    }
    while(temp[0] != EOF){
        if(command == 'n'){ //+nuova_partita
            //printf("nuova\n");
            //ripristino il counter globale
            Global_sbagliate = 0ULL;
            memset(Global_counter,0,256);
            if(end != 2){ //la prima partita non ho bisogno di ripristianre i vincoli
                ripristina_vincoli(root);
            }
            //tutte le lettere sono accettabili in ogni posizione
            memset(accettabili,UINT8_MAX,len*8);
            filtrate_counter = 0;
            //svuoto la struttura ausiliaria
            empty_list();
            end = 0; //flaggo l'inizio della partita
            primo_confronto = 1;
            command = get_input(fp,reference);
            s = fscanf(fp,"%d",&tries);
            if(s == 0){
                return 1;
            }
            temp[0] = fgetc(fp); //leggo anche \n
            command = get_input(fp,temp);
        }
        else if(command == 's'){ //stampa filtrate
            //printf("stampa\n");
            inorder(root);
            command = get_input(fp,temp);
        }
        else if(command  == 'i'){ //inserisci inizio
            //printf("inserimento\n");
            command = get_input(fp,temp);
            while(command  == 0){
                if(end == 0){
                    sc = scarta(temp,reference,accettabili);
                    insert = create_and_insert(temp,sc);
                    if(sc == 0){
                        if(head == NULL){
                            head = curr = malloc(sizeof(struct list));
                            for(i=0;i<LIST_SIZE;i++){
                                curr->ptr[i] = NULL;
                            }
                            list_index = 0;
                            curr->next = NULL;
                            curr->valid = 0;
                        }
                        else if(list_index == LIST_SIZE){
                            curr->next = malloc(sizeof(struct list));
                            curr = curr->next;
                            for(i=0;i<LIST_SIZE;i++){
                                curr->ptr[i] = NULL;
                            }
                            list_index = 0;
                            curr->next = NULL;
                            curr->valid = 0;
                        }
                        curr->ptr[list_index] = insert;
                        list_index++;
                        curr->valid++;
                    }
                }
                else{
                    create_and_insert(temp, 0);
                }
                command = get_input(fp,temp);
            }
            command = get_input(fp,temp);
        }
        else{
            while(end == 0 && tries > 0 && command == 0){
                if(binary_search(temp) == 1){
                    confronta(temp,reference,output,accettabili);
                    if(end == 1){
                        printf("ok\n");
                    }
                    else{
                        filtrate_counter = 0;
                        if(primo_confronto == 1){
                            aggiorna_vincoli_recursive(root,accettabili);
                        }
                        else{
                            aggiorna_vincoli_list(accettabili);
                        }
                        primo_confronto = 0;
                        fwrite(output,1,len,stdout);
                        printf("\n%d\n",filtrate_counter);
                    }
                    tries--;
                }
                else{
                    printf("not_exists\n");
                }
                command = get_input(fp,temp);
            }
            if(tries == 0 && end == 0){
                end = 1;
                printf("ko\n");
            }
        }
    }
    return 0;
}