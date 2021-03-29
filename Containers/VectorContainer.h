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

    template<class InputIterator>
    Vector(InputIterator first, InputIterator last);                        // Range constructor

    Vector(const Vector& copyVector);                                       // Copy constructor
    Vector(Vector&& moveVector);                                            // Move constructor
    Vector(std::initializer_list<value_type> initializerList);              // Initializer list constructor

    ~Vector();  // Destructor

    /*** Iterators ***/
    iterator begin()                { return data;          }
    iterator end()                  { return data + dataSize;   }
    const_iterator begin()  const   { return data;          }
    const_iterator end()    const   { return data + dataSize;   }
    const_iterator cbegin() const   { return data;          }
    const_iterator cend()   const   { return data + dataSize;   }

    /*** Status Checkers ***/
    size_type size() const { return dataSize; }

private:
    /*** Members ***/
    size_type dataSize = 0;
    T* data = nullptr;
};

template<class T>
Vector<T>::Vector() : dataSize(0), data(nullptr)
{ /* Empty constructor */ }

template<class T>
Vector<T>::Vector(const size_type numberOfElements)
: dataSize(numberOfElements), data(new value_type[numberOfElements])
{ /* Empty constructor */ }

template<class T>
Vector<T>::Vector(const size_type numberOfElements, const value_type& fillValue)
: dataSize(numberOfElements), data(new value_type[numberOfElements])
{
    for(reference element : *this)
        element = fillValue;
}

template<class T>
template<class InputIterator>
Vector<T>::Vector(InputIterator first, InputIterator last)
{
    const difference_type numberOfElements = last - first;

    if(numberOfElements > 0)
    {
        dataSize = numberOfElements;
        data = new value_type[numberOfElements];

        for(size_type index = 0; (index < numberOfElements) && (first != last); ++index, ++first)
            data[index] = *first;
    }
    else
    {
        dataSize = 0;
        data = nullptr;
    }
}

template<class T>
Vector<T>::Vector(const Vector& copyVector)
: dataSize(copyVector.size()), data(new value_type[copyVector.size()])
{
    const_iterator sourceIt = copyVector.cbegin();
    iterator destIt = begin();

    for(;sourceIt != copyVector.cend(); ++sourceIt, ++destIt)
        *destIt = *sourceIt;
}

template<class T>
Vector<T>::Vector(Vector&& moveVector)
: dataSize(moveVector.size()), data(moveVector.data)
{
    moveVector.dataSize = 0;
    moveVector.data     = nullptr;  // Source stolen
}

template<class T>
Vector<T>::Vector(std::initializer_list<value_type> initializerList)
: dataSize(initializerList.size()), data(new T[initializerList.size()])
{
    iterator destIt = begin();

    for(const value_type& element : initializerList)
    {
        *destIt = element;
        ++destIt;
    }
}

template<class T>
Vector<T>::~Vector()
{
    dataSize = 0;
    delete [] data;
}

#endif
