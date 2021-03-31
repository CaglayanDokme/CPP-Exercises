/**
 * @file        VectorContainer.h
 * @details     A template vector container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        March 30, 2021    -> First release
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
#include <stdexcept>            // For exceptions

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

    reference       operator[](const size_type position)        { return data[position]; }  // Element access by lValue
    const_reference operator[](const size_type position) const  { return data[position]; }  // Element access by const lValue

    /*** Element Access ***/
    reference       at(const size_type position);
    const_reference at(const size_type position) const;

    reference       front()         { return data[0]; }
    const_reference front() const   { return data[0]; }

    reference       back()          { return data[sz - 1]; }
    const_reference back() const    { return data[sz - 1]; }

    /*** Iterators ***/
    iterator begin()                { return data;      }
    iterator end()                  { return data + sz; }
    const_iterator begin()  const   { return data;      }
    const_iterator end()    const   { return data + sz; }
    const_iterator cbegin() const   { return data;      }
    const_iterator cend()   const   { return data + sz; }

    /*** Modifiers ***/
    template<class InputIterator>
    void assign(InputIterator first, InputIterator last);                   // Range assign
    void assign(size_type numberOfElements, const value_type& fillValue);   // Fill assign
    void assign(std::initializer_list<value_type> initializerList);         // Initializer list assign

    void push_back(const value_type& value);
    void push_back(value_type&& value); // TODO Not implemented yet

    void pop_back();    // Remove last elements

    iterator insert(iterator position, const value_type& value);    // Single element insertion

    void resize(const size_type newSize);
    void resize(const size_type newSize, const value_type& fillValue);
    void reserve(const size_type reservationSize);
    void shrink_to_fit();

    /*** Size and Capacity Checkers ***/
    size_type size()        const { return sz;          }
    size_type capacity()    const { return cap;         }
    // size_type max_size() const {                     }        // TODO Search for implementation
    bool empty()            const { return (sz == 0);   }

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
        return 1;

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
        throw(std::logic_error("Wrong iterator sequence!"));
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

template<class T>
T& Vector<T>::at(const size_type position)
{
    if(position < sz)
        return data[position];

    throw(std::out_of_range("Index is out-of-range!"));
}

template<class T>
const T& Vector<T>::at(const size_type position) const
{
    if(position < sz)
        return data[position];

    throw(std::out_of_range("Index is out-of-range!"));
}

template<class T>
template<class InputIterator>
void Vector<T>::assign(InputIterator first, InputIterator last)
{
    const difference_type numberOfElements = last - first;

    if(numberOfElements > 0)
    {
        if(numberOfElements > capacity())  // Is a bigger space needed?
        {
            delete [] data; // Destroy previous data

            cap     = numberOfElements;
            data    = new value_type[cap];  // Reallocate space for incoming elements
        }

        for(size_type index = 0; (index < size_type(numberOfElements)) && (first != last); ++index, ++first)
            data[index] = *first;

        sz = numberOfElements;  // Determine new size
    }
    else
        throw(std::logic_error("Wrong iterator sequence!"));
}

template<class T>
void Vector<T>::assign(size_type numberOfElements, const value_type& fillValue)
{
    if(numberOfElements > 0)
    {
        if(numberOfElements > capacity())
        {
            delete [] data; // Destroy previous data

            cap     = numberOfElements;
            data    = new value_type[cap];  // Reallocate space for incoming elements
        }

        for(size_type index = 0; index < size_type(numberOfElements); ++index)
            data[index] = fillValue;

        sz = numberOfElements;  // Determine new size
    }
    else
        throw(std::invalid_argument("Assignment size error!"));
}

template<class T>
void Vector<T>::assign(std::initializer_list<T> initializerList)
{
    if(initializerList.size() > capacity())
    {
        delete [] data; // Destroy previous data

        cap     = initializerList.size();
        data    = new value_type[cap];  // Reallocate space for incoming elements
    }

    size_type index = 0;
    for(const value_type& element : initializerList)
        data[index++] = element;

    sz = initializerList.size();
}

template<class T>
void Vector<T>::push_back(const value_type& value)
{
    if(size() == capacity())    // Size is about to surpass the capacity
    {
        cap = nextPowerOf2(capacity());
        value_type* newData = new value_type[cap];

        for(size_type index = 0; index < size(); ++index)
            newData[index] = data[index];

        delete [] data;
        data = newData;
    }

    data[sz++] = value;
}

template<class T>
void Vector<T>::pop_back()
{
    if(size() > 0)
        --sz;
}

template<class T>
T* Vector<T>::insert(iterator position, const value_type& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        push_back(value);

        return (end() - 1);     // end() changed
    }

    // Move objects after the position
    if(size() == capacity())    // Allocation needed
    {
        cap = nextPowerOf2(capacity());
        value_type* newData = new value_type[cap];

        size_type newIndex = 0;
        for(iterator it = begin(); it != position; ++it)    // Move elements before position
            newData[newIndex++] = *it;

        // Place new value
        iterator newPosition = newData + newIndex;  // Save data position for function return
        newData[newIndex++] = value;

        for(iterator it = position; it != end(); ++it)  // Move elements after position
            newData[newIndex++] = *it;

        delete [] data; // Destroy previous data
        data = newData; // Replace data

        ++sz;   // Increase size
        return newPosition;
    }
    else    // Allocation not needed
    {
        for(iterator it = end(); it != position; --it)  // Move elements after position
            *it = *(it - 1);

        *position = value;  // Insert element
        ++sz;   // Increase size

        return position;
    }
}

template<class T>
void Vector<T>::resize(const size_type newSize)
{
    if(newSize == capacity())
        return;

    value_type* newArea = new value_type[newSize];

    for(size_type index = 0; (index < newSize) && (index < size()); ++index)
        newArea[index] = data[index];

    delete [] data; // Destroy old resource

    cap     = newSize;
    sz      = (newSize < sz) ? newSize : sz;
    data    = newArea;
}

template<class T>
void Vector<T>::resize(const size_type newSize, const value_type& fillValue)
{
    if(newSize == capacity())
        return;

    value_type* newArea = new value_type[newSize];

    size_type index = 0;
    for(index = 0; (index < newSize) && (index < size()); ++index)
        newArea[index] = data[index];

    for( ; index < newSize; ++index)
        newArea[index] = fillValue;

    delete [] data; // Destroy old resource

    cap     = newSize;
    sz      = (newSize < sz) ? newSize : sz;
    data    = newArea;
}

template<class T>
void Vector<T>::reserve(const size_type reservationSize)
{
    if(reservationSize <= capacity())
        return;

    value_type* newArea = new value_type[reservationSize];

    for(size_type index = 0; index < size(); ++index)
        newArea[index] = data[index];

    delete [] data; // Destroy old resource

    cap     = reservationSize;
    data    = newArea;
}

template<class T>
void Vector<T>::shrink_to_fit()
{
    if((sz == 0) || (sz == cap))
        return;

    value_type* newArea = new value_type[sz];

    for(size_type index = 0; index < size(); ++index)
        newArea[index] = data[index];

    delete [] data; // Destroy old resource

    cap     = sz;
    data    = newArea;
}

#endif
