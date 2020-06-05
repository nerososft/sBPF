#include "../../include/vm/hashmap.h"
#include <malloc.h>


HashMapIterator_t createHashMapIterator(HashMap_t hashMap){
    HashMapIterator_t iterator = (HashMapIterator_t) malloc(sizeof(HashMapIterator));
    iterator->count = 0;
    iterator->hashCode = -1;
    iterator->entry = NULL;
    iterator->hashMap = hashMap;
    return iterator;
}

bool hasNextHashMapIterator(HashMapIterator_t iterator){
    return iterator->count < iterator->hashMap->size ? true : false;
}

HashMapIterator_t nextHashMapIterator(HashMapIterator_t iterator){
    if (hasNextHashMapIterator(iterator)) {
        if (iterator->entry != NULL && iterator->entry->next != NULL) {
            iterator->count++;
            iterator->entry = iterator->entry->next;
            return iterator;
        }
        while (++iterator->hashCode < iterator->hashMap->listSize) {
            Entry_t entry = &iterator->hashMap->list[iterator->hashCode];
            if (entry->key != NULL) {
                iterator->count++;
                iterator->entry = entry;
                break;
            }
        }
    }
    return iterator;
}

void freeHashMapIterator(HashMapIterator_t *iterator){
    free(*iterator);
    *iterator = NULL;
}

HashMap_t createHashMap(HashCodeFunction_t hashCode, EqualFunction_t equal){
    HashMap_t hashMap = (HashMap_t) malloc(sizeof(HashMap));
    hashMap->hashCodeFunction = hashCode;
    hashMap->equalFunc = equal;
    hashMap->size = 0;
    hashMap->listSize = 8;
    hashMap->hashCodeFunction = hashCode == NULL ? defaultHashCode:hashCode;
    hashMap->equalFunc = equal == NULL ? defaultEqual:equal;
    hashMap->putFunc=defaultPut;
    hashMap->getFunc=defaultGet;
    hashMap->removeFunc=defaultRemove;
    hashMap->clearFunc=defaultClear;
    hashMap->existsFunc=defaultExists;
    hashMap->autoAssign=true;

    hashMap->list = (Entry_t)malloc(hashMap->listSize*sizeof(Entry));
    for(int i =0 ;i<hashMap->listSize;i++){
        hashMap->list[i].key = hashMap->list[i].value = hashMap->list[i].next = NULL;
    }
    return hashMap;
}

void resetHashMap(HashMap_t hashMap,int listSize){
    if(listSize < 8){
        return;
    }
    // 1. create temp list to store the data of hashMap
    Entry_t tempList = (Entry_t)malloc(sizeof(Entry)*hashMap->size);
    int size = hashMap->size;

    HashMapIterator_t iterator = createHashMapIterator(hashMap);

    for(int i = 0;hasNextHashMapIterator(iterator);i++){
        iterator = nextHashMapIterator(iterator);
        tempList[i].key = iterator->entry->key;
        tempList[i].value = iterator->entry->value;
        tempList[i].next = NULL;
    }
    free(iterator);


    // 2. clear the hashMap
    hashMap->size = 0;
    for(int i = 0; i<hashMap->listSize; i++){
        Entry_t first = &hashMap->list[i]; 
        first->key = NULL;
        first->value = NULL;
        if(first->next!=NULL){
            while(first->next!=NULL){
                Entry_t temp = first->next->next;
                free(first->next);
                first->next = temp;
            }
        }
    }

    // 3. create new hashMap and store the temp data 
    hashMap->listSize = listSize;
    Entry_t newList = (Entry_t) malloc(sizeof(Entry)*listSize);
    if(newList!=NULL){
        hashMap->list = newList;
        newList = NULL;
    }


    for(int i = 0;i<hashMap->listSize;i++){
        hashMap->list[i].key = NULL;
        hashMap->list[i].value = NULL;
        hashMap->list[i].next = NULL;
    }

    for(int i = 0;i<size;i++){
        hashMap->putFunc(hashMap, tempList[i].key,tempList[i].value);
    }
    free(tempList);
}

int defaultHashCode(HashMap_t hashMap, void *key){
    long uKey = *(long *)key;
    if(uKey < 0){
        return -uKey % hashMap->listSize;
    }
    return uKey % hashMap->listSize;
}

bool defaultEqual(void *key1,void *key2){
    return key1 == key2;
}

void defaultPut(HashMap_t hashMap,void *key,void *value){

    if(hashMap->autoAssign && hashMap->size >= hashMap->listSize){
        resetHashMap(hashMap, hashMap->listSize*2);
    }

    int index = hashMap->hashCodeFunction(hashMap, key);
    if(hashMap->list[index].key==NULL){
        hashMap->size++;
        hashMap->list[index].key = key;
        hashMap->list[index].value = value;
    }else{
        Entry_t first = &hashMap->list[index];
        while(first!=NULL){
            if(hashMap->equalFunc(first->key,key)){
                first->value = value;
                return;
            }
            first = first->next;
        }
        Entry_t entry = (Entry_t)malloc(sizeof(Entry));
        entry->key = key;
        entry->value = value;
        entry->next =  hashMap->list[index].next;
        hashMap->list[index].next = entry;
        hashMap->size++;
    }
}

void* defaultGet(HashMap_t hashMap,void *key){
    int index = hashMap->hashCodeFunction(hashMap, key);
    Entry_t entry = &hashMap->list[index];
    while (entry->key != NULL && !hashMap->equalFunc(entry->key, key)) {
        entry = entry->next;
    }
    return entry->value;
}

bool defaultRemove(HashMap_t hashMap, void *key){

}

bool defaultExists(HashMap_t hashMap, void *key){

}

void defaultClear(HashMap_t hashMap){

}