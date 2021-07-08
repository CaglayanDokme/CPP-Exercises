/**
 * @file        QueueContainer.h
 * @details     A template queue container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 6, 2021 -> First release
 *              July 8, 2021 -> Copy constructor added.
 * @note        Feel free to contact for questions, bugs or any other thing.
 * @copyright   No copyright.
 */

/*** Recursive inclusion preventer ***/
#pragma once

/*** Libraries ***/
#include <memory>       // std::allocator, std::allocator_traits
#include <cstring>      // std::memcpy
#include <algorithm>    // std::swap

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

    // Destructor
    ~Queue();

    /*** Element Access ***/
    NODISCARD const_reference front() const;
    NODISCARD reference       front();
    NODISCARD const_reference back() const;
    NODISCARD reference       back();

    /*** Modifiers ***/
    void push(const value_type& value);
    void push(value_type&& val);

    template <class... Args>
    void emplace(Args&&... args);
    void pop();
    void swap(Queue& x) noexcept;

    /*** Status Checkers ***/
    NODISCARD bool        empty() const { return (0 == sz);   }
    NODISCARD size_type   size()  const { return sz;          }

private:
    /*** Members ***/
    size_type       sz;                 // General size
    size_type       idxInFrontChunk;    // Front index in the front chunk (index of the least recently added element)
    size_type       idxInBackChunk;     // Back index in the back chunk (index after the last added element)
    size_type       numOfChunks;        // Number of chunks (0 is the front)
    value_type**    chunks;             // Pointers of discrete chunks
    Allocator       allocator;          // Allocator policy

    /*** Helper Functions ***/
    NODISCARD bool isFrontChunkConsumed() const { return (C_SIZE == idxInFrontChunk); }
    NODISCARD bool isNewChunkNeeded()     const { return ((C_SIZE == idxInBackChunk) || (0 == numOfChunks)); }
    void createNewChunk();
    void removeFrontChunk();
};

template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue()
    : sz(0), idxInFrontChunk(0), idxInBackChunk(0), numOfChunks(0), chunks(nullptr)
{ /* No operation */ }

template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue(const Allocator& alloc)
    : sz(0), idxInFrontChunk(0), idxInBackChunk(0), numOfChunks(0), chunks(nullptr), allocator(alloc)
{ /* No operation */ }

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
        chunks = new value_type*[copyQ.numOfChunks];

        if(nullptr == chunks)
            throw std::runtime_error("Cannot allocate chunks array!");

        numOfChunks = copyQ.numOfChunks;

        // Allocate new chunks without constructing objects
        for(size_type chunkIdx = 0; chunkIdx < numOfChunks; ++chunkIdx)
        {
            // Allocate space for the new chunk
            chunks[chunkIdx] = std::allocator_traits<Allocator>::allocate(allocator, C_SIZE);

            if(nullptr == chunks[chunkIdx])
                throw std::runtime_error("Cannot allocate a new chunk!");
        }

        // Copy current objects to new chunks
        for(size_type chunkIdx = 0; chunkIdx < numOfChunks; ++chunkIdx) // Traverse chunks
        {
            // Copy the front chunk
            if(0 == chunkIdx)
            {
                // If the front chunk is the only chunk, then it may have missing elements
                if(1 == copyQ.numOfChunks)
                    for(size_type elemIdx = copyQ.idxInFrontChunk; elemIdx < idxInBackChunk; ++elemIdx)
                        std::allocator_traits<Allocator>::construct(allocator, chunks[chunkIdx] + elemIdx, copyQ.chunks[chunkIdx][elemIdx]);
                else
                    for(size_type elemIdx = copyQ.idxInFrontChunk; elemIdx < C_SIZE; ++elemIdx)
                        std::allocator_traits<Allocator>::construct(allocator, chunks[chunkIdx] + elemIdx, copyQ.chunks[chunkIdx][elemIdx]);
            }
            else if(chunkIdx < numOfChunks-1)
            {
                for(size_type elemIdx = 0; elemIdx < C_SIZE; ++elemIdx)
                    std::allocator_traits<Allocator>::construct(allocator, chunks[chunkIdx] + elemIdx, copyQ.chunks[chunkIdx][elemIdx]);
            }
            else    // Back chunk
            {
                // Back chunk may have missing elements as it is not completely used yet
                for(size_type elemIdx = 0; elemIdx < copyQ.idxInBackChunk; ++elemIdx)
                    std::allocator_traits<Allocator>::construct(allocator, chunks[chunkIdx] + elemIdx, copyQ.chunks[chunkIdx][elemIdx]);
            }
        }

        idxInBackChunk  = copyQ.idxInBackChunk;
        idxInFrontChunk = copyQ.idxInFrontChunk;
        numOfChunks     = copyQ.numOfChunks;
        sz              = copyQ.sz;
    }
}

template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::~Queue()
{
    if(!empty())
    {
        if(1 == numOfChunks)
        {
            // Destroy each element at front chunk(Front chunk is the only chunk in queue)
            for(size_type idx = idxInFrontChunk; idx < idxInBackChunk; ++idx)
                std::allocator_traits<Allocator>::destroy(allocator, chunks[0] + idx);
        }
        else if(1 < numOfChunks)
        {
            // Destroy each element at front chunk
            for(; idxInFrontChunk < C_SIZE; ++idxInFrontChunk)
                std::allocator_traits<Allocator>::destroy(allocator, chunks[0] + idxInFrontChunk);

            // Destroy all elements of other chunks except the back one
            for(size_type chunkIdx = 1; chunkIdx < (numOfChunks - 2); ++chunkIdx)    // Traverse chunks
            {
                for(size_type elemIdx = 0; elemIdx < C_SIZE; ++elemIdx)   // Traverse elements
                    std::allocator_traits<Allocator>::destroy(allocator, chunks[chunkIdx] + elemIdx);
            }

            // Destroy each element at back chunk
            for(size_type elemIdx = 0; elemIdx < idxInBackChunk; ++elemIdx)   // Traverse elements
                std::allocator_traits<Allocator>::destroy(allocator, chunks[numOfChunks-1] + elemIdx);
        }

        // Destroy each chunk
        for(size_type chunkIdx = 0; chunkIdx < numOfChunks; ++chunkIdx)
            std::allocator_traits<Allocator>::deallocate(allocator, chunks[chunkIdx], C_SIZE);
    }

    // Destroy chunk pointer array
    delete [] chunks;
}

template<class T, std::size_t C_SIZE, class Allocator>
const T& Queue<T, C_SIZE, Allocator>::front() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return chunks[0][idxInFrontChunk];
}

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
    std::allocator_traits<Allocator>::construct(allocator, chunks[numOfChunks-1] + idxInBackChunk, std::forward<Args>(args)...);

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

template<class T, std::size_t C_SIZE, class Allocator>
T& Queue<T, C_SIZE, Allocator>::front()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return chunks[0][idxInFrontChunk];
}

template<class T, std::size_t C_SIZE, class Allocator>
const T& Queue<T, C_SIZE, Allocator>::back() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    if(idxInBackChunk == 0)
        return chunks[numOfChunks-2][C_SIZE-1];
    else
        return chunks[numOfChunks-1][idxInBackChunk-1];
}

template<class T, std::size_t C_SIZE, class Allocator>
T& Queue<T, C_SIZE, Allocator>::back()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    if(idxInBackChunk == 0)
        return chunks[numOfChunks-2][C_SIZE-1];
    else
        return chunks[numOfChunks-1][idxInBackChunk-1];
}

template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::push(const value_type& value)
{
    if(isNewChunkNeeded() == true)
    {
        createNewChunk();
    }

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == idxInBackChunk))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, chunks[numOfChunks-1] + idxInBackChunk, value);

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::push(value_type&& value)
{
    if(isNewChunkNeeded() == true)
    {
        createNewChunk();
    }

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == idxInBackChunk))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, chunks[numOfChunks-1] + idxInBackChunk, std::move(value));

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::pop()
{
    if(0 == sz) // Nothing to pop
        return;

    // Destroy the front element
    std::allocator_traits<Allocator>::destroy(allocator, chunks[0] + idxInFrontChunk);

    // Adjust size variables
    ++idxInFrontChunk;
    --sz;

    if(isFrontChunkConsumed() == true)
    {
        removeFrontChunk();
    }
}

template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::swap(Queue& x) noexcept
{
    // Swap all members
    std::swap(sz,               x.sz                );
    std::swap(idxInBackChunk,   x.idxInBackChunk    );
    std::swap(idxInFrontChunk,  x.idxInFrontChunk   );
    std::swap(numOfChunks,      x.numOfChunks       );
    std::swap(chunks,           x.chunks            );
    std::swap(allocator,        x.allocator         );
}

template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::createNewChunk()
{
    // Check the status of container
    // This function should only be called when there is need for a new chunk
    if(isNewChunkNeeded() == false)
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
    std::allocator_traits<Allocator>::deallocate(allocator, chunks[0], C_SIZE);

    /* There is no need to allocate new space for chunk pointers
     * as the currently allocated area will be enough to contain
     * the cropped version of previous pointer array */

    // Move chunk pointers backwards
    for(size_type idx = 0; idx < (numOfChunks-1); ++idx)
        chunks[idx] = chunks[idx+1];

    --numOfChunks;  // Decrement the number of chunks

    idxInFrontChunk = 0;    // Reset front index
}
