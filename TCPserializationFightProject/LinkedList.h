#pragma once
#include <stdlib.h>
template <typename T>
class CList
{
public:
    struct Node
    {
        T _data;
        Node* _Prev;
        Node* _Next;
    };

    class iterator
    {
    private:
        Node* _node;
    public:
        iterator(Node* node = nullptr)
        {
            _node = node;
        }

        /*iterator operator ++(int)
        {

        }
        */
        iterator& operator++()
        {
            _node = _node->_Next;
            return *this;
        }
        /*
        iterator operator --(int)
        {

        }
        */
        iterator& operator--()
        {
            _node = _node->_Prev;
            return *this;
        }

        T& operator *()
        {
            return _node->_data;
        }
        bool operator ==(const iterator& other)
        {
            return _node == other._node;
        }
        bool operator !=(const iterator& other)
        {
            return _node != other._node;
        }

        friend iterator CList<T>::erase(iterator iter);


    };

public:
    CList()
    {
        _head._Next = nullptr;
        _head._Prev = nullptr;

        _tail._Next = nullptr;
        _tail._Prev = nullptr;
    }
    ~CList()
    {

    }

    iterator begin()
    {
        return iterator(_head._Next);
    }

    iterator end()
    {
        // return iterator(nullptr) 했다가는 , 
        // 그 .. tail이 포인터로 안되어있어서 접근하고 다음에 null 이라 그때 끝남
        return iterator(&_tail);
    }


    void push_front(T data)
    {
        if (size() == 0)
        {
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->_data = data;
            _head._Next = new_node;
            new_node->_Prev = &_head;
            new_node->_Next = &_tail;
            _tail._Prev = new_node;
            ++_size;
        }
        else
        {
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->_data = data;
            new_node->_Prev = &_head;
            new_node->_Next = _head._Next;
            _head._Next->_Prev = new_node;
            _head._Next = new_node;
            ++_size;
        }
    }
    void push_back(T data)
    {
        if (size() == 0)
        {
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->_data = data;
            _head._Next = new_node;
            new_node->_Prev = &_head;
            new_node->_Next = &_tail;
            _tail._Prev = new_node;
            ++_size;
        }
        else
        {
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->_data = data;
            new_node->_Next = &_tail;
            new_node->_Prev = _tail._Prev;
            _tail._Prev->_Next = new_node;
            _tail._Prev = new_node;
            ++_size;
        }
    }

    void pop_front()
    {

    }
    void pop_back()
    {

    }
    void clear()
    {
        CList<T>::iterator iter;
        iter = begin();
        while (iter != nullptr)
        {
            iter = erase(iter);

            if (iter == end())
                break;
        }
        /* 안에 DATA있는데 clear 시켜버리면 누적데이터 생김 고칠 것*/
        /*_head._Next = &_tail;
        _tail.Prev = &_head;*/
    }
    int size() { return _size; };
    bool empty() {
        if (_size == 0)
            return true;
        else
            return false;
    };

    // erase(iter) 용
    iterator erase(iterator iter)
    {
        iter._node->_Prev->_Next = iter._node->_Next; // iter를 가르키고 있음.
        iter._node->_Next->_Prev = iter._node->_Prev; // iter를 가르키고 있음

        Node* para = iter._node->_Next;

        delete* iter;
        --_size;
        return(para); // 다음노드 반환
    }

    // erase(*iter) 용
    iterator erase(T Data)
    {
        CList<T>::iterator iter;
        for (iter = _head._Next; iter != &_tail; ++iter)
        {
            if (*iter == Data)
            {
                return erase(iter);
            }
        }
    }

    void remove(T Data)
    {
        CList<T>::iterator iter;
        for (iter = _head._Next; iter != &_tail; ++iter)
        {
            if (*iter == Data)
            {
                iter = erase(iter);
            }
        }
    }

private:
    int _size = 0;
    Node _head;
    Node _tail;
};