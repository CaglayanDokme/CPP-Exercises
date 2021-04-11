/**
 * @file        VectorContainer.h
 * @details     A template vector container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        March 30, 2021 -> First release
 *              March 31, 2021 -> Single value insert(..) added.
 *              April 12, 2021 -> Allocation policy optimizated using placement new.
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
#include <new>                  // For ::operator new
#include <ostream>              // For std::cout

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
    void clear() { destroyRange(begin(), end()); sz = 0; }

    template <class... Args>
    iterator emplace(iterator position, Args&&... args);

    template <class... Args>
    void emplace_back(Args&&... args);

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

    /*** Helper Functions ***/
    template<class InputIterator>
    void assignRangeForward(InputIterator from, InputIterator to, iterator destination);
    void assignRangeForward(iterator from, iterator to, const value_type& value);

    template<class InputIterator>
    void assignRangeBackward(InputIterator from, InputIterator to, iterator destination);

    template<class InputIterator>
    void moveRangeForward(InputIterator from, InputIterator to, iterator destination);
    void moveRangeForward(iterator from, iterator to, const value_type& value);

    template<class InputIterator>
    void moveRangeBackward(InputIterator from, InputIterator to, iterator destination);

    template<class InputIterator>
    void copyRangeForward(InputIterator from, InputIterator to, iterator destination);
    void copyRangeForward(iterator from, iterator to, const value_type& value);

    template<class InputIterator>
    void copyRangeBackward(InputIterator from, InputIterator to, iterator destination);

    void destroyRange(iterator from, iterator to);
    void destroyPointer(iterator ptr);
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
: sz(numberOfElements), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    // Construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index)
        new(data + index) value_type;
}

template<class T>
Vector<T>::Vector(const size_type numberOfElements, const value_type& fillValue)
: sz(numberOfElements), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    // Construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index)
        new(data + index) value_type(fillValue);
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

        // Allocate space for incoming elements
        // Construct will take place at each element insertion
        data    = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        for(size_type index = 0; (index < cap) && (first != last); ++index, ++first)
            new(data + index) value_type(*first);
    }
    else
        throw(std::logic_error("Wrong iterator sequence!"));
}

template<class T>
Vector<T>::Vector(const Vector& copyVector)
: sz(copyVector.size()), cap(copyVector.capacity()), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data    = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    const_iterator sourceIt = copyVector.cbegin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);

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
: sz(initializerList.size()), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    const_iterator sourceIt = initializerList.begin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);
}

template<class T>
Vector<T>::~Vector()
{
    destroyRange(begin(), end());
    destroyPointer(data);

    sz  = 0;
    cap = 0;
}

template<class T>
Vector<T>& Vector<T>::operator=(const Vector& copyVector)
{
    if(this == &copyVector) // Check self assignment
        return *this;

    if(copyVector.size() > capacity())  // Reallocation needed?
    {
        destroyRange(begin(), end());   // Destroy elements of the left vector
        destroyPointer(data);

        sz      = 0;
        cap     = nextPowerOf2(copyVector.size());  // Determine new capacity
        data    = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));   // Allocate resource
    }

    /* Currently used posisitions inside the container are assignable.
     * The rest of them needs construction by copying. */
    if(0 == sz)
        copyRangeForward(copyVector.begin(), copyVector.begin() + copyVector.size(), begin());
    else if(copyVector.size() < sz)
    {
        assignRangeForward(copyVector.begin(), copyVector.begin() + copyVector.size(), begin());
        destroyRange(begin() + copyVector.size(), end());
    }
    else
    {
        assignRangeForward(copyVector.begin(), copyVector.begin() + sz, begin());
        copyRangeForward(copyVector.begin() + sz, copyVector.end(), begin() + sz);
    }

    sz = copyVector.size(); // Adjust size

    return *this;
}

template<class T>
Vector<T>& Vector<T>::operator=(Vector&& moveVector)
{
    if(this != &moveVector)     // Check self assignment
        swap(moveVector);       // Steal resources of move(right) vector

    return *this;
}

template<class T>
Vector<T>& Vector<T>::operator=(std::initializer_list<value_type> initializerList)
{
    if(initializerList.size() > capacity()) // Reallocation needed?
    {
        destroyRange(begin(), end());   // Destroy elements of the left vector
        destroyPointer(data);

        sz      = 0;
        cap     = nextPowerOf2(initializerList.size());  // Determine new capacity
        data    = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));   // Allocate resource
    }

    /* Currently used posisitions inside the container are assignable.
     * The rest of them needs construction by copying. */
    if(0 == sz)
        copyRangeForward(initializerList.begin(), initializerList.begin() + initializerList.size(), begin());
    else if(initializerList.size() < sz)
    {
        assignRangeForward(initializerList.begin(), initializerList.begin() + initializerList.size(), begin());
        destroyRange(begin() + initializerList.size(), end());
    }
    else /* (initializerList.size() > sz) */
    {
        assignRangeForward(initializerList.begin(), initializerList.begin() + sz, begin());
        copyRangeForward(initializerList.begin() + sz, initializerList.end(), begin() + sz);
    }

    sz = initializerList.size(); // Adjust size

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
        // Destroy the elements of the vector
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        sz = numberOfElements;  // Determine new size

        if(numberOfElements > capacity())  // Is a bigger space needed?
        {
            /* The allocated space will not be used anymore.
             * We shall release the resource for further usage */
            ::operator delete(static_cast<void*>(data));

            cap = nextPowerOf2(numberOfElements);

            // Allocate new resource
            if(cap > 0)
                data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));
        }

        // Copy construct the elements at predetermined locations
        for(size_type index = 0; (index < cap) && (first != last); ++index, ++first)
            new(data + index) value_type(*first);
    }
    else
        throw(std::logic_error("Wrong iterator sequence!"));
}

template<class T>
void Vector<T>::assign(size_type numberOfElements, const value_type& fillValue)
{
    if(numberOfElements > 0)
    {
        // Destroy the elements of the vector
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        sz = numberOfElements;  // Determine new size

        if(numberOfElements > capacity())  // Is a bigger space needed?
        {
            /* The allocated space will not be used anymore.
             * We shall release the resource for further usage */
            ::operator delete(static_cast<void*>(data));

            cap = nextPowerOf2(numberOfElements);

            // Allocate new resource
            if(cap > 0)
                data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));
        }

        for(size_type index = 0; index < size_type(numberOfElements); ++index)
            new(data + index) value_type(fillValue);
    }
    else
        throw(std::invalid_argument("Assignment size error!"));
}

template<class T>
void Vector<T>::assign(std::initializer_list<T> initializerList)
{
    // Destroy the elements of the vector
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for(size_type index = 0; index < sz; ++index)
        (data + index)->~value_type();

    sz = initializerList.size();  // Determine new size

    if(initializerList.size() > capacity())  // Is a bigger space needed?
    {
        /* The allocated space will not be used anymore.
         * We shall release the resource for further usage */
        ::operator delete(static_cast<void*>(data));

        cap = nextPowerOf2(initializerList.size());

        // Allocate new resource
        if(cap > 0)
            data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));
    }

    const_iterator sourceIt = initializerList.begin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);
}

template<class T>
void Vector<T>::push_back(const value_type& value)
{
    if(size() == capacity())    // Size is about to surpass the capacity
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(capacity());
        value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        copyRangeForward(begin(), end(), newData);  // Copy construct elements at their new place

        // Destroy old resource
        destroyRange(begin(), end());
        destroyPointer(data);

        data = newData;
    }

    // Copy construct new element with the incoming one
    new(data + sz++) value_type(value);
}

template<class T>
void Vector<T>::push_back(value_type&& value)
{
    if(size() == capacity())    // Size is about to surpass the capacity
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(capacity());
        value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        copyRangeForward(begin(), end(), newData);  // Copy construct elements at their new place

        // Destroy old resource
        destroyRange(begin(), end());
        destroyPointer(data);

        data = newData;
    }

    // Move construct new element with the incoming
    new(data + sz++) value_type(std::move(value));
}

template<class T>
void Vector<T>::pop_back()
{
    if(size() > 0)
    {
        --sz;
        (data + sz)->~value_type(); // Destroy popped element
    }
}

template<class T>
template <class InputIterator>
T* Vector<T>::insert(iterator position, InputIterator first, InputIterator last)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    difference_type numberOfElements    = last - first;
    difference_type positionAsIndex     = position - begin();

    if(numberOfElements <= 0)
        throw(std::logic_error("Wrong iterator sequence!"));

    value_type* newData = nullptr;  // A reallocation may be needed

    /* If there will be reallocations for multiple times,
     * we better reallocate for the final size in a single operation
     * to enhance optimization of insertion. */
    if(size() + numberOfElements > capacity())    // Is bigger space needed?
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(size() + numberOfElements);
        newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        // Copy the elements on the left side
        copyRangeForward(begin(), position, newData);   // Copy construct existing elements at their new place
    }

    // Move the elements on the right side
    if(newData != nullptr)  // If a reallocation happened
        copyRangeForward(position, end(), newData + positionAsIndex + numberOfElements);   // Gap for the incoming ones
    else
    {
        /* If there wasn't a reallocation, then move constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - numberOfElements, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - numberOfElements, position + numberOfElements);
    }

    if(newData != nullptr)  // If a reallocation happened
        moveRangeForward(first, last, newData + positionAsIndex);   // Copy construct new elements at given position
    else
        assignRangeForward(first, last, position);  // Copy assign new elements at given position

    if(newData != nullptr)  // Change data pointer if a reallocation happened
    {
        destroyRange(begin(), end());   // Explicitly destroy range
        destroyPointer(data);           // Destroy data pointer

        data = newData;                 // Assign new pointer
    }

    sz += numberOfElements;

    return (data + positionAsIndex);
}

template<class T>
T* Vector<T>::insert(iterator position, const value_type& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        push_back(value);

        return (end() - 1);     // end() may be changed
    }

    const size_type positionAsIndex = size_type(position - begin());
    value_type* newData = nullptr;  // A reallocation may be needed

    if(size() == capacity())    // Reallocation needed
    {
        // New space for incoming data
        cap = nextPowerOf2(capacity()); // Capacity changed
        newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        // Move the elements on the left side
        copyRangeForward(begin(), position, newData);   // Copy construct existing elements at their new place
    }

    // Move the elements on the right side
    if(newData != nullptr)  // If a reallocation happened
        copyRangeForward(position, end(), newData + positionAsIndex + 1);   // 1 element gap for the incoming one
    else
    {
        /* If there wasn't a reallocation, then move constructing the last element is enough.
         * The remaining ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - 1, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - 1, position + 1);
    }

    if(newData != nullptr)  // If a reallocation happened
        new(newData + positionAsIndex) value_type(value);   // Copy construct new element at given position
    else
        *(position) = value;    // Copy assign new element at given position

    if(newData != nullptr)  // Change data pointer if a reallocation happened
    {
        destroyRange(begin(), end());   // Explicitly destroy range
        destroyPointer(data);           // Destroy data pointer

        data = newData;                 // Assign new pointer
    }

    sz++;

    return (data + positionAsIndex);  // Data may be changed
}

template<class T>
T* Vector<T>::insert(iterator position, size_type numberOfElements, const value_type& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(numberOfElements == 0)
        throw(std::invalid_argument("At least one element must be inserted!"));

    difference_type positionAsIndex = position - begin();
    value_type* newData = nullptr;  // A reallocation may be needed

    /* If there will be reallocations for multiple times,
     * we better reallocate for the final size in a single operation
     * to enhance optimization of insertion. */
    if(size() + numberOfElements > capacity())    // Is bigger space needed?
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(size() + numberOfElements);
        newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        // Move the elements on the left side
        copyRangeForward(begin(), position, newData);   // Copy construct existing elements at their new place
    }

    // Move the elements on the right side
    if(newData != nullptr)  // If a reallocation happened
        copyRangeForward(position, end(), newData + positionAsIndex + numberOfElements);   // Gap for the incoming ones
    else
    {
        /* If there wasn't a reallocation, then move constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - numberOfElements, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - numberOfElements, position + numberOfElements);
    }

    if(newData != nullptr)  // If a reallocation happened
        moveRangeForward(newData + positionAsIndex, newData + positionAsIndex + numberOfElements, value);       // Copy construct new elements at given position
    else
        assignRangeForward(position, position + numberOfElements, value);     // Copy assign new elements at given position

    if(newData != nullptr)  // Change data pointer if a reallocation happened
    {
        destroyRange(begin(), end());   // Explicitly destroy range
        destroyPointer(data);           // Destroy data pointer

        data = newData;                 // Assign new pointer
    }

    sz += numberOfElements;

    return (data + positionAsIndex);
}

template<class T>
T* Vector<T>::insert(iterator position, value_type&& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        push_back(value);

        return (end() - 1);     // end() may be changed
    }

    const size_type positionAsIndex = size_type(position - begin());
    value_type* newData = nullptr;  // A reallocation may be needed

    if(size() == capacity())    // Reallocation needed
    {
        // New space for incoming data
        cap = nextPowerOf2(capacity()); // Capacity changed
        newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        // Move the elements on the left side
        copyRangeForward(begin(), position, newData);   // Copy construct existing elements at their new place
    }

    // Move the elements on the right side
    if(newData != nullptr)  // If a reallocation happened
        copyRangeForward(position, end(), newData + positionAsIndex + 1);   // 1 element gap for the incoming one
    else
    {
        /* If there wasn't a reallocation, then move constructing the last element is enough.
         * The remaining ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - 1, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - 1, position + 1);
    }

    if(newData != nullptr)  // If a reallocation happened
        new(newData + positionAsIndex) value_type(std::move(value));   // Copy construct new element at given position
    else
        *(position) = std::move(value);    // Move assign new element at given position

    if(newData != nullptr)  // Change data pointer if a reallocation happened
    {
        destroyRange(begin(), end());   // Explicitly destroy range
        destroyPointer(data);           // Destroy data pointer

        data = newData;                 // Assign new pointer
    }

    sz++;

    return (data + positionAsIndex);  // Data may be changed
}

template<class T>
T* Vector<T>::insert(iterator position, std::initializer_list<value_type> initializerList)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    return insert(position, initializerList.begin(), initializerList.end());
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

    assignRangeForward(position + 1, end(), position);  // Shift left the elements on the right
    destroyRange(end() - 1, end());                     // Destroy outsider elements
    --sz;   // Decrement size

    return position;
}

template<class T>
T* Vector<T>::erase(iterator first, iterator last)
{
    if((first < begin()) || (last > end()))
        throw(std::invalid_argument("Iterators must rely inside the container!"));

    if(last == first)
        throw(std::invalid_argument("Invalid iterator sequence!"));

    const difference_type distance = last - first;

    assignRangeForward(first + distance, end(), first); // Shift left the elements on the right
    destroyRange(end() - distance, end());
    sz -= distance;

    return first;
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
template <class... Args>
T* Vector<T>::emplace(iterator position, Args&&... args)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    const size_type positionAsIndex = size_type(position - begin());
    value_type* newData = nullptr;  // A reallocation may be needed

    if(size() == capacity())    // Reallocation needed
    {
        // New space for incoming data
        cap = nextPowerOf2(capacity()); // Capacity changed
        newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        // Move the elements on the left side
        copyRangeForward(begin(), position, newData);   // Copy construct existing elements at their new place
    }

    // Move the elements on the right side
    if(newData != nullptr)  // If a reallocation happened
        copyRangeForward(position, end(), newData + positionAsIndex + 1);   // 1 element gap for the incoming one
    else
    {
        /* If there wasn't a reallocation, then move constructing the last element is enough.
         * The remaining ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - 1, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - 1, position + 1);
    }

    if(newData != nullptr)  // If a reallocation happened
        new(newData + positionAsIndex) value_type(args...);   // Copy construct new element at given position
    else
    {
        position->~value_type();                // Destroy old element
        new(position) value_type(args...);      // Copy assign new element at given position
    }

    if(newData != nullptr)  // Change data pointer if a reallocation happened
    {
        destroyRange(begin(), end());   // Explicitly destroy range
        destroyPointer(data);           // Destroy data pointer

        data = newData;                 // Assign new pointer
    }

    sz++;

    return (data + positionAsIndex);  // Data may be changed
}

template<class T>
template <class... Args>
void Vector<T>::emplace_back(Args&&... args)
{
    if(size() == capacity())    // Size is about to surpass the capacity
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(capacity());
        value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        copyRangeForward(begin(), end(), newData);  // Copy construct elements at their new place

        // Destroy old resource
        destroyRange(begin(), end());
        destroyPointer(data);

        data = newData;
    }

    // Copy construct new element with the incoming one
    new(data + sz++) value_type(args...);
}

template<class T>
void Vector<T>::resize(const size_type newSize)
{
    if(0 == newSize)
        return clear();
    else if(size() == newSize)
        return;
    else { }

    if(newSize < size())
    {
        destroyRange(begin() + newSize, end());
    }
    else
    {
        if(newSize < capacity())
        {
            for(size_type index = size(); index < newSize; ++index)
                new(data + index) value_type;   // Default construct new elements
        }
        else    // Reallocation needed
        {
            /* A bigger place needed for current and incoming elements */
            cap = nextPowerOf2(newSize);
            value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

            copyRangeForward(begin(), end(), newData);  // Copy construct elements at their new place

            // Destroy old resource
            destroyRange(begin(), end());
            destroyPointer(data);

            data = newData;

            for(size_type index = size(); index < newSize; ++index)
                new(data + index) value_type;   // Default construct new elements
        }
    }

    sz = newSize;
}

template<class T>
void Vector<T>::resize(const size_type newSize, const value_type& fillValue)
{
    if(0 == newSize)
        return clear();
    else if(size() == newSize)
        return;
    else { }

    if(newSize < size())
    {
        destroyRange(begin() + newSize, end());
    }
    else
    {
        if(newSize < capacity())
        {
            for(size_type index = size(); index < newSize; ++index)
                new(data + index) value_type(fillValue);   // Copy construct new elements
        }
        else    // Reallocation needed
        {
            /* A bigger place needed for current and incoming elements */
            cap = nextPowerOf2(newSize);
            value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

            copyRangeForward(begin(), end(), newData);  // Copy construct elements at their new place

            // Destroy old resource
            destroyRange(begin(), end());
            destroyPointer(data);

            data = newData;

            for(size_type index = size(); index < newSize; ++index)
                new(data + index) value_type(fillValue);   // Copy construct new elements
        }
    }

    sz = newSize;
}

template<class T>
void Vector<T>::reserve(const size_type reservationSize)
{
    if(reservationSize <= capacity())
        return;

    /* A bigger place needed for current and incoming elements */
    cap = reservationSize;
    value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    copyRangeForward(begin(), end(), newData);  // Copy construct elements at their new place

    // Destroy old resource
    destroyRange(begin(), end());
    destroyPointer(data);

    data = newData;
}

template<class T>
void Vector<T>::shrink_to_fit()
{
    if(size() == capacity())
        return;

    cap = size();
    value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    copyRangeForward(begin(), end(), newData);

    destroyRange(begin(), end());
    destroyPointer(data);

    data = newData;
}

template<class T>
template<class InputIterator>
void Vector<T>::assignRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        *destination = *from;
}

template<class T>
void Vector<T>::assignRangeForward(iterator from, iterator to, const value_type& value)
{
    for( ; from != to; ++from)
        *from = value;
}

template<class T>
template<class InputIterator>
void Vector<T>::assignRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward assigning will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        *destination = *(to - 1);   // Copy assign element
}

template<class T>
template<class InputIterator>
void Vector<T>::moveRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        new(destination) value_type(std::move(*from));
}

template<class T>
void Vector<T>::moveRangeForward(iterator from, iterator to, const value_type& value)
{
    for( ; from != to; ++from)
        *from = std::move(value);
}

template<class T>
template<class InputIterator>
void Vector<T>::moveRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward copying will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        new(destination) value_type(std::move(*(to - 1)));
}

template<class T>
template<class InputIterator>
void Vector<T>::copyRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        new(destination) value_type(*from);
}

template<class T>
void Vector<T>::copyRangeForward(iterator from, iterator to, const value_type& value)
{
    for( ; from != to; ++from)
        *from = value;
}

template<class T>
template<class InputIterator>
void Vector<T>::copyRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward copying will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        new(destination) value_type(*(to - 1));
}


template<class T>
void Vector<T>::destroyRange(iterator from, iterator to)
{
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for( ; from != to; ++from)
        from->~value_type();    // Explicitly call the destructor
}

template<class T>
void Vector<T>::destroyPointer(iterator ptr)
{
    /* The allocated space will not be used anymore.
     * We shall release the resource for further usage */
    ::operator delete(static_cast<void*>(ptr));
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const Vector<T>& vector)
{
    for(const T& element : vector)
        stream << element << " ";

    return stream;
}

#endif
