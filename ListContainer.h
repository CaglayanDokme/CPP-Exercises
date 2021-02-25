/** @file       ListContainer.h
 *  @details    A template container class with an underlying
 *              doubly linked list structure.
 *  @author     Caglayan DOKME, caglayandokme@gmail.com
 *  @date       February 25, 2021 -> First release
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 */

#include <iostream>

// Forward declaration
template<class T> class ListNode;

template<class T>
class List{
public:
    List(); // Constructor

    virtual ~List();    // Destructor

    List<T>& Append(const T& data);     // Add after the last node
    List<T>& Prepend(const T& data);    // Add before the first node

    const T& First() const; // Get the first data as an rValue
    const T& Last() const;  // Get the last data as an rValue
    T& First(); // Get the first data as an lValue
    T& Last();  // Get the first data as an lValue

    List<T>& RemoveFirst(); // Remove the first node
    List<T>& RemoveLast();  // Remove the last node
    List<T>& EraseAll();    // Remove all elements
    List<T>& RemoveIf(const T& data); // Remove all samples of a specific data

    bool isEmpty() const
    { return (numberOfNodes == 0); }

    /* Declaring a function as a friend inside of a template class
       corrupts the template usage. You may want to check the holy StackOverflow :)
       stackoverflow.com/questions/4660123 */
    template<class _T>
    friend std::ostream& operator<<(std::ostream& stream, List<_T>& list);

    void PrintAll(std::ostream& stream);

    size_t GetNodeCount() const
    { return numberOfNodes; }

private:
    ListNode<T>* Find(const T& data, ListNode<T>* beginByNode);
    void RemoveNode(ListNode<T>* removingNode);

    ListNode<T>* firstPtr   = nullptr;  // First node of the list
    ListNode<T>* lastPtr    = nullptr;  // Last node of the list
    size_t numberOfNodes    = 0;        // Node count
};

template<class T>
class ListNode{
    friend class List<T>;

public:
    ListNode(const T& data)
        : data(data), prevPtr(nullptr), nextPtr(nullptr)
    { /* Empty constructor */ }

private:
    T data;
    ListNode* prevPtr = nullptr;
    ListNode* nextPtr = nullptr;
};

template<class T>
List<T>::List()
: firstPtr(nullptr), lastPtr(nullptr), numberOfNodes(0)
{ /* Empty constructor */ }

template<class T>
List<T>::~List()
{
    /* Destroy all nodes until there is no node left. */
    while(isEmpty() == false)
        RemoveFirst();
}

template<class T>
List<T>& List<T>::Append(const T& data)
{
    if(isEmpty() == true)  // If it is the first node
    {
        firstPtr    = new ListNode<T>(data);    // Create the first node
        lastPtr     = firstPtr; // The last and the first points the same node
    }
    else
    {
        lastPtr->nextPtr = new ListNode<T>(data);   // Create and append the node
        lastPtr->nextPtr->prevPtr = lastPtr;        // Adjust prevNode connection
        lastPtr = lastPtr->nextPtr;                 // Update the lastPtr
    }

    numberOfNodes++;    // Increase the number of nodes

    return *this;   // Support cascaded appends
}

template<class T>
List<T>& List<T>::Prepend(const T& data)
{
    if(isEmpty() == true)   // If it is the first node
    {
        firstPtr    = new ListNode<T>(data);    // Create the first node
        lastPtr     = firstPtr; // The last and the first points the same node
    }
    else
    {
        firstPtr->prevPtr = new ListNode<T>(data);  // Create and prepend the node
        firstPtr->prevPtr->nextPtr = firstPtr;      // Adjust nextNode connection
        firstPtr = firstPtr->prevPtr;               // Update the firstPtr
    }

    numberOfNodes++;    // Increase the number of nodes

    return *this;
}

template<class T>
const T& List<T>::First() const
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return firstPtr->data;
}

template<class T>
const T& List<T>::Last() const
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return lastPtr->data;
}

template<class T>
T& List<T>::First()
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return firstPtr->data;
}

template<class T>
T& List<T>::Last()
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return lastPtr->data;
}

template<class T>
List<T>& List<T>::RemoveFirst()
{
    if(isEmpty() == false)
    {
        ListNode<T>* tempPtr = firstPtr;    // Save removing node addresss
        firstPtr = firstPtr->nextPtr;       // Update firstPtr
        delete tempPtr;                     // Delete saved firstPtr
        numberOfNodes--;                    // Decrement node count

        if(firstPtr != nullptr)
            firstPtr->prevPtr = nullptr;    // Remove prevNode connection
    }

    return *this;   // Support cascaded remove calls
}

template<class T>
List<T>& List<T>::RemoveLast()
{
    if(isEmpty() == false)
    {
        ListNode<T>* tempPtr = lastPtr;     // Save removing node addresss
        lastPtr = lastPtr->prevPtr;         // Update lastPtr
        delete tempPtr;                     // Delete saved lastPtr
        numberOfNodes--;                    // Decrement node count

        if(lastPtr != nullptr)
            lastPtr->nextPtr = nullptr;    // Remove nextNode connection
    }

    return *this;   // Support cascaded remove calls
}

template<class T>
List<T>& List<T>::EraseAll()
{
    /* Remove all until the list is empty */
    while(isEmpty() == false)
        RemoveFirst();
}

template<class T>
List<T>& List<T>::RemoveIf(const T& data)
{
    ListNode<T>* removingNode;      // Node to be removed
    ListNode<T>* searchStartPoint;  // Node where the search will start

    /* Find and remove all specified nodes until
     * we hit the last of the list */
    removingNode = Find(data, firstPtr);    // Find the first sample
    while(removingNode != nullptr)
    {
        searchStartPoint = removingNode->nextPtr;       // Save the next node
        RemoveNode(removingNode);                       // Remove the node found
        removingNode = Find(data, searchStartPoint);    // Find the next removing node
    }

    return *this;
}

template<class T>
std::ostream& operator<<(std::ostream& stream, List<T>& list)
{
    if(list.isEmpty() == true)
        stream << "-- empty list --";
    else
        list.PrintAll(stream);

    return stream; // Support cascaded streams
}

template<class T>
void List<T>::PrintAll(std::ostream& stream)
{
    for(ListNode<T>* currentNode = firstPtr; currentNode != nullptr; currentNode = currentNode->nextPtr)
        stream << currentNode->data << " ";
}

template<class T>
ListNode<T>* List<T>::Find(const T& data, ListNode<T>* beginByNode)
{
    // Search begins by the given node
    ListNode<T>* currentNode = beginByNode;

    /* Break the search if the element is found
     * or the last element is hit */
    while(currentNode != nullptr)
    {
        if(currentNode->data == data)
            break;
        else
            currentNode = currentNode->nextPtr;
    }

    return currentNode;
}

template<class T>
void List<T>::RemoveNode(ListNode<T>* removingNode)
{
    if(removingNode == nullptr)         // Return if the node is not valids
        return;
    else if(removingNode == firstPtr)   // First and last pointers are special
        RemoveFirst();                  // as there is no previous or next
    else if(removingNode == lastPtr)    // pointers for nodes. This affects
        RemoveLast();                   // the reconnection strategy.
    else    // If it is a random node
    {
        // Connect previous and next nodes
        removingNode->nextPtr->prevPtr = removingNode->prevPtr;
        removingNode->prevPtr->nextPtr = removingNode->nextPtr;

        delete removingNode;    // Delete the node
        numberOfNodes--;        // Decrement node counter
    }
}
