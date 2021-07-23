/**
 * @file        QueueContainer.h
 * @details     A template queue container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 6, 2021  -> First release
 *              July 8, 2021  -> Copy and move constructors added.
 *              July 9, 2021  -> Doxygen added.
 *              July 23, 2021 -> Helper functions enhanced.
 * @note        Feel free to contact for questions, bugs or any other thing.
 * @copyright   No copyright.
 */

/*** Recursive inclusion preventer ***/
#pragma once

/*** Libraries ***/
#include <memory>       // std::allocator, std::allocator_traits
#include <cstring>      // std::memcpy
#include <algorithm>    // std::swap
#include <cstddef>      // std::size_t

/*** Special definitions ***/
// If the C++ version is greater or equal to 2017xx
#if __cplusplus >= 201703l
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, std::size_t C_SIZE = 128, class Allocator = std::allocator<T>>
class Queue{
public:
    /*** C++ Standard Named Requirements for Containers ***/
    using value_type        = T;
    using reference         = T&;
    using const_reference   = const T&;
    using iterator          = T*;
    using const_iterator    = const T*;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using allocator_type    = Allocator;

    /*** Constructors and Destructor ***/
    // Default constructor
    Queue();

    // Allocator Constructor
    explicit Queue(const Allocator& alloc);

    // Copy Constructor
    Queue(const Queue& copyQ);

    // Move Constructor
    Queue(Queue&& moveQ);

    // Destructor
    ~Queue();

    /*** Element Access ***/
    NODISCARD const_reference front() const;
    NODISCARD reference       front();
    NODISCARD const_reference back() const;
    NODISCARD reference       back();

    /*** Modifiers ***/
    template <class... Args>
    void emplace(Args&&... args);
    void push(const value_type& value);
    void push(value_type&& value);
    void pop();
    void swap(Queue& swapQ) noexcept;

    /*** Status Checkers ***/
    NODISCARD bool        empty() const { return (0 == sz); }
    NODISCARD size_type   size()  const { return sz;        }

private:
    /*** Members ***/
    size_type       sz;              // General size
    size_type       idxInFrontChunk; // Front index in the front chunk (index of the oldest element)
    size_type       idxInBackChunk;  // Back index in the back chunk (index 'after' the last added element)
    size_type       numOfChunks;     // Number of chunks (0 is the front)
    value_type**    chunks;          // Pointers of discrete chunks
    Allocator       allocator;       // Allocator policy

    /*** Helper Functions ***/
    NODISCARD value_type* backChunk();
    NODISCARD value_type* frontChunk();
    NODISCARD bool isFrontChunkConsumed() const { return (C_SIZE == idxInFrontChunk); }
    NODISCARD bool isNewChunkNeeded()     const { return ((C_SIZE == idxInBackChunk) || (0 == numOfChunks)); }
    void createNewChunk();
    void removeFrontChunk();
};

/**
 * @brief Default constructor
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue()
    : sz(0), idxInFrontChunk(0), idxInBackChunk(0), numOfChunks(0), chunks(nullptr)
{ /* No operation */ }

/**
 * @brief Allocator constructor
 * @param alloc Allocator object
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue(const Allocator& alloc)
    : sz(0), idxInFrontChunk(0), idxInBackChunk(0), numOfChunks(0), chunks(nullptr), allocator(alloc)
{ /* No operation */ }

/**
 * @brief   Copy constructor
 * @param   copyQ Source Queue for copying
 * @throws  std::logic_error    If the source queue was in an inconsistent state.
 * @throws  std::runtime_error  If the allocator fails to allocate chunks
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue(const Queue& copyQ)
    : sz(0), idxInFrontChunk(0), idxInBackChunk(0), numOfChunks(0), chunks(nullptr), allocator(copyQ.allocator)
{
    // Check if the source Queue is empty
    if(!copyQ.empty())
    {
        if((0 == copyQ.numOfChunks) || (nullptr == copyQ.chunks))
            throw std::logic_error("Source Queue is corrupted!");

        // Allocate space for chunk pointers
        numOfChunks = ((copyQ.sz-1) / C_SIZE) + 1;
        chunks      = new value_type*[numOfChunks];

        if(nullptr == chunks)
            throw std::runtime_error("Cannot allocate chunks array!");

        // Allocate space for chunks without constructing the objects
        for(size_type idx = 0; idx < numOfChunks; ++idx)
        {
            chunks[idx] = std::allocator_traits<Allocator>::allocate(allocator, C_SIZE);

            if(nullptr == chunks[idx])
                throw std::runtime_error("Cannot allocate chunks!");
        }

        // Copy construct objects
        size_type sourceChunk = 0, destChunk = 0;
        size_type sourceElemIdx = copyQ.idxInFrontChunk, destElemIdx = 0;
        for( ; sz < copyQ.sz; ++sz)
        {
            std::allocator_traits<Allocator>::construct(allocator, chunks[destChunk] + destElemIdx++, copyQ.chunks[sourceChunk][sourceElemIdx++]);

            if(C_SIZE == sourceElemIdx)     // Skip to the next chunk of source?
            {
                sourceElemIdx = 0;
                ++sourceChunk;
            }

            if(C_SIZE == destElemIdx)       // Skip to the next chunk of destination?
            {
                destElemIdx = 0;
                ++destChunk;
            }
        }

        // Adjust indexes
        idxInBackChunk  = destElemIdx;
        idxInFrontChunk = 0;
    }
}

/**
 * @brief Move constructor
 * @param moveQ     Source queue for stealing resources
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue(Queue&& moveQ)
    : sz(0), idxInFrontChunk(0), idxInBackChunk(0), numOfChunks(0), chunks(nullptr), allocator(moveQ.allocator)
{
    // Swap all members
    swap(moveQ);
}

/**
 * @brief Destructor
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::~Queue()
{
    if(!empty())
    {
        if(1 == numOfChunks)    // Is front chunk and back chunk is the same?
        {
            // Destroy each element at that single chunk
            for(size_type idx = idxInFrontChunk; idx < idxInBackChunk; ++idx)
                std::allocator_traits<Allocator>::destroy(allocator, frontChunk() + idx);
        }
        else if(1 < numOfChunks)
        {
            // Destroy each element at the front chunk
            for(; idxInFrontChunk < C_SIZE; ++idxInFrontChunk)  // Traverse elements
                std::allocator_traits<Allocator>::destroy(allocator, frontChunk() + idxInFrontChunk);

            // Destroy all elements of other chunks except the back one
            for(size_type chunkIdx = 1; chunkIdx < (numOfChunks - 2); ++chunkIdx)    // Traverse chunks
            {
                for(size_type elemIdx = 0; elemIdx < C_SIZE; ++elemIdx)   // Traverse elements
                    std::allocator_traits<Allocator>::destroy(allocator, chunks[chunkIdx] + elemIdx);
            }

            // Destroy each element at back chunk
            for(size_type elemIdx = 0; elemIdx < idxInBackChunk; ++elemIdx)   // Traverse elements
                std::allocator_traits<Allocator>::destroy(allocator, backChunk() + elemIdx);
        }

        // Destroy each chunk
        for(size_type chunkIdx = 0; chunkIdx < numOfChunks; ++chunkIdx)
            std::allocator_traits<Allocator>::deallocate(allocator, chunks[chunkIdx], C_SIZE);
    }

    // Destroy chunk pointer array
    delete [] chunks;
}

/**
 * @brief   Returns a const reference to the front element of Queue
 * @return  Constant lValue reference to the front element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
const T& Queue<T, C_SIZE, Allocator>::front() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return frontChunk()[idxInFrontChunk];
}

/**
 * @brief   Returns a reference to the front element of Queue
 * @return  lValue reference to the front element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
T& Queue<T, C_SIZE, Allocator>::front()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return frontChunk()[idxInFrontChunk];
}

/**
 * @brief   Returns a const reference to the back element of Queue
 * @return  Constant lValue reference to the back element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
const T& Queue<T, C_SIZE, Allocator>::back() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return backChunk()[idxInBackChunk-1];
}

/**
 * @brief   Returns a reference to the back element of Queue
 * @return  lValue reference to the back element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
T& Queue<T, C_SIZE, Allocator>::back()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return backChunk()[idxInBackChunk-1];
}

/**
 * @brief   Pushes the element by constructing it in-place with the given arguments
 * @param   args  Arguments to be forwarded to the constructor of the new element
 * @throws  std::logic_error    If the source queue was in an inconsistent state.
 */
template<class T, std::size_t C_SIZE, class Allocator>
template <class... Args>
void Queue<T, C_SIZE, Allocator>::emplace(Args&&... args)
{
    if(isNewChunkNeeded() == true)
    {
        createNewChunk();
    }

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == idxInBackChunk))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, backChunk() + idxInBackChunk, std::forward<Args>(args)...);

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value Constant lValue reference to the object to be pushed
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::push(const value_type& value)
{
    if(isNewChunkNeeded())
    {
        createNewChunk();
    }

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == idxInBackChunk))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, backChunk() + idxInBackChunk, value);

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value rValue reference to the object to be pushed
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::push(value_type&& value)
{
    return emplace(std::move(value));
}

/**
 * @brief Pops the front element of the Queue
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::pop()
{
    if(empty()) // Nothing to pop
        return;

    // Destroy the front element
    std::allocator_traits<Allocator>::destroy(allocator, frontChunk() + idxInFrontChunk);

    // Adjust size variables
    ++idxInFrontChunk;
    --sz;

    if(isFrontChunkConsumed())
    {
        removeFrontChunk();
    }
}

/**
 * @brief Swaps the content of two Queues
 * @param swapQ     Queue to be swapped with
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::swap(Queue& swapQ) noexcept
{
    // Swap all members
    std::swap(sz,               swapQ.sz             );
    std::swap(idxInBackChunk,   swapQ.idxInBackChunk );
    std::swap(idxInFrontChunk,  swapQ.idxInFrontChunk);
    std::swap(numOfChunks,      swapQ.numOfChunks    );
    std::swap(chunks,           swapQ.chunks         );
    std::swap(allocator,        swapQ.allocator      );
}

template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD T* Queue<T, C_SIZE, Allocator>::backChunk()
{
    if(0 == numOfChunks)
        throw;

    return chunks[numOfChunks-1];
}

template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD T* Queue<T, C_SIZE, Allocator>::frontChunk()
{
    if(0 == numOfChunks)
        throw;

    return chunks[0];
}

/**
 * @brief   Creates a new chunk when the size is about surpass the capacity of the back chunk
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 * @throws  std::runtime_error  If the allocator fails to allocate chunks
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::createNewChunk()
{
    // Check the status of container
    // This function should only be called when there is need for a new chunk
    if(!isNewChunkNeeded())
        throw std::logic_error("Early call to chunk creator!");

    // Allocate space for the new chunk
    value_type* newChunk = std::allocator_traits<Allocator>::allocate(allocator, C_SIZE);

    // Allocate space for chunk pointers
    value_type** newChunks = new value_type*[numOfChunks + 1];

    if((nullptr == newChunks) || (nullptr == newChunk))
        throw std::runtime_error("Cannot allocate space for the new chunk!");

    // Move the current chunk pointers
    if(0 < numOfChunks)
    {
        if(nullptr == chunks)
            throw std::logic_error("Chunks lost!");

        std::memcpy(newChunks, chunks, numOfChunks * sizeof(value_type*));   // Copy pointers directly

        // Deallocate old chunk array
        delete [] chunks;
    }

    // Append the new chunk
    newChunks[numOfChunks] = newChunk;

    // Change chunk array with the new one
    chunks = newChunks;

    // Adjust current chunk variables
    ++numOfChunks;
    idxInBackChunk = 0;
}

/**
 * @brief   Removes the front chunk when it has no more elements to fetch
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::removeFrontChunk()
{
    if(isFrontChunkConsumed() == false)
        throw std::logic_error("Front chunks is not consumed yet!");

    if((nullptr == chunks) || (0 == numOfChunks))
        throw std::logic_error("Cannot remove a non-exist chunk!");

    // No need to remove the chunk if there is only one left, just reset indexes
    if(1 == numOfChunks)
    {
        if((C_SIZE != idxInBackChunk) || (C_SIZE != idxInFrontChunk) || (0 != sz))
            throw std::logic_error("Indexes are corrupted!");

        idxInFrontChunk = 0;
        idxInBackChunk = 0;

        return;
    }

    // Deallocate front chunk
    std::allocator_traits<Allocator>::deallocate(allocator, frontChunk(), C_SIZE);

    /* There is no need to allocate new space for chunk pointers
     * as the currently allocated area will be enough to contain
     * the cropped version of previous pointer array */

    // Move chunk pointers backwards
    for(size_type idx = 0; idx < (numOfChunks-1); ++idx)
        chunks[idx] = chunks[idx+1];

    --numOfChunks;  // Decrement the number of chunks

    idxInFrontChunk = 0;    // Reset front index
}
