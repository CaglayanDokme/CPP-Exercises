/**
 * @file        VectorContainer.h
 * @details     A template vector container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        March 30, 2021      -> First release
 *              March 31, 2021      -> Single value insert(..) added.
 *              April 1, 2021       ->
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
#include <utility>              // For std::move

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
    reference       at(const size_type position);           // Random access with range check
    const_reference at(const size_type position) const;     // Random access with range check

    reference       front()         { return data[0]; }     // Access to the first element
    const_reference front() const   { return data[0]; }     // Access to the first element

    reference       back()          { return data[sz - 1]; }    // Access to the last element
    const_reference back() const    { return data[sz - 1]; }    // Access to the last element

    /*** Iterators ***/
    iterator begin()                { return data;      }   // Iterator starting from the first element
    iterator end()                  { return data + sz; }   // Iterator starting from the next of the last element
    const_iterator begin()  const   { return data;      }   // Iterator starting from the first element
    const_iterator end()    const   { return data + sz; }   // Iterator starting from the next of the last element
    const_iterator cbegin() const   { return data;      }   // Iterator starting from the first element
    const_iterator cend()   const   { return data + sz; }   // Iterator starting from the next of the last element

    /*** Modifiers ***/
    template<class InputIterator>
    void assign(InputIterator first, InputIterator last);                   // Range assign
    void assign(size_type numberOfElements, const value_type& fillValue);   // Fill assign
    void assign(std::initializer_list<value_type> initializerList);         // Initializer list assign

    void push_back(const value_type& value);    // Push element next to the last element
    void push_back(value_type&& value);         // Push element next to the last element by moving

    void pop_back();    // Remove last element

    template <class InputIterator>
    iterator insert(iterator position, InputIterator first, InputIterator last);                // Range insertion
    iterator insert(iterator position, const value_type& value);                                // Single element insertion
    iterator insert(iterator position, size_type numberOfElements, const value_type& value);    // Multiple insertion and fill
    iterator insert(iterator position, value_type&& value);                                     // Move insertion
    iterator insert(iterator position, std::initializer_list<value_type> il);                   // Initializer list insertion

    iterator erase(iterator position);              // Single element erase
    iterator erase(iterator first, iterator last);  // Iterator based multiple erase

    void swap(Vector& swapVector);  // Swap

    void resize(const size_type newSize);                               // Simple resize
    void resize(const size_type newSize, const value_type& fillValue);  // Resize and fill
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
void Vector<T>::push_back(value_type&& value)
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

    data[sz++] = std::move(value);
}

template<class T>
void Vector<T>::pop_back()
{
    if(size() > 0)
        --sz;
}

template<class T>
template <class InputIterator>
T* Vector<T>::insert(iterator position, InputIterator first, InputIterator last)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    --last;
    for(; last != first; --last)
    {
        position = insert(position, *last);
    }
    return insert(position, *first);
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
T* Vector<T>::insert(iterator position, size_type numberOfElements, const value_type& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(numberOfElements == 0)
        throw(std::invalid_argument("At least one element must be inserted!"));

    if(size() + numberOfElements > capacity()) // Allocation needed
    {
        cap = nextPowerOf2(size() + numberOfElements);
        value_type* newData = new value_type[cap];  // Allocate new space

        size_type newIndex = 0;
        for(iterator it = begin(); it != position; ++it)    // Move elements before position
            newData[newIndex++] = *it;

        iterator newPosition = newData + newIndex;
        for(size_type index = 0; index < numberOfElements; ++index)        // Place new elements
            newData[newIndex++] = value;

        for(iterator it = position; it != end(); ++it)      // Move elements after position
            newData[newIndex++] = *it;

        delete [] data; // Destroy previous data
        data = newData; // Replace data

        sz += numberOfElements;        // Increase size

        return newPosition;
    }
    else
    {
        for(iterator it = end() + numberOfElements - 1; it != position + numberOfElements - 1; --it)  // Move elements after position
            *it = *(it - numberOfElements);

        for(iterator it = position; it != position + numberOfElements; ++it)           // Fill the gap
            *it = value;

        sz += numberOfElements;

        return position;
    }
}

template<class T>
T* Vector<T>::insert(iterator position, value_type&& value)
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
        newData[newIndex++] = std::move(value);

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

        *position = std::move(value);  // Insert element by moving
        ++sz;   // Increase size

        return position;
    }
}

template<class T>
T* Vector<T>::insert(iterator position, std::initializer_list<value_type> il)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        for(const_reference element : il)
            push_back(element);

        return (end() - il.size());     // end() may be changed
    }

    return insert(position, il.begin(), il.end());
}

template<class T>
T* Vector<T>::erase(iterator position)
{
    if((position < begin()) || (position >= end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == (end() - 1))
    {
        pop_back();

        return position;
    }

    for(iterator it = position; it != end(); ++it)  // Shift left each element
        *it = *(it + 1);

    --sz;
    return position;
}

template<class T>
T* Vector<T>::erase(iterator first, iterator last)
{
    if((first < begin()) || (last > end()))
        throw(std::invalid_argument("Iterators must rely inside the container!"));

    const difference_type distance = last - first;

    if(distance > 0)
    {
        const difference_type numberOfMovingElements = end() - last;

        for(size_type index = 0; index < size_type(numberOfMovingElements); ++index)
            *(first + index) = *(last + index);

        sz -= size_type(distance);

        return first;
    }
    else
        throw(std::invalid_argument("Invalid iterator sequence!"));
}

template<class T>
void Vector<T>::swap(Vector& swapVector)
{
    if(this == &swapVector) // Check self swap
        return;

    value_type* tempData;
    size_type tempSzCap;

    tempData        = data;
    data            = swapVector.data;
    swapVector.data = tempData;

    tempSzCap       = sz;
    sz              = swapVector.size();
    swapVector.sz   = tempSzCap;

    tempSzCap       = cap;
    cap             = swapVector.capacity();
    swapVector.cap  = tempSzCap;
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
