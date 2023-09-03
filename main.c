/* Project: Final Project for API class in Politecnico di Milano
 * Topics of Exam: Algorithms, data structures and automata theory
 * Mark: 30/30
 * @author: Giacomo Brunetta
 * Date: 12/9/2022
 * Description: The following code handles a game in which the user has to guess a word. The program must handle a dictionary
 * of words, tell the user which letter in the guess are correct, which one are wrong and which one are in the wrong position.
 * Every turn the program should keep count of the strings that are compatible with the suggestions given to the user and
 * be able to print those strings in order.
 * Strings can be added to the dictionary at the state of every turn.
 * The dictionary is implemented as a red black tree and the constraints on the strings are marked on hash tables.
 * Hash tables of boolean are implemented as long ints, while hash tables of integers are implemented as vectors.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//the following macros are used as hash functions in the bitmask. H(c): 64 possible characters -> [0,63] (position of bit in bitflag)
//POS is h: character -> hash of character
#define POS(A,B) if(A <= 'Z' && A >= 'A') B = A - 'A' + 11;\
else if(A <= 'z' && A >= 'a') B = A - 'a' + 38;\
else if(A >= '0' && A <= '9') B = A - '0' + 1;\
else if(A == '_') B = 37;\
else B = 0;
//ASC is h^-1: hash of character -> character
#define ASC(A,B) if(A >= 38) B = A - 38 + 'a';\
else if(A >= 11 && A <= 36) B = A - 11 + 'A'; \
else if(A>= 1 && A <= 10) B = A - 1 + '0';    \
else if(A == 37) B = '_';                     \
else B = '-';

//global variables are defined here
static int len = 0; //length of strings, given as input
static unsigned int filtered_strings_counter = 0, total_counter = 0; //counters to return as output
static uint8_t letters_tally_table[64];
static int8_t* updated_constraints;
static uint64_t letters_not_in_word = 0ULL;
static int state=2; //state = 2 is the first match, state = 1 state of a match, state = 0 other
static uint8_t updated_count;

//this part of the code handles getting, comparing and sorting strings in the dictionary (filtered/discarded)
uint8_t get_input(uint8_t temp[]){ //reads input and saves the hashed version of the string (by hashing every character)
    char c;
    uint8_t flag = 0;
    int i = 0, k;
    c = getc(stdin);
    if(c == EOF){
        return 'e';
    }
    while(c != '\n'){
        if(c == '+'){
            c = getc(stdin);
            flag = c; //the command is identified by the first letter after the +
        }
        else if(flag == 0){
            POS(c,k);
            temp[i] = k;
            i++;
        }
        c = getc(stdin);
    }
    return flag;
}

//compares string p with reference string r and updates constraints on the dictionary
void compate_strings(uint8_t p[], uint8_t r[], char out[], uint64_t acceptable_letters[]){
    uint8_t temp_counter[64], counter[64];
    uint64_t j = 1ULL;
    uint64_t aggiorna = 0ULL;
    memset(counter,0,64);
    memset(temp_counter,0,64);
    int v = 0, i = 0, k;
    state = 1;
    for(i=0;i<len;i++){
        j = 1ULL << p[i];
        if(p[i] == r[i]){
            out[i] = '+'; //mark the correct letters
            counter[p[i]]++; //count correct letters in p
            if(counter[p[i]] > letters_tally_table[p[i]]){ //new max number of a letter, update constraints
                letters_tally_table[p[i]] = counter[p[i]];
                if((aggiorna & j) == 0){
                    updated_count = 1;
                    updated_constraints[v] = p[i];
                    v++;
                    aggiorna |= j;
                }
            }
            if(acceptable_letters[i] != j){
                acceptable_letters[i] = j; //mark the letter as the only acceptable letter in position i
            }
        }
        else{
            state = 0;
            out[i] = 0;
            temp_counter[r[i]]++; //mark the letters of r with a mismatch in p
            if((acceptable_letters[i] & j) > 0){
                acceptable_letters[i] &= ~j; //mark the letter as not acceptable in ith position
            }
        }
    }
    for(i=0;i<len;i++){ //assign / and |
        if(out[i] == 0){
            j = 1ULL << p[i];
            if(temp_counter[p[i]] > 0){ //right letters in wrong position
                out[i] = '|';
                counter[p[i]]++;
                temp_counter[p[i]]--;
                if(counter[p[i]] > letters_tally_table[p[i]]){ //new max of legal appearence of letter
                    letters_tally_table[p[i]] = counter[p[i]];
                    if((aggiorna & j) == 0){
                        updated_constraints[v] = p[i];
                        updated_count = 1;
                        v++;
                        aggiorna |= j;
                    }
                }
            }
            else{
                out[i] = '/';
                if((letters_not_in_word & j) == 0){
                    letters_not_in_word |= j;
                    if(counter[p[i]] == 0){
                        for(k=0;k<len;k++){
                            acceptable_letters[k] &= ~j;
                        }
                    }
                    if((aggiorna & j) == 0){
                        updated_constraints[v] = p[i];
                        updated_count = 1;
                        v++;
                        aggiorna |= j;
                    }
                }
            }
        }
    }
    updated_constraints[v] = -1;
    return;
}

//discards words in the dictionary accordingly with the most recent updates of constraints
int discard_words_updated_constraints_only(uint8_t p[], uint64_t acceptable_letters[]){
    int i=0, k=0;
    uint64_t j;
    uint8_t counter[64];
    memset(counter,0,64);
    while(i<len){
        j = 1ULL << p[i];
        if((j & acceptable_letters[i]) == 0){
            return 1;
        }
        counter[p[i]]++;
        i++;
    }
    while(updated_constraints[k] != -1){
        j = 1ULL << updated_constraints[k];
        if((j & letters_not_in_word) > 0){ //il flag sbagliate mi garantisce di conoscere il numero esatto
            if(counter[updated_constraints[k]] != letters_tally_table[updated_constraints[k]]){
                return 1;
            }
        }
        else if(counter[updated_constraints[k]] < letters_tally_table[updated_constraints[k]]){ //la lettera non appare abbastanza volte
            return 1;
        }
        k++;
    }
    return 0;
}
//discards words in the dictionary accordingly with the most recent updates in the bitmasks
int discard_words_updated_mask_only(uint8_t p[], uint64_t acceptable_letters[]){
    int i=0;
    uint64_t j;
    while(i<len){
        j = 1ULL << p[i];
        if((j & acceptable_letters[i]) == 0){
            return 1;
        }
        i++;
    }
    return 0;
}
//discards words in the dictionary accordingly with all the constraints
int discard_words(uint8_t p[], uint8_t r[], uint64_t acceptable_letters[]){
    uint64_t j;
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
        if((j & acceptable_letters[i]) == 0){
            return 1;
        }
        counter[p[i]]++;
        i++;
    }
    to_check[k] = -1;
    k=0;
    while(to_check[k] != -1){
        j = 1ULL << to_check[k];
        if((j & letters_not_in_word) > 0){
            if(counter[to_check[k]] != letters_tally_table[to_check[k]]){
                return 1;
            }
        }
        else if(counter[to_check[k]] < letters_tally_table[to_check[k]]){
            return 1;
        }
        k++;
    }
    return 0;
}

//The following part implements RED BLACK trees accordingly to the textbook "Introduction to Algorithms" by T. Cormen
//Two additional flag are added to the node. One marks discarded words, the other allows the binary visit to traverse
// only the nodes that could be updated if a branch of the tree contains only words that are already discarded
// (and no new words where added) that part of the
// tree will not be visited.

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
        int i,k;
        for(i=0; i<len; i++){
            ASC(str[i],k);
            putc(k,stdout);
        }
        printf("\n");
    }
    if(!visit->grey_r){
        quick_inorder(visit->r);
    }
    return;
}

void inorder(node* visit){
    if(visit->l != NULL){
        inorder(visit->l);
    }
    if(!visit->scartata){
        char* str = (char*) visit+sizeof(node);
        int i, k;
        for(i=0; i<len; i++){
            ASC(str[i],k);
            putc(k,stdout);
        }
        printf("\n");
    }
    if(visit->r != NULL){
        inorder(visit->r);
    }
    return;
}

void left_rotate(node* x){
    node* y = x->r;
    if(y == NULL){
        return;
    }
    x->r = y->l;
    if(y->l != NULL){
        y->l->p = x;
    }
    y->p = x->p;
    if(x->p == NULL){
        root = y;
    }
    else if(x == x->p->l){
        x->p->l = y;
    }
    else{
        x->p->r = y;
    }
    y->l = x;
    x->p = y;
}

void right_rotate(node* x){
    node* y = x->l;
    if(y == NULL){
        return;
    }
    x->l = y->r;
    if(y->r != NULL){
        y->r->p = x;
    }
    y->p = x->p;
    if(x->p == NULL){
        root = y;
    }
    else if(x == x->p->r){
        x->p->r = y;
    }
    else{
        x->p->l = y;
    }
    y->r = x;
    x->p = y;
}

void RB_Insert_Fixup(node* x){
    node* y;
    while(x->p!=NULL && x->p->color==RED){
        if(x->p == x->p->p->l) {
            y = x->p->p->r;
            if (y != NULL && y->color == RED) {
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
        else{
            y = x->p->p->l;
            if(y != NULL && y->color == RED){
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
        if (memcmp((char*) z+sizeof(node),(char*) x+sizeof(node),len)<0) {
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
    else if(memcmp((char*) z+sizeof(node),(char*) y+sizeof(node),len)<0){
        y->l = z;
    }
    else{
        y->r = z;
    }
    RB_Insert_Fixup(z);
}

void create_and_insert(uint8_t* str, uint8_t scartata){
    node *a;
    a = (node*)malloc(sizeof(node)+len);
    memcpy((uint8_t *) a + sizeof(node),str,len);
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

int binary_search(uint8_t * target){// 1 if found, 0 otherwise
    node* temp = root;
    int c = 1;
    while(c!=0 && temp != NULL){
        c = memcmp(target,(uint8_t *) temp+sizeof(node),len);
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

//the following part implements RBtree visits that discard words that do not respect constraints
//the gray_l and gray_r indicate that there are only discarded words in the left and rigth subtree of the node
//gray bits are updated recursively using the return statements.

uint8_t set_constraints(node* x, uint64_t accettabili[]){
    if(!x->scartata && discard_words_updated_constraints_only((uint8_t *) x + sizeof(node), accettabili)){
        x->scartata = 1;
        filtered_strings_counter--;
    }
    if(x->l != NULL){
        x->grey_l = set_constraints(x->l, accettabili);
    }
    else{
        x->grey_l = 1;
    }
    if(x->r != NULL){
        x->grey_r = set_constraints(x->r, accettabili);
    }
    else{
        x->grey_r = 1;
    }
    return x->scartata & x->grey_l & x->grey_r;
}

uint8_t set_constraints_mask_only(node* x, uint64_t accettabili[]){ //update using bitmasks only
    if(!x->scartata && discard_words_updated_mask_only((uint8_t *) x + sizeof(node), accettabili)){
        x->scartata = 1;
        filtered_strings_counter--;
    }
    if(x->l != NULL){
        x->grey_l = set_constraints_mask_only(x->l, accettabili);
    }
    else{
        x->grey_l = 1;
    }
    if(x->r != NULL){
        x->grey_r = set_constraints_mask_only(x->r, accettabili);
    }
    else{
        x->grey_r = 1;
    }
    return x->scartata & x->grey_l & x->grey_r;
}

uint8_t update_constraints(node* x, uint64_t accettabili[]){
    if(!x->scartata && discard_words_updated_constraints_only((uint8_t *) x + sizeof(node), accettabili)){
        x->scartata = 1;
        filtered_strings_counter--;
    }
    if(!x->grey_l){
        x->grey_l = update_constraints(x->l, accettabili);
    }
    if(!x->grey_r){
        x->grey_r = update_constraints(x->r, accettabili);
    }
    return x->scartata & x->grey_l & x->grey_r;
}

uint8_t update_constraints_mask_only(node* x, uint64_t accettabili[]){
    if(!x->scartata && discard_words_updated_mask_only((uint8_t *) x + sizeof(node), accettabili)){
        x->scartata = 1;
        filtered_strings_counter--;
    }
    if(!x->grey_l){
        x->grey_l = update_constraints_mask_only(x->l, accettabili);
    }
    if(!x->grey_r){
        x->grey_r = update_constraints_mask_only(x->r, accettabili);
    }
    return x->scartata & x->grey_l & x->grey_r;
}

void reset_constraints(node* x){
    x->scartata = 0;
    x->grey_l = 0;
    x->grey_r = 0;
    if(x->l != NULL){
        reset_constraints(x->l);
    }
    if(x->r != NULL){
        reset_constraints(x->r);
    }
    return;
}

void RB_del(node* geri){
    if(geri->l != NULL){
        RB_del(geri->l);
    }
    if(geri->r != NULL){
        RB_del(geri->r);
    }
    free(geri);
}

//main function parses the input and creates the data structures. when the match begins a state machine selects
//wich tasks to complete. The state is updated by input commands that start with a +

int main(){
    int tries, s = 0, set = 0, sc = 0;
    char command = 0;
    s = scanf("%d",&len);
    if(s == 0){
        return 1;
    }
    uint8_t temp[len];
    uint8_t reference[len];
    char output[len];
    int8_t vi[len+1];
    updated_constraints = vi;
    uint64_t acceptable_letters[len];
    temp[0] = getc(stdin);
    command = get_input(temp);
    while(command == 0){
        create_and_insert(temp,0);
        total_counter++;
        command = get_input(temp);
    }
    while(command != 'e'){
        if(command == 'n'){
            letters_not_in_word = 0ULL;
            memset(letters_tally_table, 0, 64);
            if(state != 2){
                reset_constraints(root);
            }
            filtered_strings_counter = total_counter;
            memset(acceptable_letters, UINT8_MAX, len * 8);
            state = 0;
            set = 1;
            command = get_input(reference);
            s = scanf("%d",&tries);
            temp[0] = getc(stdin);
            command = get_input(temp);
        }
        else if(command == 's'){
            if(!set){
                quick_inorder(root);
            }
            else{
                inorder(root);
            }
            command = get_input(temp);
        }
        else if(command  == 'i'){
            command = get_input(temp);
            set = 1;
            while(command == 0){
                total_counter++;
                if(state == 0){
                    sc = discard_words(temp, reference, acceptable_letters);
                    create_and_insert(temp,sc);
                    if(!sc){
                        filtered_strings_counter++;
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
            while(command == 0 && state == 0 && tries > 0){
                if(binary_search(temp)){
                    updated_count = 0;
                    compate_strings(temp, reference, output, acceptable_letters);
                    if(state == 1){
                        printf("ok\n");
                    }
                    else{
                        if(filtered_strings_counter != 1){
                            if(set == 1){
                                set = 0;
                                if(updated_count){
                                    set_constraints(root, acceptable_letters);
                                }
                                else{
                                    set_constraints_mask_only(root, acceptable_letters);
                                }
                            }
                            else{
                                if(updated_count){
                                    update_constraints(root, acceptable_letters);
                                }
                                else{
                                    update_constraints_mask_only(root, acceptable_letters);
                                }
                            }
                        }
                        fwrite(output,1,len,stdout);
                        printf("\n%d\n", filtered_strings_counter);
                    }
                    tries--;
                }
                else{
                    printf("not_exists\n");
                }
                command = get_input(temp);
            }
            if(tries == 0 && state == 0){
                state = 1;
                printf("ko\n");
            }
        }
    }
    RB_del(root);
    return 0;
}