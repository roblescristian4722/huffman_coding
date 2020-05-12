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
    return 0;
}
