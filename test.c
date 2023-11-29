#include <stdio.h>
#include <stdlib.h>

#define M 5

typedef struct BPlusTreeNode {
    int is_leaf;
    int num_keys;
    int keys[M];
    struct BPlusTreeNode* children[M+1];
} BPlusTreeNode;

BPlusTreeNode* create_node() {
    BPlusTreeNode* node = (BPlusTreeNode*)malloc(sizeof(BPlusTreeNode));
    node->is_leaf = 1;
    node->num_keys = 0;
    for (int i = 0; i <= M; ++i) {
        node->children[i] = NULL;
    }
    return node;
}

BPlusTreeNode* insert_into_leaf(BPlusTreeNode* leaf, int key) {
    int i = leaf->num_keys - 1;
    while (i >= 0 && key < leaf->keys[i]) {
        leaf->keys[i + 1] = leaf->keys[i];
        i--;
    }
    leaf->keys[i + 1] = key;
    leaf->num_keys++;
    return leaf;
}

BPlusTreeNode* split_leaf(BPlusTreeNode* root, BPlusTreeNode* leaf) {
    BPlusTreeNode* new_leaf = create_node();
    int split_index = (M + 1) / 2;

    for (int i = split_index; i < leaf->num_keys; ++i) {
        new_leaf->keys[i - split_index] = leaf->keys[i];
        new_leaf->num_keys++;
        leaf->num_keys--;
    }

    new_leaf->children[M] = leaf->children[M];
    leaf->children[M] = new_leaf;

    return new_leaf;
}

BPlusTreeNode* insert_into_internal(BPlusTreeNode* root, int key) {
    int i = root->num_keys - 1;

    while (i >= 0 && key < root->keys[i]) {
        i--;
    }

    i++;

    BPlusTreeNode* child = root->children[i];
    BPlusTreeNode* new_child = insert_into_bplus_tree(child, key);

    if (new_child != child) {
        while (i < root->num_keys && key < root->keys[i]) {
            i++;
        }

        int j = root->num_keys - 1;
        while (j >= i) {
            root->keys[j + 1] = root->keys[j];
            root->children[j + 2] = root->children[j + 1];
            j--;
        }

        root->keys[i] = new_child->keys[0];
        root->children[i + 1] = new_child;
        root->num_keys++;

        if (root->num_keys > M) {
            BPlusTreeNode* new_internal = split_internal(root);
            return new_internal;
        }
    }

    return root;
}

BPlusTreeNode* split_internal(BPlusTreeNode* root) {
    BPlusTreeNode* new_internal = create_node();
    int split_index = (M + 1) / 2;

    for (int i = split_index; i < root->num_keys; ++i) {
        new_internal->keys[i - split_index] = root->keys[i];
        new_internal->children[i - split_index] = root->children[i];
        new_internal->num_keys++;
        root->num_keys--;
    }

    new_internal->children[new_internal->num_keys] = root->children[root->num_keys];

    return new_internal;
}

BPlusTreeNode* insert_into_bplus_tree(BPlusTreeNode* root, int key) {
    if (!root) {
        root = create_node();
        insert_into_leaf(root, key);
        return root;
    }

    if (root->is_leaf) {
        if (root->num_keys < M) {
            insert_into_leaf(root, key);
        } else {
            BPlusTreeNode* new_leaf = split_leaf(root, root);
            root = insert_into_bplus_tree(root, key);
        }
    } else {
        root = insert_into_internal(root, key);
    }

    return root;
}

void print_bplus_tree(BPlusTreeNode* root) {
    if (root) {
        for (int i = 0; i < root->num_keys; ++i) {
            printf("%d ", root->keys[i]);
        }
        printf("\n");
        if (!root->is_leaf) {
            for (int i = 0; i <= root->num_keys; ++i) {
                print_bplus_tree(root->children[i]);
            }
        }
    }
}

int main() {
    BPlusTreeNode* root = NULL;

    int keys[] = {3, 7, 1, 5, 8, 4, 2, 6};
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < num_keys; ++i) {
        root = insert_into_bplus_tree(root, keys[i]);
    }

    print_bplus_tree(root);

    return 0;
}