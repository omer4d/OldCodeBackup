/* 
 * File:   List.hpp
 * Author: Stas
 *
 * Created on February 26, 2013, 11:26 AM
 */

#ifndef LIST_HPP
#define	LIST_HPP

template <class T>
class List {
public:

    class Node {
        friend class List;
        
    public:
        T data;
    private:
        Node* prev;
        Node* next;
        
        Node(T const& data, Node* prev = NULL, Node* next = NULL)
        {
            this->data = data;
            this->prev = prev;
            this->next = next;
        }

    public:

        Node const* getNext() const
        {
            return next;
        }

        Node const* getPrev() const
        {
            return prev;
        }
        
        Node* getNext()
        {
            return next;
        }

        Node* getPrev()
        {
            return prev;
        }
    };

private:
    int size;
    Node* first;
    Node* last;

public:
    
    List()
    {
        size = 0;
        first = NULL;
        last = NULL;
    }
    
    int getSize() const
    {
        return size;
    }
    
    Node* getFirst()
    {
        return first;
    }
    
    Node* getLast()
    {
        return last;
    }
    
    Node const* getFirst() const
    {
        return first;
    }
    
    Node const* getLast() const
    {
        return last;
    }
    
    Node* pushFront(T const& data)
    {
        if(!size)
        {
            first = new Node(data, NULL, NULL);
            last = first;
        }

        else
        {
            Node* n = new Node(data, NULL, first);
            first->prev = n;
            first = n;
        }

        size++;

        return first;
    }

    Node* pushBack(T const& data)
    {
        if(!size)
        {
            first = new Node(data, NULL, NULL);
            last = first;
        }

        else
        {
            Node* n = new Node(data, last, NULL);
            last->next = n;
            last = n;
        }

        size++;

        return last;
    }

    void popFront()
    {
        if(size == 1)
        {
            delete first;
            first = NULL;
            last = NULL;
        }

        else
        {
            Node* oldFirst = first;
            first = oldFirst->next;
            first->prev = NULL;
            delete oldFirst;
        }

        size--;
    }

    void popBack()
    {
        if(size == 1)
        {
            delete first;
            first = NULL;
            last = NULL;
        }

        else
        {
            Node *oldLast = last;
            last = oldLast->prev;
            last->next = NULL;
            delete oldLast;
        }

        size--;
    }

    Node* insertBefore(Node* node, T const& data)
    {
        Node* newNode = NULL;

        if(node == first)
            newNode = pushFront(data);
        else
        {
            newNode = new Node(data, node->prev, node);
            node->prev->next = newNode;
            node->prev = newNode;
            size++;
        }

        return newNode;
    }

    Node* insertAfter(Node* node, T const& data)
    {
        Node *newNode = NULL;

        if(node == last)
            newNode = pushBack(data);
        else
        {
            newNode = new Node(data, node, node->next);
            node->next->prev = newNode;
            node->next = newNode;
            size++;
        }

        return newNode;
    }

    Node* erase(Node* node)
    {
        Node* next = node->next;

        if(node == first)
            popFront();
        else if(node == last)
            popBack();
        else
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            delete node;
            size--;
        }

        return next;
    }
    
    void remove(T const& data)
    {
        for(Node* node = first; node != NULL; )
            if(node->data == data)
                node = erase(node);
            else
                node = node->next;
    }
};

#endif	/* LIST_HPP */

