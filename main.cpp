#include <iostream>
#include "huffman.h"
#include "lsl.h"
using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 4)
        cout << "MODO DE USO: [nombre del ejecutable] [comprimir / descomprimir] "
                "[nombre del archivo origen] [nombre del archivo destino]"
             << endl;
    else if (string(argv[1]) == "comprimir")
        compress(argv[2], argv[3]);
    else if (string(argv[1]) == "descomprimir")
        decompress(argv[2], argv[3]);
    return 0;
}
