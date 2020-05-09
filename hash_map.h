#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <iostream>
#include <sstream>
#include "lsl.h"
#define PRIMO 211
using namespace std;

//// EXTRA METHOD DE CLARATION ////
template <typename T>
long binary_search(LSL<T> &list, T data);

//// CLASS PROTOTYPES ////
template<typename K, typename V>
class Node;
template <typename K, typename V>
class Bucket;
template <typename K, typename V>
class HashMap;

//////////////
//// NODE ////
//////////////
template <typename K, typename V>
class Node
{
private:
    K key;
    V value;

public:
    Node() {}
    Node(K _key, V _val) : key(_key), value(_val) {}
    ~Node() {}

    bool operator>(Node<K, V> &other)
    { return this->key > other.key; }

    bool operator==(Node<K, V> &other)
    { return this->key == other.key; }

    bool operator<(Node<K, V> &other)
    { return this->key < other.key; }

    friend class Bucket<K,V>;
    friend class HashMap<K, V>;
};

////////////////
//// BUCKET ////
////////////////
template <typename K, typename V>
class Bucket
{
private:
    LSL<Node<K, V>> m_nodes;
    bool m_visited;
public:
    Bucket()
    { m_visited = false; }
    ~Bucket()
    { m_nodes.clear(); }

    size_t bucket_size()
    { return m_nodes.size(); }

    bool operator>(Bucket<K, V> &other)
    { return this->m_hashValue > other.m_hashValue; }

    bool operator<(Bucket<K, V> &other)
    { return this->m_hashValue < other.m_hashValue; }

    bool operator==(Bucket<K, V> &other)
    { return this->m_hashValue == other.m_hashValue; }

    friend class HashMap<K, V>;
};
//////////////////
//// HASH MAP ////
//////////////////
template <typename K, typename V>
class HashMap
{
private:
    Bucket<K, V>* m_buckets;
    size_t m_size;

public:
    HashMap();
    ~HashMap();

    struct Pair
    {
        K *key;
        V *value;
    };

    bool empty();
    bool colide(size_t hash);
    size_t size();

    void insert(const K &keyVal, const V &val);
    void delete_value(const K &keyVal);
    void clear();
    V *operator[](K index);
    size_t hash_function(K val);
    HashMap::Pair get_position(size_t index);
};

//////////////////////
//// HASH MAP     ////
//// DEFINITIONS  ////
//////////////////////

template <typename K, typename V>
HashMap<K, V>::HashMap()
{
    m_size = 0;
    m_buckets = new Bucket<K,V>[PRIMO];
}

template<typename K, typename V>
HashMap<K, V>::~HashMap()
{ clear(); }

/// Returns true if empty
/// and false otherwise
template <typename K, typename V>
bool HashMap<K, V>::empty()
{ return m_size; }

/// indicates whether the hash value returned
/// by hash_function already exists and if it does this
/// method gives you it's exact position
template <typename K, typename V>
bool HashMap<K, V>::colide(size_t hash)
{
    if (empty())
        return false;
    if (!m_buckets[hash].m_visited)
        return false;
    return true;
}

/// Returns the total size of the hash map (i.e. the
/// summation of the size of every bucket in buckets list)
template <typename K, typename V>
size_t HashMap<K, V>::size()
{
    size_t sizeTotal = 0;
    for (size_t i = 0; i < PRIMO; ++i)
        if (m_buckets[i].m_visited)
            sizeTotal += m_buckets[i].bucket_size();
    return sizeTotal;
}

/// Inserts a new node in the hash map
template <typename K, typename V>
void HashMap<K, V>::insert(const K &keyVal, const V &val)
{
    Node<K, V> nodeTmp(keyVal, val);
    size_t hash;

    // Hash value is calculated
    hash = hash_function(keyVal);
    // The node is inserted in it's corresponding position
    // using the previously calculated hash value
    m_buckets[hash].m_nodes.push_back(nodeTmp);
    // The node list gets sorted
    sort(m_buckets[hash].m_nodes);
    m_buckets[hash].m_visited = true;
    // The size counter increases by one
    ++m_size;
}

/// Deletes an existing value in the hash map
template <typename K, typename V>
void HashMap<K, V>::delete_value(const K &keyVal)
{
    size_t i;
    LSL<Node<K, V>> *auxList;
    long pos = hash_function(keyVal);

    if (m_buckets[pos].m_visited){
        auxList = &m_buckets[pos].m_nodes;
        for (i = 0; i < auxList->size(); ++i)
            if ((*auxList)[i].key == keyVal) {
                auxList->erase(i);
                // The size counter decreases
                --m_size;
                break;
            }
        // If the node list is empty the "visited" flag
        // turns into false again
        if (auxList->empty())
            m_buckets[pos].m_visited = false;
    }
    else
        cout << "There is not such value" << endl;
}

/// Removes every value stored in the hash map
template <typename K, typename V>
void HashMap<K, V>::clear()
{
    for (long i = 0; i < PRIMO; ++i){
        m_buckets[i].m_nodes.clear();
        m_buckets[i].m_visited = false;
    }
    m_size = 0;
}

/// It gets the key's corresponding hash
/// value using the PJW method
template <typename K, typename V>
size_t HashMap<K, V>::hash_function(K val)
{
    // The input turns into a stringstream value
    // and then turns into a string due to
    // possible numeric keys
    stringstream ss;
    char pos;
    size_t hash = 0;
    size_t tmp;

    ss << val;
    string auxStr = ss.str();
    for (long i = 0; i < auxStr.size(); ++i){
        pos = auxStr[0];
        hash = (hash << 4) + pos;
        tmp = hash & 0xF0000000;
        if (tmp > 1){
            hash = hash ^ (tmp << 24);
            hash = hash ^ tmp;
        }
    }
    return (hash % PRIMO);
}

/// Helps to iterate over the hash map
/// using numeric positions like those in lists
template<typename K, typename V>
typename HashMap<K,V>::Pair HashMap<K,V>::get_position(size_t index)
{
    LSL<Node<K,V>> *node;
    HashMap<K,V>::Pair par;
    par.key = nullptr;
    par.value = nullptr;
    size_t indexTmp = index;
    size_t j;

    for (size_t i = 0; i < PRIMO && indexTmp >= 0 && index < size(); ++i){
        if (m_buckets[i].m_visited){
            node = &m_buckets[i].m_nodes;
            for (j = 0; j < node->size() && indexTmp > 0; ++j)
                --indexTmp;
            if (!indexTmp && j != node->size()){
                par.key = &(*node)[j].key;
                par.value = &(*node)[j].value;
                return par;
            }
        }
    }
    cout << "Error: index is greater than the hash size" << endl;
    return par;
}

/// Returns a value stored in the hash map
/// using a key as the way to find it
template <typename K, typename V>
V *HashMap<K, V>::operator[](K index)
{
    LSL<Node<K, V>> *auxList;
    Node<K, V> nodeTmp;
    long posNode;
    size_t hashValue = hash_function(index);
    V *v = nullptr;

    nodeTmp.key = index;
    if (m_buckets[hashValue].m_visited) {
        auxList = &m_buckets[hashValue].m_nodes;
        posNode = binary_search(*auxList, nodeTmp);
        if (posNode != -1)
            v = &(*auxList)[posNode].value;
    }
    return v;
}

template <typename T>
long binary_search(LSL<T> &list, T data)
{
    int l = 0;
    int r = int(list.size() - 1);
    while (l <= r)
    {
        int m = (l + r) / 2;
        if (data == list[m])
            return m;
        else if (data < list[m])
            r = m - 1;
        else
            l = m + 1;
    }
    return -1;
}

#endif //HASHMAP_H
