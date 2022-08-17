#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define POS(A,B) if(A=='_') B = 62;\
else if(A == '-') B = 63;\
else if(A <= '9') B = A - '0';\
else if(A <= 'Z') B = A - 'A' + 10;\
else B = A - 'a' + 36;

static int len = 0;
static unsigned long int filtrate_counter = 0, total_counter = 0;
static uint8_t Global_counter[64];
static uint8_t vincoli_aggiornati[64];
static uint64_t Global_sbagliate = 0ULL;
static uint8_t end=2; //end = 2 indica la prima partita

//questa parte del codice gestisce l'acquisizione, il confronto e la classificazione delle stringhe nel dizionario (filtrate/scartate)

char get_input(char temp[]){
    char c;
    char flag = 0;
    int i = 0;
    c = getc(stdin);
    if(c == EOF){
        temp[0] = c;
        return 1;
    }
    while(c != '\n'){
        if(c == '+'){
            c = getc(stdin);
            flag = c; //il flag è la prima lettera del comando dopo il +
        }
        else if(flag == 0){
            temp[i] = c;
            i++;
        }
        c = getc(stdin);
    }
    return flag;
}

void confronta(char p[], char r[], char out[], uint64_t accettabili[]){ //genera l'output del confronto tra parole (r corretta, p da verificare)
    uint8_t temp_counter[64], counter[64], pos;
    uint64_t j = 1ULL;
    uint64_t aggiorna = 0ULL;
    memset(counter,0,64);
    memset(temp_counter,0,64);
    memset(vincoli_aggiornati,0,64);
    unsigned int v = 0, i = 0;
    end = 1;
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
                    Global_sbagliate |= j;
                    if((aggiorna & j) == 0){
                        vincoli_aggiornati[v] = p[i];
                        v++;
                        aggiorna |= j;
                    }
                }
            }
        }
    }
    return;
}

uint8_t scarta_update(char p[], uint64_t accettabili[]){
    uint i=0, k=0;
    uint64_t j = 0;
    uint8_t pos, counter[64];
    memset(counter,0,64);
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
    uint8_t counter[64];
    unsigned int i = 0, k = 0;
    memset(counter,0,64);
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
    uint8_t grey_l:1;
    uint8_t grey_r:1;
}node;

static node* root = NULL;

void inorder(node* visit, uint8_t slow){
    if(visit == NULL)
        return;
    if(slow == 1){
        if(visit->l != NULL){
            inorder(visit->l,slow);
        }
        if(visit->scartata == 0){
            fwrite((char*) visit+sizeof(node),1,len,stdout);
            printf("\n");
        }
        if(visit->r != NULL){
            inorder(visit->r,slow);
        }
        return;
    }
    else{
        if(visit->l != NULL && visit->grey_l == 0){
            inorder(visit->l,slow);
        }
        if(visit->scartata == 0){
            fwrite((char*) visit+sizeof(node),1,len,stdout);
            printf("\n");
        }
        if(visit->r != NULL && visit->grey_r == 0){
            inorder(visit->r,slow);
        }
        return;
    }
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

void create_and_insert(char* str, uint8_t scartata){ //si potrebbe accorpare con insert
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
    a->grey_l = 0;
    a->grey_r = 0;
    if(root == NULL){
        a->color = BLACK;
        root = a; //se sto creando il primo nodo assegno a alla radice
    }
    else{
        a->color = RED;
        RB_Insert(a);
    }
    return;
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

//questa parte del codice implementa la gestione del dizionario e della struttura secondaria (linked list)


uint8_t aggiorna_vincoli_recursive(node* x, uint64_t accettabili[], uint8_t set){ //return 1 se da li a sotto l'albero è tutto composto di scartate (grey)
    if(x == NULL){
        return 1;
    }
    if(x->scartata == 0 && scarta_update((char*) x+sizeof(node),accettabili) == 1){
        x->scartata = 1;
        filtrate_counter--;
    }
    if(set == 0){
        if(x->l != NULL && x->grey_l == 0){
            x->grey_l = aggiorna_vincoli_recursive(x->l, accettabili, set);
        }
        else if(x->l == NULL){
            x->grey_l = 1;
        }
        if(x->r != NULL && x->grey_r == 0){
            x->grey_r = aggiorna_vincoli_recursive(x->r, accettabili, set);
        }
        else if(x->r == NULL){
            x->grey_r = 1;
        }
    }
    else{
        if(x->l != NULL){
            x->grey_l = aggiorna_vincoli_recursive(x->l, accettabili, set);
        }
        else{
            x->grey_l = 1;
        }
        if(x->r != NULL){
            x->grey_r = aggiorna_vincoli_recursive(x->r, accettabili, set);
        }
        else{
            x->grey_r = 1;
        }
    }
    return x->scartata & x->grey_l & x->grey_r;
}

void ripristina_vincoli(node* x){
    if(x == NULL){
        return;
    }
    x->scartata = 0;
    x->grey_l = 0;
    x->grey_r = 0;
    if(x->l != NULL){
        ripristina_vincoli(x->l);
    }
    if(x->r != NULL){
        ripristina_vincoli(x->r);
    }
    return;
}

//Il main funge da parser e gestisce le chiamate alle funzioni necessarie per gestire le strutture dati

int main(){
    int tries, s = 0;
    uint8_t set = 0;
    uint8_t sc = 0;
    char command;
    s = scanf("%d",&len);
    if(s == 0){
        return 1;
    }
    //stringa letta, stringa di riferimento della partita ed output da stampare
    char temp[len];
    char reference[len];
    char output[len];
    uint64_t accettabili[len]; //vettore di bitmask per segnare le lettere accettate/vietate/obbligatorie in ogni pos
    temp[0] = getc(stdin); //leggo \n che scanf ignora
    command = get_input(temp);
    while(command == 0){ //inserimento iniziale di parole
        create_and_insert(temp,0);
        total_counter++;
        command = get_input(temp);
    }
    while(temp[0] != EOF){
        if(command == 'n'){ //+nuova_partita
            Global_sbagliate = 0ULL; //ripristino i counter globali
            memset(Global_counter,0,64);
            if(end != 2){ //la prima partita non ho bisogno di ripristinare i vincoli
                ripristina_vincoli(root);
            }
            filtrate_counter = total_counter;
            memset(accettabili,UINT8_MAX,len*8); //tutte le lettere sono accettabili in ogni posizione
            end = 0; //flaggo l'inizio della partita
            set = 1;
            command = get_input(reference); //assegno la reference
            s = scanf("%d",&tries); //assegno i tentativi
            temp[0] = getc(stdin); //leggo anche \n
            command = get_input(temp);
        }
        else if(command == 's'){ //stampa filtrate
            inorder(root,set);
            command = get_input(temp);
        }
        else if(command  == 'i'){ //inserisci inizio
            command = get_input(temp);
            set = 1;
            while(command  == 0){
                total_counter++;
                if(end == 0){
                    sc = scarta(temp,reference,accettabili);
                    create_and_insert(temp,sc);
                    if(sc == 0){
                        filtrate_counter++;
                    }
                }
                else{
                    create_and_insert(temp, 0);
                }
                command = get_input(temp);
            }
            command = get_input(temp);
        }
        else{
            while(end == 0 && tries > 0 && command == 0){
                if(binary_search(temp) == 1){
                    confronta(temp,reference,output,accettabili);
                    if(end == 1){
                        printf("ok\n");
                    }
                    else{
                        if(filtrate_counter != 1 || set == 1){
                            aggiorna_vincoli_recursive(root,accettabili,set);
                            set = 0;
                        }
                        fwrite(output,1,len,stdout);
                        printf("\n%lu\n",filtrate_counter);
                    }
                    tries--;
                }
                else{
                    printf("not_exists\n");
                }
                command = get_input(temp);
            }
            if(tries == 0 && end == 0){
                end = 1;
                printf("ko\n");
            }
        }
    }
    return 0;
}