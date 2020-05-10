#include <iostream>
#include "huffman.h"
#include "lsl.h"
using namespace std;

int main(int argc, char* argv[])
{
    if (string(argv[1]) == "comprimir")
        compress(argv[2]);
    else if (string(argv[1]) == "descomprimir")
        decompress(argv[2]);
    /*TreeNode *a =  new TreeNode({'1', 1});
    a->left = new TreeNode({'2', 2});
    a->right = new TreeNode({'3', 3});
    in_order_parsing(a);*/
    return 0;
}
