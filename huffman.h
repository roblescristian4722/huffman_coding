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
HashMap <char, char> codes;

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
bool is_leaf(TreeNode*& node);
void parsing_in_order(TreeNode*& node);
void parsing_pre_order(TreeNode*& node, char& aux, int& size,
                       int code);
void write_byte(char &whole, char& part, int &size);
void write_bit(char &aux, int &size, bool value);
void clear_treeNode(TreeNode*& node);
void sort_pointer(LSL<TreeNode*> &list);
void compress(const char* orgFile);
void decompress(const char* orgFile);

bool is_leaf(TreeNode*& node)
{ return (node->left == nullptr && node->right == nullptr); }

void parsing_in_order(TreeNode*& node)
{
    if (node != nullptr){
        parsing_in_order(node->left);
        cout << node->data->first << " (" << int(node->data->first) << ") " << node->data->second;
        if (is_leaf(node))
            cout << " (HOJA) ";
        parsing_in_order(node->right);
    }
}

void parsing_pre_order(TreeNode*& node, char& aux, int& size,
                       int code)
{
    bitset<8> bit;
    if (node != nullptr){
        if (is_leaf(node)){
            write_bit(aux, size, 1);
            bit = aux;
            cout << size << ": " << bit << endl;
            write_byte(node->data->first, aux, size);
        }
        else
            write_bit(aux, size, 0);
        bit = aux;
        cout << size << ": " << bit << endl;

        parsing_pre_order(node->left, aux, size, code);
        parsing_pre_order(node->right, aux, size, code);
    }
}

void write_byte(char &whole, char& part, int &size)
{
    if (size){
        char tmp = (whole >> size);
        char borrar = 0;
        part <<= BYTE_L - size;
        bitset<8> bit = part;
        tmp |= part;
        byteList.push_back(tmp);
        cout << "write_byte (" << whole << ": " << int(whole) << ") " << size << ": " << bit << endl;
        part = (whole << BYTE_L - size);
        part >>= BYTE_L - size;
        for (int i = size; i > 0; --i)
            borrar |= char(pow(2, i - 1));
        part &= borrar;
    }
    else{
        byteList.push_back(whole);
        bitset<8> bit = whole;
        cout << "write_byte (" << whole << ": " << int(whole) << ") " << size << ": " << bit << endl;
    }
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
        bitset<8> bit = aux;
        cout << "write_bit " << size << ": " << bit << endl;
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

/// Compresses a given file using Huffman algorithm
void compress(const char* orgFile)
{
    HashMap<char, size_t> items;
    LSL<TreeNode*> itemList;
    TreeNode *auxItem;
    TreeNode *root;
    fstream file(orgFile, ios::in);
    fstream output;
    char aux;
    char byte = 0;
    int byteSize = 0;
    long pos = 0;

    // If file doesn't exist we throw an error
    if (!file.is_open())
        throw range_error("Origin file not found");
    // Otherwise, we read every character on the file and
    // store it in a hash map
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
    for (size_t i = 0; i < items.size(); ++i){
        auxItem = new TreeNode({*items.get_position(i).key, *items.get_position(i).value});
        itemList.push_back(auxItem);
    }
    items.clear();
    sort_pointer(itemList);
    for (size_t i = 0; i < itemList.size(); ++i)
        cout << itemList[i]->data->first << " (" << int(itemList[i]->data->first) << ") |" << itemList[i]->data->second << endl;
    cout << endl;
    while (1){
        size_t itemValue = itemList[0]->data->second + itemList[1]->data->second;
        root = new TreeNode({'\0', itemValue});
        cout << "first: " << itemList[0]->data->first << " (" << int(itemList[0]->data->first) << ") |" << itemList[0]->data->second << endl;
        cout << "second: " << itemList[1]->data->first << " (" << int(itemList[1]->data->first) << ") |" << itemList[1]->data->second << endl;
        root->left = itemList[0];
        root->right = itemList[1];
        itemList.pop_front();
        itemList.pop_front();
        if (!itemList.size())
            break;
        itemList.push_back(root);
        sort_pointer(itemList);
    }
    parsing_pre_order(root, byte, byteSize, 0);
    aux = 0;
    write_byte(aux, byte, byteSize);
    if (byteSize)
        byteList.push_back(byte);
    
    output.open("respaldo.cmp", ios::out | ios::binary);
    for (size_t i = 0; i < byteList.size(); ++i)
        output.write((char*)&byteList[i], sizeof(char));

    output.close();
    clear_treeNode(root);
}
/// Decompresses a given file using Huffman algorithm
void decompress(const char *orgFile)
{

}


#endif // HUFFMAN_H