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

void nodeCopy(Node* tmp, Node* node){
    tmp->leaf = node->leaf;
    tmp->count = node->count;
    for(int i = 0 ; i < node->count ; i++) tmp->value[i] = node->value[i];
    for(int i = 0 ; i <= node->count ; i++) tmp->child[i] = node->child[i];
    tmp->successor = node->successor;
    tmp->predecessor = node->predecessor;
    tmp->parent = node->parent;
}


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

<<<<<<< HEAD
void leafDelete(Node* node, int value){
    if (node->value[0] == value) IndexChange(node,value,node->value[1]);

    for(int i = 0; i < node->count ; i++){
        if(node->value[i]==value){
            for(int j = i ; j < node->count ; j++){
                node->value[j] = node->value[j+1];
=======
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
    for(int i = 0; i < node->count ; i++){
        if(node->child[i]==value){
            for(int j = i ; j < node->count ; j++){
                node->child[i] = node->child[i+1];
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae
            }
        }
    } node->count--;
}

int searchParentByLeft(Node* parent, int leafValue){
    for(int i = 0; i < parent->count; i++){
<<<<<<< HEAD
        if (leafValue<parent->value[i]){
            return parent->value[i];
        }
    } return 0;
}

int searchParentByRight(Node* parent, int leafValue){
=======
        if (parent->value[i] < leafValue){
            return parent->value[i];
        }
    }
}

int searchParentByright(Node* parent, int leafValue){
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae
    for(int i = parent->count-1 ; i >= 0 ; i--){
        if (parent->value[i] <= leafValue){
            return parent->value[i];
        }
<<<<<<< HEAD
    } return 0;
=======
    }
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae
}

void treeDelete(Node* parent, int value){
    for(int i = 0 ; i < parent->count ; i++){
        if(parent->value[i] == value){
            for(int j = i ; j < parent->count; j++){
<<<<<<< HEAD
                parent->value[j] = parent->value[j+1];
                parent->child[j+1] = parent->child[j+2];
=======
                parent->value[i] = parent->value[i+1];
                parent->value[i+1] = parent->value[i+2];
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae
            } break;
        } 
    }
    parent->count--;
}

<<<<<<< HEAD
void verification(Node* node){
    int min = M/2+M%2-1;

    if (node->count < min) recover(node);

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

void mergeRoot(){
    Node* leftChild = root->child[0];
    Node* rightChild = root->child[1];
    root->leaf = leftChild->leaf;

    int p_value = 0;
    int c_value = 0;
    root->count = 0;
    for(int i = 0 ; leftChild->value[i] ; i++){
        root->value[p_value] = leftChild->value[i];
        p_value++;
        root->count++;
    }

    for(int i = 0 ; rightChild->value[i] ; i++){
        root->value[p_value] = rightChild->value[i];
        p_value++;
        root->count++;
    }

    for(int i = 0 ; leftChild->child[i] != NULL ; i++){
        root->child[c_value] = leftChild->child[i];
        c_value++;
    }

    for(int i = 0 ; rightChild->child[i] != NULL ; i++){
        root->child[c_value] = rightChild->child[i];
        c_value++;
    }
}

void recover(Node* leaf){
=======
Node* verification(Node* root, Node* node){
    int min = M/2+M%2-1;
    
    if (node == root) return root;
    else if (node->count < min) return recover(root, node);
    else return root;
}

Node* recover(Node* root, Node* leaf){
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae
    int min = M/2+M%2-1;

    Node* successor = leaf->successor;
    Node* predecessor = leaf->predecessor;
    Node* parent = leaf->parent;
    
    //전임자에게 빌림
<<<<<<< HEAD
    if (predecessor!=NULL&&predecessor->count > min){
=======
    if (predecessor->count > min){
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae

        leafInsert(leaf,predecessor->value[predecessor->count-1]);
        IndexChange(leaf,leaf->value[1],leaf->value[0]);
        leafDelete(predecessor, predecessor->value[predecessor->count-1]);
    
    //후임자에게 빌림
<<<<<<< HEAD
    } else if (successor!=NULL&&successor->count > min){
=======
    } else if (successor->count > min){
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae

        leafInsert(leaf,successor->value[0]);
        IndexChange(successor,successor->value[0],successor->value[1]);
        leafDelete(successor, successor->value[0]);

    //부모에게 빌림
    } else{
<<<<<<< HEAD
        //부모가 루트 라면
        if (parent == root){
            leafInsert(leaf, root->value[0]);
            mergeRoot();
        } else{

            int leftParent = searchParentByRight(parent,leaf->value[0]);
            int rightParent = searchParentByLeft(parent,leaf->value[0]);

            if(leftParent){
                Node* child = leftChild(parent,leftParent);
                leafInsert(child,leaf->value[0]);
                treeDelete(parent,leftParent); 

            } else if(rightParent){
                Node* child = rightChild(parent,rightParent);
                leafInsert(child,leaf->value[0]);
                IndexChange(leaf,rightParent,leaf->value[0]);
                treeDelete(parent,rightParent);
            }

            if(leaf->leaf){
                predecessor->successor=successor;
                successor->predecessor=predecessor;
                free(leaf);
            }
            verification(parent);
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


//printTree
//delete(root,value)
//insert(root,value)
//rand()
void* _insertThread(void* arg){
    
    int n = *((int*) arg);

    int start = n*100+1;
    int end = (n+1)*100;

    
    for(int value = start ; value <= end ; value++ ){
        pthread_mutex_lock(&lock);
        insert(value);
        pthread_mutex_unlock(&lock);
    }
    

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
=======

        if(predecessor!=NULL){
            int target = searchParentByright(parent,leaf->value[0]);
            leafInsert(predecessor,leaf->value[0]);
            treeDelete(parent,target); 

        } else if(successor!=NULL){
            int target = searchParentByLeft(parent,leaf->value[0]);
            leafInsert(successor,leaf->value[0]);
            IndexChange(leaf,target,leaf->value[0]);
            treeDelete(parent,target);
        }
        root = verification(root, parent);
    }
    return root;
}

Node* delete(Node* root, int value){

    Node* leaf = find(root,value);
    if (leaf->child[0] == value) IndexChange(leaf,value,leaf->child[1]);
    leafDelete(leaf, value);
    root = verification(root, leaf);

    return root;
}

int main() {
    Node* root = NULL;
    int value[] = {3, 11, 7, 1, 18, 15, 9, 5, 21, 19, 13, 17, 8, 20, 14, 4, 10, 16, 2, 12, 6};
    int num = sizeof(value) / sizeof(value[0]);
>>>>>>> c17d9b3508e572b1679f97fbdae25df16e6f1eae

    for(int i = 0 ; i < sizeof(insertThread) / sizeof(insertThread[0]) ; i ++){
        pthread_join(insertThread[i], NULL);
    }
    // int value[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    // int num = sizeof(value) / sizeof(value[0]);

    // for (int i = 0; i < num; i++ ) {
    //     printf("\n\n%d 가 입력됩니다.\n",value[i]);
    //     insert(value[i]);
    //     printTree();
    //     printf("\n\n");
    // }
    
    printTree();
    return 0;
}
