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
    Vector();                                                               // Default constructor
    Vector(const size_type numberOfElements);                               // Fill constructor
    Vector(const size_type numberOfElements, const value_type& fillValue);  // Fill construcotr with fill value

    /*** Iterators ***/
    iterator begin()                { return data;          }
    iterator end()                  { return data + size;   }
    const_iterator begin()  const   { return data;          }
    const_iterator end()    const   { return data + size;   }
    const_iterator cbegin() const   { return data;          }
    const_iterator cend()   const   { return data + size;   }
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

template<class T>
Vector<T>::Vector(const size_type numberOfElements, const value_type& fillValue)
: size(numberOfElements), data(new value_type[numberOfElements])
{
    for(reference element : *this)
        element = fillValue;
}

#endif
