#include <stdio.h>
#include <stdlib.h>

#define M 5

typedef struct Node {
    int leaf;
    int count;
    int value[M];
    struct Node* child[M + 1];
    struct Node* successor;
    struct Node* predecessor;
    struct Node* parent;
} Node;

Node* split();


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

Node* find(Node* root, int value){
    while(!root->leaf){
        int flag = 0;
        for(int i = 0; i < root->count; i++){
            if(value<root->value[i]){
                root = root->child[i];
                flag = 1;
                break;
            }
        } if (flag == 0){
            root = root->child[root->count];
        }
    }
    return root;
    
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

Node* indexInsert(Node* indexNode, int value, Node* left, Node* right){
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

    if (indexNode->count==M) indexNode = split(indexNode);
    return indexNode;
}


Node* split(Node* node){

    //부모 노드
    Node* root = node->parent;
    //부모 값
    int value = node->value[M/2];
    //형제 노드
    Node* sibling = newNode();
    sibling->parent = node->parent;
    sibling->leaf = node->leaf;
    if(sibling->leaf) {
        node->successor = sibling;
        sibling->predecessor = node;
    }

    //리프 노드 분할
    if (node->leaf) {
        for (int i = M/2 ; i < M ; i ++){
            sibling->value[i-M/2] = node->value[i];
            node->value[i] = 0;
        }
    
        node->count = M/2;
        sibling->count = (M+M%2)/2;
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
            node->child[i] = NULL;
        }


    }

    //부모 노드 Insert
    if (root == NULL) {
        root = newNode();
        root->leaf = 0;
        root->value[0] = value;
        root->count++;
        root->child[0] = node;
        root->child[1] = sibling;
        node->parent = root;
        sibling->parent = root;
    } else root = indexInsert(root,value,node,sibling);

    return root;
}

Node* insert(Node* root, int value) {
    //첫 Insert
    if(root == NULL) {
        Node* tmp =  newNode();
        
        leafInsert(tmp, value);
        return tmp;

    //기본 Insert
    } else{
        Node* leafNode = find(root, value);
        leafInsert(leafNode, value);
        if(leafNode->count==M) root = split(leafNode);
        return root;
    }
}

void printTree(Node* root){
    root = find(root, 0);
    while(root != NULL){
        for(int i = 0 ; i < M ; i++){
            if(root->value[i] == 0) break;
            printf("%d ",root->value[i]);
        }
        root = root->successor;
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
    for(int i = 0; i < node->count ; i++){
        if(node->child[i]==value){
            for(int j = i ; j < node->count ; j++){
                node->child[i] = node->child[i+1];
            }
        }
    } node->count--;
}

int searchParentByLeft(Node* parent, int leafValue){
    for(int i = 0; i < parent->count; i++){
        if (parent->value[i] < leafValue){
            return parent->value[i];
        }
    }
}

int searchParentByright(Node* parent, int leafValue){
    for(int i = parent->count-1 ; i >= 0 ; i--){
        if (parent->value[i] <= leafValue){
            return parent->value[i];
        }
    }
}

void treeDelete(Node* parent, int value){
    for(int i = 0 ; i < parent->count ; i++){
        if(parent->value[i] == value){
            for(int j = i ; j < parent->count; j++){
                parent->value[i] = parent->value[i+1];
                parent->value[i+1] = parent->value[i+2];
            } break;
        } 
    }
    parent->count--;
}

Node* verification(Node* root, Node* node){
    int min = M/2+M%2-1;
    
    if (node == root) return root;
    else if (node->count < min) return recover(root, node);
    else return root;
}

Node* recover(Node* root, Node* leaf){
    int min = M/2+M%2-1;

    Node* successor = leaf->successor;
    Node* predecessor = leaf->predecessor;
    Node* parent = leaf->parent;
    
    //전임자에게 빌림
    if (predecessor->count > min){

        leafInsert(leaf,predecessor->value[predecessor->count-1]);
        IndexChange(leaf,leaf->value[1],leaf->value[0]);
        leafDelete(predecessor, predecessor->value[predecessor->count-1]);
    
    //후임자에게 빌림
    } else if (successor->count > min){

        leafInsert(leaf,successor->value[0]);
        IndexChange(successor,successor->value[0],successor->value[1]);
        leafDelete(successor, successor->value[0]);

    //부모에게 빌림
    } else{

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

    for (int i = 0; i < num; i++ ) {
        root = insert(root, value[i]);
    }

    printTree(root);

    return 0;
}
