#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define M 5
#define SEED 10
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


typedef struct Node {
    int leaf;
    int count;
    int value[M];
    struct Node* child[M + 1];
    struct Node* successor;
    struct Node* predecessor;
    struct Node* parent;
} Node;

Node* root = NULL;
Node* split();
void recover();

Node* newNode(){
    Node* tmp = (Node *)malloc(sizeof(Node));
    tmp->leaf = 1;
    tmp->count = 0;
    for(int i = 0 ; i < M ; i ++) tmp->value[i] = 0;
    for(int i = 0 ; i <= M ; i ++) tmp->child[i] = NULL;
    tmp->successor = NULL;
    tmp->predecessor = NULL;
    tmp->parent = NULL;
    return tmp;
}

Node* find(int value){
    Node* node = root;
    while(!node->leaf){
        int flag = 0;
        for(int i = 0; i < node->count; i++){
            if(value<node->value[i]){
                node = node->child[i];
                flag = 1;
                break;
            }
        } if (flag == 0){
            node = node->child[node->count];
        }
    }
    return node;
}

void leafInsert(Node* leafNode, int value){
    for (int i = 0 ; i < M ; i ++){
         if (leafNode->value[i] == 0) {
            leafNode->value[i] = value;
            break;
        } else if(value<leafNode->value[i]) {
            for(int j = M-1 ; j > i ; j--) leafNode->value[j] = leafNode->value[j-1];
            leafNode->value[i] = value;
            break;
        }
    }

    leafNode->count++;
}

void indexInsert(Node* indexNode, int value, Node* left, Node* right){
    for (int i = indexNode->count ; i >= 0 ; i --){
        if (indexNode->child[i]==left){
            indexNode->value[i] = value;
            indexNode->child[i+1] = right;

            if(right->leaf){
                if (i<indexNode->count) {
                    indexNode->child[i+1]->successor=indexNode->child[i+2];
                    indexNode->child[i+2]->predecessor=indexNode->child[i+1];
                } 
            } break;
        } else{
            indexNode->value[i] = indexNode->value[i-1];
            indexNode->child[i+1] = indexNode->child[i];
        }
    }

    indexNode->count++;
}


Node* split(Node* node){

    int value = node->value[M/2];
    Node* sibling = newNode();
    sibling->parent = node->parent;
    sibling->leaf = node->leaf;

    //리프 노드 분할
    if (node->leaf) {
        for (int i = M/2 ; i < M ; i ++){
            sibling->value[i-M/2] = node->value[i];
            node->value[i] = 0;
        }
    
        node->count = M/2;
        sibling->count = (M+M%2)/2;

        sibling->successor = node->successor;
        node->successor = sibling;
        if(sibling->successor!=NULL) sibling->successor->predecessor = sibling;
        sibling->predecessor=node;

    //인덱스 노드 분할
    } else {
        node->value[M/2] = 0;
        for (int i = M/2+1 ; i < M ; i ++){
            sibling->value[i-M/2-1] = node->value[i];
            node->value[i] = 0;
        }
        node->count = M/2;
        sibling->count = (M-M%2)/2;

        for(int i = (M+1)/2 ; i < M+1 ; i++){
            sibling->child[i-(M+1)/2] = node->child[i];
            node->child[i]->parent = sibling;
            node->child[i] = NULL;
        }
    }

    //부모 노드 Insert
    if (node->parent == NULL) {
        Node* tmp = newNode();
        tmp->leaf = 0;
        tmp->value[0] = value;
        tmp->count++;
        tmp->child[0] = node;
        tmp->child[1] = sibling;
        node->parent = tmp;
        sibling->parent = tmp;
        root = tmp;
        
    } else {
        indexInsert(node->parent,value,node,sibling);
        if (node->parent->count == M) split(node->parent);
    }
    

    
}

void insert(int value) {
    //첫 Insert
    if(root == NULL) {
        Node* tmp =  newNode();
        
        leafInsert(tmp, value);
        root = tmp;

    //기본 Insert
    } else{
        Node* leafNode = find(value);
        leafInsert(leafNode, value);
        if(leafNode->count==M) split(leafNode);
    }
}

void printTree(){
    Node* node = root;
    while(!node->leaf){
        node = node->child[0];
    }
    while(1){
        for(int i = 0 ; i < node->count ; i++){
            printf("%d ",node->value[i]);
        }
        if (node->successor) node = node->successor;
        else break;
    }
}   

void IndexChange(Node* node, int before, int after){
    Node* up = node->parent;
    while(up != NULL){
        int flag = 0;
        for (int i = 0 ; i < up->count ; i++) {
            if(up->value[i] == before) {
                up->value[i] = after;
                flag = 1;
            }
        } if (flag) break;
        else up = up->parent;
    }
}

void leafDelete(Node* node, int value){
    if (node->value[0] == value) IndexChange(node,value,node->value[1]);

    for(int i = 0; i < node->count ; i++){
        if(node->value[i]==value){
            for(int j = i ; j < node->count ; j++){
                node->value[j] = node->value[j+1];
            }
        }
    } node->count--;
}

int searchParentByLeft(Node* parent, int leafValue){
    for(int i = 0; i < parent->count; i++){
        if (leafValue<parent->value[i]){
            return parent->value[i];
        }
    } return 0;
}

int searchParentByRight(Node* parent, int leafValue){
    for(int i = parent->count-1 ; i >= 0 ; i--){
        if (parent->value[i] <= leafValue){
            return parent->value[i];
        }
    } return 0;
}

void treeDelete(Node* parent, int value){
    for(int i = 0 ; i < parent->count ; i++){
        if(parent->value[i] == value){
            for(int j = i ; j < parent->count; j++){
                parent->value[j] = parent->value[j+1];
                parent->child[j+1] = parent->child[j+2];
            } break;
        } 
    }
    parent->count--;
}

void _treeDelete(Node* parent, int value){
    for(int i = 0 ; i < parent->count ; i++){
        if(parent->value[i] == value){
            for(int j = i ; j < parent->count+1; j++){
                parent->value[j] = parent->value[j+1];
                parent->child[j] = parent->child[j+1];
            } break;
        } 
    }
    parent->count--;
}


void verification(Node* node){
    int min = M/2+M%2-1;

    if (node->parent != NULL&&node->count < min) recover(node);

}

Node* leftChild(Node* node,int value){
    for (int i = 0 ; i < node->count ; i++){
        if(node->value[i] == value) return node->child[i];
    }
}

Node* rightChild(Node* node,int value){
    for (int i = 0 ; i < node->count ; i++){
        if(node->value[i] == value) return node->child[i+1];
    }
}

void mergeRoot(Node* parent){
    Node* leftChild = parent->child[0];
    Node* rightChild = parent->child[1];
    parent->leaf = leftChild->leaf;

    int p_value = 0;
    int c_value = 0;
    parent->count = 0;
    for(int i = 0 ; leftChild->value[i] ; i++){
        parent->value[p_value] = leftChild->value[i];
        p_value++;
        parent->count++;
    }

    for(int i = 0 ; rightChild->value[i] ; i++){
        parent->value[p_value] = rightChild->value[i];
        p_value++;
        parent->count++;
    }

    for(int i = 0 ; leftChild->child[i] != NULL ; i++){
        parent->child[c_value] = leftChild->child[i];
        leftChild->child[i]->parent = parent;
        c_value++;
    }

    for(int i = 0 ; rightChild->child[i] != NULL ; i++){
        parent->child[c_value] = rightChild->child[i];
        rightChild->child[i]->parent = parent;
        c_value++;
    }

    free(leftChild);
    free(rightChild);
}

void mergeTreeL(Node* left, Node* right, int parent){
    int l_count = left->count;
    leafInsert(left,parent);
    int r_count = right->count;
    for (int i = l_count+1; i < l_count+1+r_count ; i++ ){
        left->value[i] = right->value[i-l_count-1];
        left->count++;
    }
    for (int i = l_count+1; i < l_count+r_count+2;i++ ){
        left->child[i] = right->child[i-(l_count+1)];
        left->child[i]->parent=left;
    }
}

void recover(Node* leaf){
    int min = M/2+M%2-1;

    Node* successor = leaf->successor;
    Node* predecessor = leaf->predecessor;
    Node* parent = leaf->parent;
    Node* child; 

    int leftParent = searchParentByRight(parent,leaf->value[0]);
    int rightParent = searchParentByLeft(parent,leaf->value[0]);
    if(leftParent) child = leftChild(parent,leftParent);
    else child = rightChild(parent,rightParent);
    
    //전임자에게 빌림
    if (leftParent&&child->count > min){
        if(leaf->leaf){
            leafInsert(leaf,child->value[child->count-1]);
            IndexChange(leaf,leaf->value[1],leaf->value[0]);
            leafDelete(child, child->value[child->count-1]);
        } else{
            leafInsert(leaf,leftParent);
            IndexChange(child,leftParent,child->value[child->count-1]);
            leaf->child[2] = leaf->child[1];
            leaf->child[1] = leaf->child[0];
            leaf->child[0] = child->child[child->count];
            child->child[child->count]->parent = leaf;
            treeDelete(child,child->value[child->count-1]);
        }
    
    //후임자에게 빌림
    } else if (rightParent&&child->count > min){
        if(leaf->leaf){
            leafInsert(leaf,child->value[0]);
            IndexChange(child,child->value[0],child->value[1]);
            leafDelete(child, child->value[0]);
        } else{
            leafInsert(leaf,rightParent);
            IndexChange(child,rightParent,child->value[0]);
            leaf->child[leaf->count]=child->child[0];
            child->child[0]->parent=leaf;
            _treeDelete(child,child->value[0]);
        }

    //부모에게 빌림
    } else{
        //부모가 루트 라면
        if (parent->count==1){
            if(!leaf->leaf) leafInsert(leaf, parent->value[0]);
            mergeRoot(parent);

        } else{
            if(leaf->leaf){
                if(leftParent){
                    leafInsert(child,leaf->value[0]);
                    treeDelete(parent,leftParent); 
                    if(leaf->leaf&&successor!=NULL) {
                        predecessor->successor=successor;
                        successor->predecessor=child;
                    } else child->successor=NULL;

                } else if(rightParent){
                    leafInsert(child,leaf->value[0]);
                    IndexChange(leaf,rightParent,leaf->value[0]);
                    _treeDelete(parent,leaf->value[0]);
                    if(leaf->leaf&&predecessor!=NULL) {
                        predecessor->successor=child;
                        child->predecessor=predecessor;
                    } else child->predecessor=NULL;
                }
                free(leaf);
                verification(parent);
            } else{
                if(leftParent){
                    mergeTreeL(child,leaf,leftParent);
                    treeDelete(parent,leftParent);

                } else if(rightParent){
                    mergeTreeL(leaf,child,rightParent);
                    treeDelete(parent,rightParent);
                }
            }
        }
    }
}

void delete(int value){

    Node* leaf = find(value);
    
    leafDelete(leaf, value);
    verification(leaf);
}

int readNode(int value){
    Node* node = root;
    if (node == NULL) {
        printf("전부 삭제됨");
        return -1;
    }
    while(!node->leaf){
        int flag = 0;
        for(int i = 0; i < node->count; i++){
            if(value<node->value[i]){
                node = node->child[i];
                flag = 1;
                break;
            }
        } if (flag == 0){
            node = node->child[node->count];
        }
    }
    for (int i = 0 ; i < node->count ; i++){
        if(node->value[i] == value) return 1;
    } return 0;
}

void* _insertThread(void* arg){
    
    int n = *((int*) arg);

    int start = n*100+1;
    int end = (n+1)*100;

    
    for(int value = start ; value <= end ; value++ ){
        pthread_mutex_lock(&lock);
        printf("%d 쓰기 시도\n",value);
        insert(value);
        printf("%d 쓰기 성공\n",value);
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

void* _readThread(){
    while(1){
        int value = rand()%300+1;

        pthread_mutex_lock(&lock);
        printf("%d 읽기 시도\n",value);
        if(readNode(value)==1) {
            printf("%d 읽기 성공\n",value);
            pthread_mutex_unlock(&lock);
        }
        else if(readNode(value)==-1) {
            break;
        }
        else {
            printf("%d 읽기 실패\n",value);
            pthread_mutex_unlock(&lock);
            usleep(100000);
        }
    }
    pthread_exit(NULL);
}

void* _deleteThread(){
    while(1){
        int value = rand()%300+1;
        
        pthread_mutex_lock(&lock);
        printf("%d 삭제 시도\n",value);
        if(readNode(value)) {
            delete(value);
            printf("%d 삭제 성공\n",value);
            pthread_mutex_unlock(&lock);
        } else if(readNode(value)==-1) {
            break;
        } else {
            printf("%d 삭제 실패\n",value);
            pthread_mutex_unlock(&lock);
            usleep(100000);
        }
    }
    pthread_exit(NULL);
}

int main() {
    
    srand(SEED);

    pthread_t insertThread[3];
    pthread_t readThread[5];
    pthread_t deleteThread;

    int args[3];
    for(int i = 0 ; i < sizeof(insertThread) / sizeof(insertThread[0]) ; i ++){
        args[i] = i;
        pthread_create(&insertThread[i], NULL, _insertThread, &args[i]);
    }

    for(int i = 0 ; i < sizeof(readThread) / sizeof(readThread[0]) ; i ++){
        pthread_create(&readThread[i], NULL, _readThread, NULL);
    }

    pthread_create(&deleteThread, NULL, _deleteThread, NULL);

    
    for(int i = 0 ; i < sizeof(insertThread) / sizeof(insertThread[0]) ; i ++){
        pthread_join(insertThread[i], NULL);
    }

    for(int i = 0 ; i < sizeof(readThread) / sizeof(readThread[0]) ; i ++){
        pthread_join(readThread[i], NULL);
    }

    pthread_join(deleteThread, NULL);

    int value[] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,17,18,19,20,21,22,23,24};
    int num = sizeof(value) / sizeof(value[0]);

    // for (int i = 0; i < num; i++ ) {
    //     printf("\n\n%d 가 입력됩니다.\n",value[i]);
    //     insert(value[i]);
    //     printTree();
    //     printf("\n\n");
    // }

    // for (int i = 0; i < num; i++ ) {
    //     printf("\n\n%d 가 삭제됩니다.\n",value[i]);
    //     delete(value[i]);
    //     printTree();
    //     printf("\n\n");
    // }

    return 0;
}
