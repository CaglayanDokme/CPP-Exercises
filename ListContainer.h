/** Description :    A template container class with an underlying
 *                   doubly linked list structure.
 *  Author :         Caglayan DOKME, caglayandokme@gmail.com
 *  Date :           February 25, 2021 -> First release
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

    List<T>& RemoveFirst(); // Remove the first node
    List<T>& RemoveLast();  // Remove the last node

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
        delete tempPtr;                     // Delete saved firstPtr
        numberOfNodes--;                    // Decrement node count

        if(lastPtr != nullptr)
            lastPtr->nextPtr = nullptr;    // Remove nextNode connection
    }

    return *this;   // Support cascaded remove calls
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
    ListNode<T>* currentNode = firstPtr;

    for( ; currentNode != nullptr; currentNode = currentNode->nextPtr)
        stream << currentNode->data << " ";
}
