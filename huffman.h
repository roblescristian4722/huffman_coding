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

/* ESSENTIAL GLOBAL VARIABLES */
LSL<char> byteList;
HashMap <char, string> codes;
HashMap <string, char> codesOut;
long posLen;

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

/* METHOD DECLARATIONS */
/* TREE METHODS */
bool is_leaf(TreeNode*& node);
void preorder_parsing_write(TreeNode*& node, char& aux, int& size, string code);
void clear_treeNode(TreeNode*& node);
/* WRITING METHODS */
void write_bit(char &aux, int &size, bool value);
void write_byte(char &whole, char &part, int &size);
void write_file(const char *orgFile, const char *dstFile);
/* READING METHODS */
bool read_bit(fstream &stream, char &byte, int &size);
char read_byte(fstream &stream, char &byte, int &size);
TreeNode* read_node(TreeNode*& node, fstream& stream, char& byte, int& size, string code);
/* SORTING (LSL) */
void sort_pointer(LSL<TreeNode*> &list);
/* COMPRESSING / DECOMPRESSING */
void compress(const char* orgFile, const char* dstFile);
void decompress(const char *orgFile, const char *dstFile);

/* METHOD DEFINITIONS */
/* TREE METHODS */
/// Returns true if the given node is has no children
bool is_leaf(TreeNode*& node)
{ return (node->left == nullptr && node->right == nullptr); }

///
void preorder_parsing_write(TreeNode*& node, char& aux, int& size, string code)
{
    if (node != nullptr){
        if (is_leaf(node)){
            codes.insert(node->data->first, code);
            write_bit(aux, size, 1);
            write_byte(node->data->first, aux, size);
        }
        else
            write_bit(aux, size, 0);

        preorder_parsing_write(node->left, aux, size, code + "0");
        preorder_parsing_write(node->right, aux, size, code + "1");
    }
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

/* WRITING METHODS  */
/// Stores a whole byte in the data list that will be written in a file
void write_byte(char &whole, char &part, int &size)
{
    if (size){
        char tmp = (whole >> size);
        char erase = 0;
        int shift = BYTE_L - size;

        if ((unsigned char)tmp >= 0x80){
            for (int i = BYTE_L - 1; i >= shift; --i)
                tmp ^= char(pow(2, i));
        }
        part <<= shift;
        if (part & 0x1 == 0x1){
            for (int i = 0; i < shift; ++i)
                part ^= char(pow(2, i));
        }
        tmp |= part;
        byteList.push_back(tmp);
        part = (whole << shift);
        part >>= shift;
        for (int i = size; i > 0; --i)
            erase |= char(pow(2, i - 1));
        part &= erase;
    }
    else
        byteList.push_back(whole);
}

/// Writes a bit in an auxiliar char variable which will be stored in the data
/// list once it gets filled with all 8 bits.
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

/* SORTING (LSL) */
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

/// Writes the data from the list to a given file and then reads the data from
/// the origin file so they can be translated and then written to the destiny file
/// using a hash map.
void write_file(const char *orgFile, const char *dstFile)
{
    char item;
    char aux = 0;
    int size = 0;
    unsigned int len;
    string code;
    fstream output(dstFile, ios::out | ios::binary);
    fstream input;

    // The data list (containing the compressed binary tree)
    // gets written in a given file
    for (size_t i = 0; i < byteList.size(); ++i){
        if (i != byteList.size() - 1 && byteList[i] != 0)
            output.write((char*)&byteList[i], sizeof(char));
    }
    // If aux still has some data left then it gets a shift left and
    // then gets written into the file
    if (size){
        aux <<= BYTE_L - size;
        output.write((char*)&aux, sizeof(char));
    }
    aux = 0;
    // We write an unsigned int into the file and store it's file position
    // so we can modify it later, it will store the size of characters to be read.
    posLen = output.tellp();
    output.write((char*)&len, sizeof(len));
    // Every code gets printed along with the value it represents
    for (size_t i = 0; i < codes.size(); ++i)
        cout << *codes.get_position(i).key << ": "
             << *codes.get_position(i).value << endl;
    // We read every character from the origin file and translate it
    input.open(orgFile, ios::in | ios::binary);
    while (!input.eof()){
        input.read((char*)&item, sizeof(item));
        if (input.eof())
            break;
        code = *codes[item];
        // We store every bit on an aux variable, when it gets filled with
        // 8 characters it gets written into the destination file
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
        ++len;
    }
    // If there are bits left in the aux variable we apply a shift
    // left and we write it into the destination file.
    if (size){
        aux <<= BYTE_L - size;
        output.write((char*)&aux, sizeof(char));
    }
    output.seekp(posLen);
    output.write((char*)&len, sizeof(len));
    input.close();
    output.close();
}

/* READING METHODS */
/// Reads a bit from the origin file and returns true if the given bit
/// is 1, otherwise returns 0
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

/// reads a whole byte from the origin file
char read_byte(fstream &stream, char &byte, int &size)
{
    char aux;
    char erase;

    aux = byte << size;
    stream.read((char*)&byte, sizeof(char));
    if (size){
        erase = byte >> BYTE_L - size;
        if ((unsigned char)erase >= 0x80){
            for (int i = BYTE_L - 1; i >= size; --i){
                erase ^= char(pow(2, i));
            }
        }
        aux |= erase;
    }
    else
        aux = byte;
    return aux;
}

/// Reads the "header" from the origin file so it can recreate the compressed binary tree
/// using a preorder parsing.
TreeNode* read_node(TreeNode*& node, fstream& stream, char& byte, int& size, string code)
{
    if (read_bit(stream, byte, size)){
        char aux = read_byte(stream, byte, size);
        codesOut.insert(code, aux);
        return new TreeNode({aux, 0});
    }
    else{
        node = new TreeNode({});
        node->left = read_node(node, stream, byte, size, code + "0");
        node->right = read_node(node, stream, byte, size, code + "1");
    }
    return node;
}

/* COMPRESSING / DECOMPRESSING (PUBLIC METHODS) */
/// Compresses a given file using the Huffman algorithm
void compress(const char *orgFile, const char *dstFile)
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
    preorder_parsing_write(root, byte, byteSize, "");
    aux = 0;
    write_byte(aux, byte, byteSize);
    if (byteSize)
        byteList.push_back(byte);

    write_file(orgFile, dstFile);
    byteList.clear();
    codes.clear();
    clear_treeNode(root);

}
/// Decompresses a given file using the Huffman algorithm
void decompress(const char *orgFile, const char *dstFile)
{
    TreeNode *root;
    fstream input(orgFile, ios::in | ios::binary);
    ofstream output;
    string str;
    int size = 0;
    unsigned int len;
    char byte;
    char bit;
    char data = 0;
    bool tree = true;

    // If file doesn't exist we throw an error
    if (!input.is_open())
        throw range_error("Origin file not found");

    // We recreate the compressed file and fill a hash map with
    // every character available along with it's translation
    read_node(root, input, byte, size, "");
    size = 0;
    input.read((char*)&len, sizeof(len));
    cout << endl << "characters: " << len << endl;
    output.open(dstFile, ios::binary);
    // Then we read every byte from the origin file and translate it
    // using a hash map that stores every character translation
    while (!input.eof() && len){
        string aux = read_bit(input, byte, size) ? "1" : "0";
        if (input.eof())
            break;
        str += aux;
        char *res = codesOut[str];
        if (res != nullptr){
            --len;
            str = "";
            output.write((char*)res, sizeof(char));
        }
    }

    input.close();
    output.close();
    codesOut.clear();
    clear_treeNode(root);
}

#endif // HUFFMAN_H