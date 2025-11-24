#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 1009

typedef struct Node {
    int key;
    char* value;

    struct Node* prev;
    struct Node* next;
} Node;

typedef struct HashNode {
    int key;
    Node* node;
    struct HashNode* next;
} HashNode;

typedef struct LRUCache {
    int capacity;
    int size;

    Node* head;
    Node* tail;

    HashNode* hashMap[HASH_SIZE];
} LRUCache;

int hashFunction(int key) {
    return key % HASH_SIZE;
}

LRUCache* createCache(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));

    if(!cache) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;

    for(int index = 0; index < HASH_SIZE; index++) {
        cache->hashMap[index] = NULL;
    }

    return cache;
}

void hashPut(LRUCache* cache, int key, Node* node) {
    int hashIndex = hashFunction(key);
    HashNode* bucket = cache->hashMap[hashIndex];

    while(bucket != NULL) {
        if(bucket->key == key) {
            bucket->node = node;
            return;
        }
        bucket = bucket->next;
    }

    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->key = key;
    newNode->node = node;
    newNode->next = cache->hashMap[hashIndex];
    cache->hashMap[hashIndex] = newNode;
}

Node* hashGet(LRUCache* cache, int key) {
    int hashIndex = hashFunction(key);
    HashNode* bucket = cache->hashMap[hashIndex];

    while(bucket != NULL) {
        if(bucket->key == key) {
            return bucket->node;
        }
        bucket = bucket->next;
    }
    return NULL;
}

void hashRemove(LRUCache* cache, int key) {
    int hashIndex = hashFunction(key);
    HashNode* bucket = cache->hashMap[hashIndex];
    HashNode* prev = NULL;

    while(bucket != NULL) {
        if(bucket->key == key) {
            if(prev) {
                prev->next = bucket->next;
            } else {
                cache->hashMap[hashIndex] = bucket->next;
            }

            free(bucket);
            return;
        }
        prev = bucket;
        bucket = bucket->next;
    }
}

void moveToFront(LRUCache *cache, Node *node) {
    if (node == cache->head)
        return;

    if (node->prev)
        node->prev->next = node->next;
    else
        cache->head = node->next; 

    if (node->next)
        node->next->prev = node->prev;
    else
        cache->tail = node->prev; 

    node->prev = NULL;
    node->next = cache->head;

    if (cache->head)
        cache->head->prev = node;

    cache->head = node;

    if (cache->tail == NULL)
        cache->tail = node;
}

void removeLRU(LRUCache* cache) {
    if(cache->tail == NULL) {
        return;
    }

    Node* removeNode = cache->tail;
    hashRemove(cache, removeNode->key);

    if(removeNode->prev) {
        removeNode->prev->next = NULL;
    } else {
        cache->head = NULL;
    }

    cache->tail = removeNode->prev;

    free(removeNode->value);
    free(removeNode);

    cache->size--;
}

void insertAtFront(LRUCache* cache, int key, const char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = strdup(value);
    newNode->next = cache->head;
    newNode->prev = NULL;

    if(cache->head) {
        cache->head->prev = newNode;
    }

    cache->head = newNode;

    if(cache->tail == NULL) {
        cache->tail = newNode;
    }

    cache->size++;

    hashPut(cache, key, newNode);
}

char* get(LRUCache* cache, int key) {
    Node* node = hashGet(cache, key);

    if(!node) {
        return NULL;
    }

    moveToFront(cache, node);
    return node->value;
}

void put(LRUCache* cache, int key, const char* value) {
    Node* node = hashGet(cache, key);

    if(node != NULL) {
        free(node->value);
        node->value = strdup(value);
        moveToFront(cache, node);
        return;
    }

    if(cache->size >= cache->capacity) {
        removeLRU(cache);
    }

    insertAtFront(cache, key, value);
}

void freeCache(LRUCache* cache) {
    if(!cache) {
        return;
    }

    Node* current = cache->head;
    while(current) {
        Node* next = current->next;
        free(current->value);
        free(current);
        current = next;
    }

    for(int index = 0; index < HASH_SIZE; index++) {
        HashNode* bucket = cache->hashMap[index];

        while(bucket) {
            HashNode* next = bucket->next;
            free(bucket);
            bucket = next;
        }
    }

    free(cache);
}

int main() {
    char command[50];
    LRUCache* cache = NULL;

    while(1) {
        scanf("%s", command);
        
        if(strcmp(command, "createCache") == 0) {
            int size;
            scanf("%d", &size);
            cache = createCache(size);
        } else if(strcmp(command, "put") == 0) {
            int key;
            char value[100];
            scanf("%d %s", &key, value);
            put(cache, key, value);
        } else if(strcmp(command, "get") == 0) {
            int key;
            scanf("%d", &key);
            char* result = get(cache, key);
            if(result) {
                printf("%s\n", result);
            } else {
                printf("NULL\n");
            }
        } else if(strcmp(command, "exit") == 0) {
            freeCache(cache);
            break;
        }
    }

    return 0;
}