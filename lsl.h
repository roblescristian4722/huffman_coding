#ifndef LSL_H
#define LSL_H
#include <iostream>
#include <stdexcept>
using namespace std;

template<typename T>
class LSL{
    private:
        struct NodoLSL{
            T dato;
            NodoLSL* siguiente;
            //sig = nullptr representa un valr por omision para la variable sig
            NodoLSL(const T& elem, NodoLSL* sig = nullptr){
                dato = elem;
                siguiente = sig;
            }
        };

        size_t listSize;
        NodoLSL* listFront;
        NodoLSL* listBack;
    public:
        LSL(){
            listSize = 0;
            listFront = nullptr;
            listBack = nullptr;
        }
        ~LSL(){
            clear();
        }
        bool empty()const;
        size_t size()const;
        void push_front(const T& element);
        void push_back(const T& element);
        const T& front()const;
        const T& back()const;
        void pop_front();
        void pop_back();
        void insert(size_t position, const T& element);
        void erase(size_t position);
        void clear();
        void remove(const T& value);
        T& operator [](size_t index);
        LSL& operator =(const LSL<T>& other);
};

template<typename T>
bool LSL<T>::empty()const{
    return listSize == 0;
}

template<typename T>
size_t LSL<T>::size()const{
    return listSize;
}

template<typename T>
void LSL<T>::push_front(const T& element){
    if(empty()){
        listFront = new NodoLSL(element);
        listBack = listFront;
    }
    else{
        NodoLSL* temp = new NodoLSL(element, listFront);
        listFront = temp;
    }
    ++listSize;
}

template<typename T>
void LSL<T>::push_back(const T& element){
    if(empty()){
        listFront = new NodoLSL(element);
        listBack = listFront;
    }
    else{
        NodoLSL* temp = new NodoLSL(element);
        listBack->siguiente = temp;
        listBack = temp;
    }
    ++listSize;
}

template<typename T>
const T& LSL<T>::front()const{
    if(empty()){
        throw range_error("Trying front() on empty list");
    }
    return listFront->dato;

}

template<typename T>
const T& LSL<T>::back()const{
    if(empty()){
        throw range_error("Trying back() on empty list");
    }

    return listBack->dato;

}
template<typename T>
void LSL<T>::pop_front(){
    if(empty()){
        throw range_error("Trying pop_front() on empty list");
    }
    NodoLSL* temp = listFront;
    listFront = listFront->siguiente;
    delete temp;
    temp = nullptr;
    --listSize;
}

template<typename T>
void LSL<T>::pop_back(){
    if(empty()){
        throw range_error("Trying pop_back() on empty list");
    }
    NodoLSL* temp = listFront;
    while(temp->siguiente != nullptr && temp->siguiente != listBack){
        temp = temp->siguiente;
    }
    delete temp->siguiente;
    temp->siguiente = nullptr;
    listBack = temp;
    listBack->siguiente = nullptr;

    --listSize;
}

template<typename T>
void LSL<T>::insert(size_t position, const T& element){
    if(position > listSize){
        throw range_error("Trying insert() in non valid position");
    }
    if(position == 0){
        push_front(element);
    }
    else if(position == listSize){
        push_back(element);
    }
    else{
        NodoLSL* temp = listFront;
        for(size_t i(0); i < position-1; ++i){
            temp = temp->siguiente;
        }
        NodoLSL* nuevo = new NodoLSL(element, temp->siguiente);
        temp->siguiente = nuevo;

        ++listSize;
    }
}

template<typename T>
void LSL<T>::erase(size_t position){
    if(empty()){
        throw range_error("Trying erase() on empty list");
    }
    if(position >= listSize){
        throw range_error("Trying erase() in non valid position");
    }
    else if(position == 0){
        pop_front();
    }
    else if(position == listSize-1){
        pop_back();
    }
    else{
        NodoLSL* temp = listFront;
        for(size_t i(0); i < position-1; ++i){
            temp = temp->siguiente;
        }
        NodoLSL* eliminar = temp->siguiente;
        temp->siguiente = eliminar->siguiente;
        delete eliminar;
        eliminar = nullptr;
        --listSize;
    }
}

template<typename T>
void LSL<T>::clear(){
    for(size_t i(0); i < listSize; ++i){
        pop_front();
    }
    listSize = 0;
    listFront = nullptr;
    listBack = nullptr;
}

template<typename T>
void LSL<T>::remove(const T& value){
    if(empty()){
        throw range_error("Trying remove() from empty list");
    }
    NodoLSL* temp = listFront;
    size_t i = 0;
    T dato;
    while(temp != nullptr){
        dato = temp->dato;
        temp = temp->siguiente;
        if(dato == value){
            erase(i);
            --i;
        }
        ++i;
    }

}

template<typename T>
T& LSL<T>::operator [](size_t index){
    if(empty()){
        throw range_error("Trying [] on empty list");
    }
    if(index >= listSize){
        throw range_error("Trying [] in non valid position");
    }
    NodoLSL* temp = listFront;
    for(size_t i(0); i < index; ++i){
        temp = temp->siguiente;
    }
    return temp->dato;
}

template<typename T>
LSL<T>& LSL<T>::operator=(const LSL<T>& other){
    clear();
    NodoLSL* temp = other.listFront;
    for(size_t i(0); i<other.size(); ++i){
        this->push_back(temp->dato);
        temp = temp->siguiente;
    }
    return *this;
}

template <typename T>
void sort(LSL<T> &list)
{
    size_t n = list.size();
    T tmp;
    int brecha = (int)n / 2;
    int j;
    while (brecha > 0){
        for (int i = brecha; i < (int)n; ++i){
            tmp = list[i];
            j = i;
            while (j >= brecha && list[j - brecha] > tmp){
                list[j] = list[j - brecha];
                j -= brecha;
            }
            list[j] = tmp;
        }
        brecha /= 2;
    }
}

#endif //LSL_H
