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
    typedef T                value_type;
    typedef T&               reference;
    typedef const T&         const_reference;
    typedef T*               iterator;
    typedef const T*         const_iterator;
    typedef std::ptrdiff_t   difference_type;
    typedef std::size_t      size_type;

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

    /*** Operator Overloadings ***/
    Vector& operator=(const Vector& copyVector);                            // Copy assignment operator
    Vector& operator=(Vector&& moveVector);                                 // Move assignment operator
    Vector& operator=(std::initializer_list<value_type> initializerList);   // Initializer list assignment operator

    /*** Iterators ***/
    iterator begin()                { return data;      }
    iterator end()                  { return data + sz; }
    const_iterator begin()  const   { return data;      }
    const_iterator end()    const   { return data + sz; }
    const_iterator cbegin() const   { return data;      }
    const_iterator cend()   const   { return data + sz; }

    /*** Status Checkers ***/
    size_type size() const { return sz; }
    // size_type max_size() const {}        // TODO Search for implementation
    size_type capacity() const { return cap; }
    bool empty() const { return (sz == 0); }


private:
    /*** Members ***/
    size_type sz    = 0;
    size_type cap   = 0;
    T* data         = nullptr;
};

// Finds the next power of 2 which is greater than N.
std::size_t nextPowerOf2(std::size_t N)
{
    if(N == 0)
        return 0;

    std::size_t maxValuedBit = 1;

    while(N != 0)
    {
        N = N >> 1;
        maxValuedBit = maxValuedBit << 1;
    }

    return maxValuedBit;
}

template<class T>
Vector<T>::Vector() : sz(0), cap(0), data(nullptr)
{ /* Empty constructor */ }

template<class T>
Vector<T>::Vector(const size_type numberOfElements)
: sz(numberOfElements), cap(nextPowerOf2(sz)), data((cap != 0) ? new value_type[cap] : nullptr)
{ /* Empty constructor */ }

template<class T>
Vector<T>::Vector(const size_type numberOfElements, const value_type& fillValue)
: sz(numberOfElements), cap(nextPowerOf2(sz)), data((cap != 0) ? new value_type[cap] : nullptr)
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
        sz      = numberOfElements;
        cap     = nextPowerOf2(sz);
        data    = new value_type[cap];

        for(size_type index = 0; (index < cap) && (first != last); ++index, ++first)
            data[index] = *first;
    }
    else
    {
        sz      = 0;
        cap     = 0;
        data    = nullptr;
    }
}

template<class T>
Vector<T>::Vector(const Vector& copyVector)
    : sz(copyVector.size()), cap(copyVector.capacity()), data((cap != 0) ? new value_type[cap] : nullptr)
{
    const_iterator sourceIt = copyVector.cbegin();
    iterator destIt = begin();

    for(;sourceIt != copyVector.cend(); ++sourceIt, ++destIt)
        *destIt = *sourceIt;
}

template<class T>
Vector<T>::Vector(Vector&& moveVector)
: sz(moveVector.size()), cap(moveVector.capacity()), data(moveVector.data)
{
    moveVector.sz   = 0;
    moveVector.cap  = 0;
    moveVector.data = nullptr;  // Source stolen
}

template<class T>
Vector<T>::Vector(std::initializer_list<value_type> initializerList)
: sz(initializerList.size()), cap(nextPowerOf2(sz)), data((cap != 0) ? new value_type[cap] : nullptr)
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
    sz  = 0;
    cap = 0;
    delete [] data;
}

template<class T>
Vector<T>& Vector<T>::operator=(const Vector& copyVector)
{
    if(this == &copyVector) // Check self assignment
        return *this;

    // Destroy resource of the left vector
    sz  = 0;
    cap = 0;
    delete [] data;

    // Allocate space for incoming elements
    sz      = copyVector.size();
    cap     = copyVector.capacity();
    data    = (cap != 0) ? new value_type[cap] : nullptr;

    // Copy elements
    const_iterator sourceIt = copyVector.cbegin();
    iterator destIt = begin();

    for(;sourceIt != copyVector.cend(); ++sourceIt, ++destIt)
        *destIt = *sourceIt;

    return *this;
}

template<class T>
Vector<T>& Vector<T>::operator=(Vector&& moveVector)
{
    if(this == &moveVector) // Check self assignment
        return *this;

    // Destroy resource of the left vector
    sz = 0;
    cap = 0;
    delete [] data;

    // Steal resources of move(right) vector
    sz      = moveVector.size();
    cap     = moveVector.cap;
    data    = moveVector.data;

    // Prevent destruction of stolen resource
    moveVector.sz   = 0;
    moveVector.cap  = 0;
    moveVector.data = nullptr;

    return *this;
}

template<class T>
Vector<T>& Vector<T>::operator=(std::initializer_list<value_type> initializerList)
{
    // Destroy resource of the left vector
    sz  = 0;
    cap = 0;
    delete [] data;

    // Adjust new resource
    sz      = initializerList.size();
    cap     = nextPowerOf2(sz);
    data    = (cap != 0) ? new value_type[cap] : nullptr;

    iterator destIt = begin();

    for(const value_type& element : initializerList)
    {
        *destIt = element;
        ++destIt;
    }

    return *this;
}

#endif
