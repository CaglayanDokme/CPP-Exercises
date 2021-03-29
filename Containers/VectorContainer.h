/**
 * @file        VectorContainer.h
 * @details     A template vector container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        March 29, 2021    -> First release
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 *  @copyright  No copyright.
 */

/*** Recursive inclusion preventer ***/
#ifndef VECTOR_CONTAINER_H
#define VECTOR_CONTAINER_H

/*** Libraries ***/
#include <cstddef>              // For std::size_t, ptrdiff_t
#include <initializer_list>     // For std::initializer_list

/*** Container Class ***/
template<class T>
class Vector{
public:
    /*** C++ Standard Named Requirements for Containers ***/
    typedef T           value_type;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T*          iterator;
    typedef const T*    const_iterator;
    typedef ptrdiff_t   difference_type;
    typedef size_t      size_type;

    /*** Constructors and Destructors ***/
    Vector();                                   // Default constructor
    Vector(const size_type numberOfElements);   // Fill constructor

private:
    size_type size;
    T* data;
};

template<class T>
Vector<T>::Vector() : size(0), data(nullptr)
{ /* Empty constructor */ }

template<class T>
Vector<T>::Vector(const size_type numberOfElements)
: size(numberOfElements), data(new value_type[numberOfElements])
{ /* Empty constructor */ }

#endif
