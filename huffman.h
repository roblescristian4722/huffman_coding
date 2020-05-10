#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <bitset>
#include <cmath>
#include "hash_map.h"
#include "lsl.h"
#define ITEM std::pair<char, size_t>
#define BYTE_L 8
using namespace std;

LSL<char> byteList;
HashMap <char, string> codes;

struct TreeNode
{
    ITEM* data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const ITEM& d)
    { 
        data = new ITEM(d);
        left = nullptr;
        right = nullptr;
    }
    /// OPERATOR OVERLOADS
    bool operator < (const TreeNode& other)
    { return this->data->second < other.data->second; }

    bool operator > (const TreeNode& other)
    { return this->data->second > other.data->second; }
};

void write_bit(char &aux, int &size, bool value);
void write_byte(char &whole, char &part, int &size);
bool is_leaf(TreeNode*& node);
void parsing_pre_order(TreeNode*& node, char& aux, int& size, string code);
void clear_treeNode(TreeNode*& node);
void sort_pointer(LSL<TreeNode*> &list);
void write_file(const char *orgFile);
void compress(const char* orgFile);
void decompress(const char* orgFile);

bool is_leaf(TreeNode*& node)
{ return (node->left == nullptr && node->right == nullptr); }

void parsing_pre_order(TreeNode*& node, char& aux, int& size, string code)
{
    if (node != nullptr){
        if (is_leaf(node)){
            codes.insert(node->data->first, code);
            write_bit(aux, size, 1);
            write_byte(node->data->first, aux, size);
        }
        else
            write_bit(aux, size, 0);

        parsing_pre_order(node->left, aux, size, code + "0");
        parsing_pre_order(node->right, aux, size, code + "1");
    }
}

void write_byte(char &whole, char &part, int &size)
{
    if (size){
        char tmp = (whole >> size);
        char borrar = 0;
        part <<= BYTE_L - size;
        tmp |= part;
        byteList.push_back(tmp);
        bitset<8> bit = part;
        cout << "write_byte (" << whole << ": " << int(whole) << ") " << size << ": " << bit << endl;
        part = (whole << BYTE_L - size);
        part >>= BYTE_L - size;
        for (int i = size; i > 0; --i)
            borrar |= char(pow(2, i - 1));
        part &= borrar;
    }
    else
        byteList.push_back(whole);
}

void write_bit(char &aux, int &size, bool value)
{
    aux <<= 1;
    if (value)
        aux |= 0x1;
    else
        aux |= 0x0;
    if (size == 7){
        byteList.push_back(aux);
        size = -1;
        aux = 0;
    }
    ++size;
}

// Removes all nodes from a tree
void clear_treeNode(TreeNode*& node)
{
   if (node != nullptr){
       clear_treeNode(node->left);
       clear_treeNode(node->right);
       delete node;
   }
   node = nullptr;
}

/// Sorts a LSL that stores pointers
void sort_pointer(LSL<TreeNode*> &list)
{
    size_t n = list.size();
    TreeNode *tmp;
    int brecha = (int)n / 2;
    int j;
    while (brecha > 0){
        for (int i = brecha; i < (int)n; ++i){
            tmp = list[i];
            j = i;
            while (j >= brecha && list[j - brecha]->data->second > tmp->data->second){
                list[j] = list[j - brecha];
                j -= brecha;
            }
            list[j] = tmp;
        }
        brecha /= 2;
    }
}

void write_file(const char *orgFile)
{
    char item;
    char aux = 0;
    int size = 0;
    string code;
    fstream output("respaldo.cmp", ios::out | ios::binary);
    fstream input;

    for (size_t i = 0; i < byteList.size(); ++i)
        output.write((char*)&byteList[i], sizeof(char));
    if (byteList.back() != 0)
        output.write((char*)&aux, sizeof(char));
    
    for (size_t i = 0; i < codes.size(); ++i)
        cout << *codes.get_position(i).key << ": "
             << *codes.get_position(i).value << endl;

    input.open(orgFile, ios::in | ios::binary);
    while (!input.eof()){
        input.read((char*)&item, sizeof(item));
        if (input.eof())
            break;
        code = *codes[item];
        cout << "code: " << code << endl;
        for (size_t i = 0; i < code.size(); ++i){
            aux <<= 1;
            if (code[i] == '1')
                aux |= 1;
            if (size == BYTE_L - 1){
                output.write((char*)&aux, sizeof(char));
                size = -1;
                aux = 0;
            }
            ++size;
        }
    }
    if (size)
        output.write((char*)&aux, sizeof(char));
    
    input.close();
    output.close();
}

bool read_bit(fstream &stream, char &byte, int &size)
{
    char aux;
    if (!size)
        stream.read((char*)&byte, sizeof(char));
    aux = char(pow(2, BYTE_L - size - 1));
    ++size;
    size %= BYTE_L;
    return byte & aux;
}

char read_byte(fstream &stream, char &byte, int &size)
{
    char aux;
    char borrar;

    aux = byte << size;
    stream.read((char*)&byte, sizeof(char));
    if (size){
        borrar = byte >> BYTE_L - size;
        if ((unsigned char)borrar >= 0x80){
            for (int i = BYTE_L - 1; i >= size; --i){
                borrar ^= char(pow(2, i));
            }
        }
        aux |= borrar;
    }
    else
        aux = byte;
    return aux;
}

TreeNode* read_node(TreeNode*& node, fstream& stream, char& byte, int& size)
{
    if (read_bit(stream, byte, size)){
        char aux = read_byte(stream, byte, size);
        cout << aux << "(" << int(aux) << ")" << endl;
        return new TreeNode({aux, 0});
    }
    else{
        cout << "*" << endl;
        node = new TreeNode({});
        node->left = read_node(node, stream, byte, size);
        node->right = read_node(node, stream, byte, size);
    }
    return node;
}

/// Compresses a given file using Huffman algorithm
void compress(const char* orgFile)
{
    HashMap<char, size_t> items;
    LSL<TreeNode*> itemList;
    TreeNode *auxItem;
    TreeNode *root;
    fstream file(orgFile, ios::in);
    char aux;
    char byte = 0;
    int byteSize = 0;
    long pos = 0;
    char code = 0;

    // If file doesn't exist we throw an error
    if (!file.is_open())
        throw range_error("Origin file not found");
    // Otherwise, we read every character on the file and
    // store them in a hash mapa along with a counter
    // of times it gets repeated
    while (!file.eof()){
        file.read((char*)&aux, sizeof(char));
        if (file.eof())
            break;

        if (items[aux] == nullptr)
            items.insert(aux, 1);
        else
            ++(*items[aux]);
    }
    file.close();
    // Then, each value from the hash map gets stored in a list
    for (size_t i = 0; i < items.size(); ++i){
        auxItem = new TreeNode({*items.get_position(i).key, *items.get_position(i).value});
        itemList.push_back(auxItem);
    }
    items.clear();
    sort_pointer(itemList);
    // Here we make the tree using pairs from the list
    while (1){
        size_t itemValue = itemList[0]->data->second + itemList[1]->data->second;
        root = new TreeNode({'\0', itemValue});
        root->left = itemList[0];
        root->right = itemList[1];
        itemList.pop_front();
        itemList.pop_front();
        if (!itemList.size())
            break;
        itemList.push_back(root);
        sort_pointer(itemList);
    }
    // Then turn the tree into binary code so it can get stored
    // in a file using write_file and also we get the code for each character
    // and store it in a hash map
    parsing_pre_order(root, byte, byteSize, "");
    aux = 0;
    write_byte(aux, byte, byteSize);
    if (byteSize)
        byteList.push_back(byte);

    write_file(orgFile);

    clear_treeNode(root);
}
/// Decompresses a given file using Huffman algorithm
void decompress(const char *orgFile)
{
    TreeNode *root;
    fstream input(orgFile, ios::in | ios::binary);
    int size = 0;
    char byte;
    char bit;
    char data = 0;
    bool tree = true;

    // If file doesn't exist we throw an error
    if (!input.is_open())
        throw range_error("Origin file not found");

    read_node(root, input, byte, size);

    input.close();
    clear_treeNode(root);
}

#endif // HUFFMAN_H