// Dictionary.cpp



#include "Dictionary.h"
#include <string>
#include <list>
#include <cctype>
#include <utility>  // for swap

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

void removeNonLetters(string& s);


struct Node {
    string sortedKey; // sorted key word, so that all permutations fall under the same sorted key
    vector<string> permutations; // vector of all permutations that are defined in the dictionary
};

// helper function to find prime number (will be used in the constructor)
bool isPrime(int number) {
    if (number < 2) {
        return false;
    }
    for (int i = 2; i*i <= number; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

class DictionaryImpl
{
  public:
    DictionaryImpl(int maxBuckets) {
        
        // find the greatest prime less than or equal to the input to decrease amount of collisions
        m_maxBuckets = maxBuckets;
        for (int i = maxBuckets; i >= 2; i--) {
            if (isPrime(i)) {
                m_maxBuckets = i;
                break;
            }
        }
        
        // initialize all buckets to nullptrs
        for (int i = 0; i < m_maxBuckets; i++) {
            m_hashTable.push_back(nullptr);
        }
        
    }
    ~DictionaryImpl() {
        for (int i = 0; i < m_hashTable.size(); i++) {
            vector<Node*>* bucket = m_hashTable[i];
            if (bucket == nullptr) {
                continue;
            }
            for (int j = 0; j < (*bucket).size(); j++) {
                Node* node = (*bucket)[j];
                delete node;
            }
            delete bucket;
        }
    }
    void insert(string word);
    void lookup(string letters, void callback(string)) const;
  private:
    // my hash table will be a vector of vectors of node pointers
    vector<vector<Node*>*> m_hashTable;
    int m_maxBuckets = 0;
};



void DictionaryImpl::insert(string word)
{
    
    removeNonLetters(word);
    
    // sort the word so that the index for different permutations will always be the same
    string sorted = word;
    sort(sorted.begin(), sorted.end());
    
    // apply hash function and modulo operator
    unsigned int x = (unsigned int)hash<string>()(sorted);
    int index = x % m_maxBuckets;
    
    // if there isn't already a vector at that bucket, make a new one
    if (m_hashTable[index] == nullptr) {
        vector<Node*>* newVector = new vector<Node*>;
        m_hashTable[index] = newVector;
    }
    
    // check to see if our sorted input matches with any of the keys of existing nodes. if it does, append the original permutation to the vector of permutations
    vector<Node*>* vector = m_hashTable[index];
    for (int i = 0; i < vector->size(); i++) {
        Node *n = (*vector)[i];
        if (sorted == n->sortedKey) {
            n->permutations.push_back(word);
            return;
        }
    }
    
    // if the sorted input does not match any of the existing keys, make a new node and set the key to the sorted input and add the original permutation to the vector
    Node* newNode = new Node;
    newNode->sortedKey = sorted;
    newNode->permutations.push_back(word);
    (*vector).push_back(newNode);
  
}

void DictionaryImpl::lookup(string letters, void callback(string)) const
{
    
    if (callback == nullptr)
        return;

    removeNonLetters(letters);
    if (letters.empty())
        return;

    // sort the letters so that the key will be the same across permutations
    string sorted = letters;
    sort(sorted.begin(), sorted.end());
    
    // apply the hash function and modulo operator
    unsigned int x = (unsigned int)hash<string>()(sorted);
    int index = x % m_maxBuckets;
    
    // if there is no bucket at this index, return
    if (m_hashTable[index] == nullptr) {
        return;
    }
    
    // for every node in the vector, if the sorted word matches the key, then call callback on all elements in the permutations vector
    vector<Node*>* vector = m_hashTable[index];
    for (int i = 0; i < vector->size(); i++) {
        Node *n = (*vector)[i];
        if (sorted == n->sortedKey) {
            for (int j = 0; j < n->permutations.size(); j++) {
                callback(n->permutations[j]);
            }
            return;
        }
    }
    
    // if we could not find a sorted matching key, return
    return;
    
}

void removeNonLetters(string& s)
{
    string::iterator to = s.begin();
    for (string::const_iterator from = s.begin(); from != s.end(); from++)
    {
        if (isalpha(*from))
        {
            *to = tolower(*from);
            to++;
        }
    }
    s.erase(to, s.end());  // chop everything off from "to" to end.
}


Dictionary::Dictionary(int maxBuckets)
{
    m_impl = new DictionaryImpl(maxBuckets);
}

Dictionary::~Dictionary()
{
    delete m_impl;
}

void Dictionary::insert(string word)
{
    m_impl->insert(word);
}

void Dictionary::lookup(string letters, void callback(string)) const
{
    m_impl->lookup(letters,callback);
}
