#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

#define POS(A,B) if(A <= 'Z' && A >= 'A') B = A - 'A' + 11;\
else if(A <= 'z' && A >= 'a') B = A - 'a' + 38;\
else if(A >= '0' && A <= '9') B = A - '0' + 1;\
else if(A == '_') B = 37;\
else B = 0;

#define ASC(A,B) if(A >= 38) B = A - 38 + 'a';\
else if(A >= 11 && A <= 36) B = A - 11 + 'A'; \
else if(A>= 1 && A <= 10) B = A - 1 + '0';    \
else if(A == 37) B = '_';                     \
else B = '-';

static int len = 0;
static unsigned int filtrate_counter = 0, total_counter = 0;
static uint8_t Global_counter[64];
static int8_t* vincoli_aggiornati;
static uint64_t Global_sbagliate = 0ULL;
static int end=2; //end = 2 indica la prima partita //end = 1 fine di una partita
static uint8_t updated_count = 0;

//questa parte del codice gestisce l'acquisizione, il confronto e la classificazione delle stringhe nel dizionario (filtrate/scartate)

int8_t get_input(int8_t temp[]){
    int8_t c;
    int8_t flag = 0;
    int i = 0, k;
    c = getchar();
    if(c == EOF){
        return -1;
    }
    while(c != '\n'){
        if(c == '+'){
            c = getchar();
            flag = c; //il flag è la prima lettera del comando dopo il +
        }
        else if(flag == 0){
            POS(c,k)
            temp[i] = k;
            i++;
        }
        c = getchar();
    }
    return flag;
}

void confronta(int8_t p[], int8_t r[], char out[], uint64_t accettabili[]){ //genera l'output del confronto tra parole (r corretta, p da verificare)
    uint8_t temp_counter[64], counter[64];
    uint64_t j = 1ULL;
    uint64_t aggiorna = 0ULL;
    memset(counter,0,64);
    memset(temp_counter,0,64);
    int v = 0, i = 0, k=0;
    end = 1;
    for(i=0;i<len;i++){
        j = 1ULL << p[i];
        if(p[i] == r[i]){
            out[i] = '+'; //se è corretta lo segno subito
            counter[p[i]]++; //conto le lettere di p in posizione corretta
            if(counter[p[i]] > Global_counter[p[i]]){ //ho un nuovo massimo di apparizioni legali di una lettera
                Global_counter[p[i]] = counter[p[i]];
                if((aggiorna & j) == 0){
                    updated_count = 1;
                    vincoli_aggiornati[v] = p[i];
                    v++;
                    aggiorna |= j;
                }
            }
            if(accettabili[i] != j){
                accettabili[i] = j; //imposto vincolo globale per accettare solo una lettera nella i-esima posizione
            }
        }
        else{
            end = 0;
            out[i] = 0;
            temp_counter[r[i]]++; //segno le lettere di r con un mismatch in p, poi le uso per le |
            if((accettabili[i] & j) > 0){
                accettabili[i] &= ~j; //apprendo che la lettera non è ammessa in quella posizione NB j è ancora flaggato da p[i]
            }
        }
    }
    for(i=0;i<len;i++){ //scorro p e assegno le caselle / e |
        if(out[i] == 0){
            j = 1ULL << p[i];
            if(temp_counter[p[i]] > 0){ //assegno le giuste in pos sbagliata
                out[i] = '|';
                counter[p[i]]++; //conto quelle giuste in pos sbagliata
                temp_counter[p[i]]--;
                if(counter[p[i]] > Global_counter[p[i]]){ //ho un nuovo massimo di apparizioni legali di una lettera
                    Global_counter[p[i]] = counter[p[i]];
                    if((aggiorna & j) == 0){
                        vincoli_aggiornati[v] = p[i];
                        updated_count = 1;
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
                    if(counter[p[i]] == 0){
                        for(k=0;k<len;k++){
                            accettabili[k] &= ~j;
                        }
                    }
                    if((aggiorna & j) == 0){
                        vincoli_aggiornati[v] = p[i];
                        updated_count = 1;
                        v++;
                        aggiorna |= j;
                    }
                }
            }
        }
    }
    vincoli_aggiornati[v] = -1;
}

int scarta_update(int8_t p[], uint64_t accettabili[]){
    int i=0, k=0;
    uint64_t j = 0;
    uint8_t counter[64];
    memset(counter,0,64);
    while(i<len){
        j = 1ULL << p[i];
        if((j & accettabili[i]) == 0){
            return 1;
        }
        counter[p[i]]++;
        i++;
    }
    while(vincoli_aggiornati[k] != -1){
        j = 1ULL << vincoli_aggiornati[k];
        if((j & Global_sbagliate) > 0){ //il flag sbagliate mi garantisce di conoscere il numero esatto
            if(counter[vincoli_aggiornati[k]] != Global_counter[vincoli_aggiornati[k]]){
                return 1;
            }
        }
        else if(counter[vincoli_aggiornati[k]] < Global_counter[vincoli_aggiornati[k]]){ //la lettera non appare abbastanza volte
            return 1;
        }
        k++;
    }
    return 0;
}

int scarta_updated_mask(int8_t p[], uint64_t accettabili[]){
    int i=0;
    uint64_t j = 0;
    while(i<len){
        j = 1ULL << p[i];
        if((j & accettabili[i]) == 0){
            return 1;
        }
        i++;
    }
    return 0;
}

int scarta(int8_t p[], int8_t r[], uint64_t accettabili[]){
    uint64_t j = 0;
    uint64_t checklist = 0ULL;
    int8_t to_check[2*len+1];
    uint8_t counter[64];
    int i = 0, k = 0;
    memset(counter,0,64);
    while(i<len){
        j = 1ULL << r[i];
        if((checklist & j) == 0){
            to_check[k] = r[i];
            k++;
        }
        j = 1ULL << p[i];
        if((checklist & j) == 0){
            to_check[k] = p[i];
            k++;
        }
        if((j & accettabili[i]) == 0){ //mi assicuro che la lettera possa andare nella i-esima posizione
            return 1;
        }
        counter[p[i]]++;
        i++;
    }
    to_check[k] = -1;
    k=0;
    while(to_check[k] != -1){
        j = 1ULL << to_check[k];
        if((j & Global_sbagliate) > 0){ //il flag sbagliate mi garantisce di conoscere il numero esatto
            if(counter[to_check[k]] != Global_counter[to_check[k]]){
                return 1;
            }
        }
        else if(counter[to_check[k]] < Global_counter[to_check[k]]){ //la lettera non appare abbastanza volte
            return 1;
        }
        k++;
    }
    return 0;
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

void quick_inorder(node* visit){
    if(!visit->grey_l){
        quick_inorder(visit->l);
    }
    if(!visit->scartata){
        char* str = (char*) visit+sizeof(node);
        int i;
        char str_out[len+1];
        str_out[len] = '\n';
        for(i=0; i<len; i++){
            ASC(str[i],str_out[i])
        }
        fwrite(str_out,1,len+1,stdout);
    }
    if(!visit->grey_r){
        quick_inorder(visit->r);
    }
}

void inorder(node* visit){
    if(visit->l != NULL){
        inorder(visit->l);
    }
    if(!visit->scartata) {
        char *str = (char *) visit + sizeof(node);
        int i;
        char str_out[len + 1];
        str_out[len] = '\n';
        for (i = 0; i < len; i++) {
            ASC(str[i], str_out[i])
        }
        fwrite(str_out, 1, len + 1, stdout);
    }
    if(visit->r != NULL){
        inorder(visit->r);
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
}

void RB_Insert(node* z){
    node* y = NULL;
    node* x = root;
    while(x != NULL) {
        y = x;
        if (memcmp((int8_t *) z+sizeof(node),(int8_t *) x+sizeof(node),len)<0) {
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
    else if(memcmp((int8_t *) z+sizeof(node),(int8_t *) y+sizeof(node),len)<0){
        y->l = z;
    }
    else{
        y->r = z;
    }
    RB_Insert_Fixup(z);
}

#define SIZE 4096
static char* H = NULL;
static unsigned int h = 0;

void create_and_insert(int8_t* str, int8_t scartata){
    if(h % SIZE == 0){
        //printf("MALLOC\n");
        H = (char*) malloc((sizeof(node)+len) * SIZE);
        h = 0;
    }
    //printf("%d\n",h);
    node* a = (node*) (H + (sizeof(node) + len)*h);
    h++;
    memcpy((int8_t *) a + sizeof(node),str,len);
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
}

int binary_search(int8_t * target){//ricerca binaria in BST, restituisce 1 se trovato, 0 altrimenti
    node* temp = root;
    int c = 1;
    while(c!=0 && temp != NULL){
        c = memcmp(target,(int8_t *) temp+sizeof(node),len);
        if(c < 0){
            temp = temp->l;
        }
        else if(c > 0) {
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

uint8_t set_vincoli_recursive(node* x, uint64_t accettabili[]){
    if(!x->scartata && scarta_update((int8_t *) x+sizeof(node),accettabili)){
        x->scartata = 1;
        filtrate_counter--;
    }
    if(x->l != NULL){
        x->grey_l = set_vincoli_recursive(x->l, accettabili);
    }
    else{
        x->grey_l = 1;
    }
    if(x->r != NULL){
        x->grey_r = set_vincoli_recursive(x->r, accettabili);
    }
    else{
        x->grey_r = 1;
    }
    return x->scartata & x->grey_l & x->grey_r;
}

uint8_t set_vincoli_recursive_mask(node* x, uint64_t accettabili[]){
    if(!x->scartata && scarta_updated_mask((int8_t *) x+sizeof(node),accettabili)){
        x->scartata = 1;
        filtrate_counter--;
    }
    if(x->l != NULL){
        x->grey_l = set_vincoli_recursive_mask(x->l, accettabili);
    }
    else{
        x->grey_l = 1;
    }
    if(x->r != NULL){
        x->grey_r = set_vincoli_recursive_mask(x->r, accettabili);
    }
    else{
        x->grey_r = 1;
    }
    return x->scartata & x->grey_l & x->grey_r;
}

uint8_t aggiorna_vincoli_recursive(node* x, uint64_t accettabili[]){ //return 1 se da li a sotto l'albero è tutto composto di scartate (grey)
    if(!x->scartata && scarta_update((int8_t *) x+sizeof(node),accettabili)){
        x->scartata = 1;
        filtrate_counter--;
    }
    if(!x->grey_l){
        x->grey_l = aggiorna_vincoli_recursive(x->l, accettabili);
    }
    if(!x->grey_r){
        x->grey_r = aggiorna_vincoli_recursive(x->r, accettabili);
    }
    return x->scartata & x->grey_l & x->grey_r;
}

uint8_t aggiorna_vincoli_recursive_mask(node* x, uint64_t accettabili[]){ //return 1 se da li a sotto l'albero è tutto composto di scartate (grey)
    if(!x->scartata && scarta_updated_mask((int8_t *) x+sizeof(node),accettabili)){
        x->scartata = 1;
        filtrate_counter--;
    }
    if(!x->grey_l){
        x->grey_l = aggiorna_vincoli_recursive_mask(x->l, accettabili);
    }
    if(!x->grey_r){
        x->grey_r = aggiorna_vincoli_recursive_mask(x->r, accettabili);
    }
    return x->scartata & x->grey_l & x->grey_r;
}

void ripristina_vincoli(node* x){
    x->scartata = 0;
    x->grey_l = 0;
    x->grey_r = 0;
    if(x->l != NULL){
        ripristina_vincoli(x->l);
    }
    if(x->r != NULL){
        ripristina_vincoli(x->r);
    }
}

//Il main funge da parser e gestisce le chiamate alle funzioni necessarie per gestire le strutture dati

int main(){
    //freopen("/home/giacomo/Scaricati/n128000_k5_g200_test1.txt","r",stdin);
    //clock_t begin = clock();
    int s = 0, set = 0, sc = 0;
    long unsigned int tries = 0;
    int8_t command = 0;
    s = scanf("%d",&len);
    if(s == 0){
        return 1;
    }
    int8_t temp[len];
    int8_t reference[len];
    char output[len];
    int8_t vi[len+1];
    vincoli_aggiornati = vi;
    memset(vincoli_aggiornati,-1,len+1);
    uint64_t accettabili[len]; //vettore di bitmask per segnare le lettere accettate/vietate/obbligatorie in ogni pos
    temp[0] = getchar(); //leggo \n che scanf ignora
    command = get_input(temp);
    while(command == 0){ //inserimento iniziale di parole
        create_and_insert(temp,0);
        total_counter++;
        command = get_input(temp);
    }
    while(command != -1){
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
            s = scanf("%lu",&tries); //assegno i tentativi
            temp[0] = getchar(); //leggo anche \n
            command = get_input(temp);
        }
        else if(command == 's'){ //stampa filtrate
            if(!set){
                quick_inorder(root);
            }
            else{
                inorder(root);
            }
            command = get_input(temp);
        }
        else if(command  == 'i'){ //inserisci inizio
            command = get_input(temp);
            set = 1;
            while(command == 0){
                total_counter++;
                if(end == 0){
                    sc = scarta(temp,reference,accettabili);
                    create_and_insert(temp,sc);
                    if(!sc){
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
            while(command == 0 && end == 0 && tries > 0){
                if(binary_search(temp)){
                    updated_count = 0;
                    confronta(temp,reference,output,accettabili);
                    if(end == 1){
                        printf("ok\n");
                    }
                    else{
                        if(filtrate_counter != 1){
                            if(set == 1){
                                set = 0;
                                if(updated_count){
                                    set_vincoli_recursive(root,accettabili);
                                }
                                else{
                                    set_vincoli_recursive_mask(root,accettabili);
                                }
                            }
                            else{
                                if(updated_count){
                                    aggiorna_vincoli_recursive(root,accettabili);
                                }
                                else{
                                    aggiorna_vincoli_recursive_mask(root,accettabili);
                                }
                            }
                        }
                        fwrite(output,1,len,stdout);
                        printf("\n%d\n",filtrate_counter);
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
    /*clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    FILE* fp = fopen("/home/giacomo/Scaricati/time_spent.txt","w");
    fprintf(fp,"%f",time_spent);*/
    return 0;
}