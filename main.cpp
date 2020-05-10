#include <iostream>
#include "huffman.h"
#include "lsl.h"
using namespace std;

int main(int argc, char* argv[])
{
    //compress(argv[1]);
    decompress(argv[1]);
    /*TreeNode *a =  new TreeNode({'1', 1});
    a->left = new TreeNode({'2', 2});
    a->right = new TreeNode({'3', 3});
    in_order_parsing(a);*/
    return 0;
}
