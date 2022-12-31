#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define MAX_LEN 20      //최대문자열
#define MNEMO_LEN 4     //mnemonic1 길이

typedef struct AssemblyNode {   //intermed File을 구성하기 위한 리스트의 노드
    int address;
    char *label;
    char *opcode;
    char *operand;
    struct AssemblyNode *next;
} AssemblyNode;

typedef struct SymbolNode { //Symbol Table을 구성하기 위한 노드
    int address;            //Tree구성할시 key값
    char *operand;
    struct SymbolNode *left;
    struct SymbolNode *right;
} SymbolNode;

AssemblyNode *newAssemblyNode(int address, char *label, char *opcode, char *operand);   //Intermed List 구성하는 노드 생성
SymbolNode *newSymbolNode(int address, char *operand);  //symbol table 구성하는 노드 생성
SymbolNode *insertTree(SymbolNode *root, SymbolNode *node); //symbol table에 노드 삽입
SymbolNode *makeAVL(SymbolNode *root);  //symbol table에 노드 삽입시 AVL Tree로 변환
SymbolNode *rotateRight(SymbolNode *parent);    //서브트리를 오른쪽 회전
SymbolNode *rotateLeft(SymbolNode *parent);     //서브트리를 왼쪽으로 회전
int height(SymbolNode *root);   //트리의 left, right 서브트리의 높이 반환
int balance(SymbolNode *root);  //left, right서브트리 높이 차이 반환
int findAddress(SymbolNode *root, char *operand);   //operand에 해당하는 주소 반환
void insertList(AssemblyNode **head, AssemblyNode **tail, AssemblyNode *node);  //Tail을 사용하여 list 마지막에 노드 삽입
void printList(AssemblyNode *head); //Intermed File List 출력
int main(int argc, char *argv[]) {
    char opcode[10], operand[10], label[10], a[10], ad[10], symbol[10], ch;
    char code[10][10], code1[4][3] = {"33", "44", "53", "57"};
    char mnemonic[6][6] = {"START", "LDA", "STA", "LDCH", "STCH", "END"};
    char mnemonic1[4][5] = {"LDA", "STA", "LDCH", "STCH"};
    int locctr, start, length, i = 0, j = 0, k, l = 0;
    int st, diff, address, add, len, actual_len, finaddr, prevaddr;

    FILE *fpInput = NULL;
    AssemblyNode *intermHead = NULL;   //List의 head
    AssemblyNode *intermTail = NULL;   //List 삽입시 순회하는 것이 아닌 Tail에 삽입
    AssemblyNode *temp = NULL;         //List 순회연산을 intermHead대신 수행
    SymbolNode *symbolRoot = NULL;     //Tree의 root
    SymbolNode *mnemonicRoot = NULL;   //명령어를 상수코드로 변환하기 위한 Tree의 root

    getopt(argc, argv, "i:");   //"프로그랭명 -i 문자열" 형태로 프로그램 실행시 unistd.h 의 optarg변수에 문자열 저장

    //input 파일 출력, intermed 파일 생성.
    fpInput = fopen(optarg, "r");
    if (fpInput == NULL) return 0;
    printf("The contents of Input file:\n\n");
    locctr = -1;    //의미 없는 값(최초의 임의의 주소값)
    while (1) {
        fscanf(fpInput, "%s%s%s", label, opcode, operand);  //코드 한 줄 읽기
        printf("%s %s %s\n", label, opcode, operand);   //Input File 출력
        insertList(&intermHead, &intermTail, newAssemblyNode(locctr, label, opcode, operand));  //Intermed File List구성
        add = 0;    //현재 코드가 할당 받는 메모리 크기 저장
        if (strcmp(opcode, "START") == 0) { //코드의 시작
            locctr = atoi(operand); //각 코드의 주소를 저장(주소 초기화)
            start = locctr;  //코드의 시작주소
        } else if (strcmp(opcode, "END") == 0) {  //코드의 마지막
            break;
        } else if (strcmp(label, "**") == 0 && strcmp(opcode, "END") != 0) { //명령어
            add = 3;
        } else if (strcmp(label, "**") != 0 && strcmp(opcode, "START") != 0) { //Storage
            if (strcmp(opcode, "WORD") == 0) {  //WORD == 3byte
                add = 3;
            } else if (strcmp(opcode, "BYTE") == 0) {
                add = 1;
            } else if (strcmp(opcode, "RESW") == 0) {   //WORD 예약, 3*operand
                add = 3 * atoi(operand);
            } else if (strcmp(opcode, "RESB") == 0) {   //BYTE 예약, 1*operand
                add = atoi(operand);
            }
        }
        locctr += add;  //코드 한줄이 사용한 크기만큼 더함
    }
    fclose(fpInput);
    finaddr = locctr;   //마지막 주소("END" 주소) 저장
    printf("\nLength of the input program is %d.\n", finaddr - start);   //프로그램 길이 = 마지막 주소 - 시작 주소

    //symbol table 생성
    printf("\nThe contents of Symbol Table:\n\n");
    temp = intermHead->next;  //START이후로 판단
    while (strcmp(temp->opcode, "END") != 0) {
        if (strcmp(temp->label, "**") != 0) { //Storage일 경우 Symbol Table에 저장
            SymbolNode *sNode = newSymbolNode(temp->address,temp->label);  //intermed file node의 데이터로 symbol node 생성(Storage에서는 lable이 operand)
            printf("%s\t%d\n", sNode->operand, sNode->address); //Symbol Table을 AVL Tree로 구성한 특성 탓에 주소가 작은 순서대로 출력이 안되기 때문에 삽입전 출력
            symbolRoot = insertTree(symbolRoot, sNode); //출력후 생성한 symbol node 삽입
        }
        temp = temp->next;  //intermed file list 순회
    }
    printf("\n\nThe contents of Intermed File:\n\n");
    printList(intermHead);  //symbol table 출력후 intermed file 출력

    //Object Program 생성
    for (i = 0; i < MNEMO_LEN; i++)  //mnemonic1, code1 배열을 이용한 AVL Tree 생성, 명령어를 상수코드로 변환할때 사용
        mnemonicRoot = insertTree(mnemonicRoot, newSymbolNode(atoi(code1[i]), mnemonic1[i]));
    temp = intermHead;
    printf("\nObject Program has been generated.\n\n");
    printf("Object Program:\n\n");
    //Header
    printf("H^%s^%06d^%06d\n", temp->label, start, finaddr);   //COPY 시작주소 마지막주소 순으로 출력(주소는 6자리로 출력)
    //Text
    temp = temp->next;    //START를 처리 했기때문에 Text부분으로 이동
    printf("T^%06d^%d", start, finaddr - start - 1); //Haeder와 Text 총 바이트수(finaddr-1까지 메모리 사용)
    while (strcmp(temp->opcode, "END") != 0) {  //END직전 까지 intermed file list를 순회
        if (strcmp(temp->label, "**") == 0) {   //mnemonic 일경우(명령어)
            int mnemonicAddr = findAddress(mnemonicRoot, temp->opcode); //mnemonic Tree에서 opcode 주소값 반환
            int operandAddr = findAddress(symbolRoot, temp->operand);   //symbol table에서 operand 주소값 반환
            //둘다 0이 아닐때 출력(0이면 오류로 간주), mnemonicAddr + operandAddr
            if (mnemonicAddr != 0 && operandAddr != 0) printf("^%d%d", mnemonicAddr, operandAddr);
        } else {    //Storage
            if (strcmp(temp->opcode, "WORD") == 0) { //10진수 operand
                printf("^%06d", atoi(temp->operand));    //10진수 6자리로 출력
            } else if (strcmp(temp->opcode, "BYTE") == 0) { //16진수 operand
                printf("^");
                for (i = 2; i < strlen(temp->operand) - 1; i++) {    //C''를 제외하고 변환(문자열 인덱스 2부터 마지막-1 까지)
                    printf("%02x", temp->operand[i]); //문자 하나를 16진수로 출력
                }
            }
        }
        temp = temp->next;
    }
    //END
    printf("\nE^%06d\n", start); //시작주소 6자리로 출력
    return 0;
}

AssemblyNode *newAssemblyNode(int address, char *label, char *opcode, char *operand) {
    AssemblyNode *node = (AssemblyNode *) malloc(sizeof(AssemblyNode));
    node->address = address;
    node->label = (char *) malloc(MAX_LEN);
    node->opcode = (char *) malloc(MAX_LEN);
    node->operand = (char *) malloc(MAX_LEN);
    strcpy(node->label, label);
    strcpy(node->opcode, opcode);
    strcpy(node->operand, operand);
    node->next = NULL;
    return node;
}

void insertList(AssemblyNode **head, AssemblyNode **tail, AssemblyNode *node) {
    if (*head == NULL) {
        *head = node;
        *tail = node;
        return;
    }
    (*tail)->next = node;   //순회하는 것이 아니라 tail변수를 주어 마지막에 추가
    *tail = node;
}

void printList(AssemblyNode *head) {
    if (head == NULL) return;
    AssemblyNode *temp = head;
    printf("%s\t%s\t%s\n", temp->label, temp->opcode, temp->operand);   //START 예외처리
    temp = temp->next;
    while (temp != NULL) {
        printf("%d\t%s\t%s\t%s\n", temp->address, temp->label, temp->opcode, temp->operand);
        temp = temp->next;
    }
}

SymbolNode *newSymbolNode(int address, char *operand) {
    SymbolNode *node = (SymbolNode *) malloc(sizeof(SymbolNode));
    node->address = address;
    node->operand = (char *) malloc(MAX_LEN);
    strcpy(node->operand, operand);
    node->left = NULL;
    node->right = NULL;
    return node;
}

SymbolNode *insertTree(SymbolNode *root, SymbolNode *node) {
    if (root == NULL) return node;  //leaf node에 연결할 노드
    if (strcmp(root->operand, node->operand) > 0) root->left = insertTree(root->left, node); //작으면 왼쪽 서브트리
    else if (strcmp(root->operand, node->operand) < 0)
        root->right = insertTree(root->right, node);   //크면 오른쪽 서브 트리(중복된 값을 없다고 가정)
    root = makeAVL(root);
    return root;    //insert완료된 root 반환
}

SymbolNode *makeAVL(SymbolNode *root) {
    if (balance(root) > 1) {    //왼쪽 서브트리의 불균형
        SymbolNode *tempNode = root->left;  //LL인지 LR인지 판단하기위해 left를 기준으로 확인
        if (height(tempNode->left) >= height(tempNode->right)) {    //LL
            root = rotateRight(root);    //root->left가 새로운 root가 되고 기존의 루트는 오른쪽 child로 변환
        } else { //LR
            root->left = rotateLeft(root->left);    //root->left->right가 root->left에 위치하고 right는 left로 변환하여 LL 상태로 변환
            root = rotateRight(root);
        }
    } else if (balance(root) < -1) {   //오른쪽 서브트리의 불균형
        SymbolNode *tempNode = root->right;
        if (height(tempNode->left) <= height(tempNode->right)) {   //RR
            root = rotateLeft(root);
        } else {  //RL
            root->right = rotateRight(root->right);
            root = rotateLeft(root);
        }
    }
    return root;
}

int height(SymbolNode *root) {
    if (root == NULL) { //리프 노트까지 탐색
        return 0;
    } else {
        int left = height(root->left);
        int right = height(root->right);
        return 1 + (left > right ? left : right);
    }
}

int balance(SymbolNode *root) {
    if (root == NULL) return 0;
    else return height(root->left) - height(root->right);    //좌,우 서브트리의 높이 차이가 1초과이면 rotate수행
}

SymbolNode *rotateRight(SymbolNode *parent) {
    SymbolNode *child = parent->left;
    parent->left = child->right;
    child->right = parent;
    return child;
}

SymbolNode *rotateLeft(SymbolNode *parent) {
    SymbolNode *child = parent->right;
    parent->right = child->left;
    child->left = parent;
    return child;
}

int findAddress(SymbolNode *root, char *operand) {
    if (root == NULL) return 0;
    if (strcmp(root->operand, operand) == 0)
        return root->address;
    if (strcmp(root->operand, operand) > 0) {
        findAddress(root->left, operand);
    } else if (strcmp(root->operand, operand) < 0) {
        findAddress(root->right, operand);
    }
}