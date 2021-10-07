// A module of Dlib v1.4.1.
// Written by Keith Pomakis during the summer of 1994.
// Released to the public domain on October 10, 1994.
// This version released on May 12, 1996.

#ifndef QUEUE_DEFINED
#define QUEUE_DEFINED

#include <stdlib.h>
#include "basic.h"
#include "list.h"

// Assumes Element has a default constructor and operator=().

template <class Element>
class Queue {
    public:
        Queue();
        Queue(const Queue<Element> &queue);
        Queue(const List<Element> &list);
        Queue(const Element &element);
        ~Queue();
        operator List<Element>() const;
        Queue<Element> &operator=(const Queue<Element> &queue);
        Queue<Element> &operator=(const Element &element);
        Queue<Element> &enqueue(const Element &element);
        Element dequeue();
        Element &head();
        const Element &head() const;
        Element &tail();
        const Element &tail() const;
        int size() const;
        bool is_empty() const;
        Queue<Element> &clear();
    protected:
        static void queue_empty_error(const char *fname);
        List<Element> my_list;
};

/****************************************************************************/

template <class Element>
inline
Queue<Element>::Queue(): my_list()
{ /* do nothing */ }

/****************************************************************************/

template <class Element>
inline
Queue<Element>::Queue(const Queue<Element> &queue): my_list(queue.my_list)
{ /* do nothing */ }

/****************************************************************************/

template <class Element>
inline
Queue<Element>::Queue(const List<Element> &list): my_list(list)
{ /* do nothing */ }

/****************************************************************************/

template <class Element>
inline
Queue<Element>::Queue(const Element &element): my_list(element)
{ /* do nothing */ }

/****************************************************************************/

template <class Element>
inline
Queue<Element>::~Queue()
{
    my_list.clear();
}

/****************************************************************************/

//template <class Element>
//inline
//Queue<Element>::operator List<Element>() const
//{
//    return my_list;
//}

/****************************************************************************/

template <class Element>
inline Queue<Element> &
Queue<Element>::operator=(const Queue<Element> &queue)
{
    my_list = queue.my_list;
    return *this;
}

/****************************************************************************/

template <class Element>
inline Queue<Element> &
Queue<Element>::operator=(const Element &element)
{
    my_list = element;
    return *this;
}

/****************************************************************************/

template <class Element>
inline Queue<Element> &
Queue<Element>::enqueue(const Element &element)
{
    my_list.add_to_tail(element);
    return *this;
}

/****************************************************************************/

template <class Element>
inline Element
Queue<Element>::dequeue()
{
    if (my_list.size() < 1)
        queue_empty_error("dequeue()");
    
    Element element = my_list.head();
    my_list.remove_head();
    return element;
}

/****************************************************************************/

template <class Element>
inline Element &
Queue<Element>::head()
{
    if (my_list.size() < 1)
        queue_empty_error("head()");

    return my_list.head();
}

/****************************************************************************/

template <class Element>
inline const Element &
Queue<Element>::head() const
{
    if (my_list.size() < 1)
        queue_empty_error("head()");

    return my_list.head();
}

/****************************************************************************/

template <class Element>
inline Element &
Queue<Element>::tail()
{
    if (my_list.size() < 1)
        queue_empty_error("tail()");

    return my_list.tail();
}

/****************************************************************************/

template <class Element>
inline const Element &
Queue<Element>::tail() const
{
    if (my_list.size() < 1)
        queue_empty_error("tail()");

    return my_list.tail();
}

/****************************************************************************/

template <class Element>
inline int
Queue<Element>::size() const
{
    return my_list.size();
}

/****************************************************************************/

template <class Element>
inline bool
Queue<Element>::is_empty() const
{
    return my_list.is_empty();
}

/****************************************************************************/

template <class Element>
inline Queue<Element> &
Queue<Element>::clear()
{
    my_list.clear();
    return *this;
}

/****************************************************************************/

template <class Element>
void
Queue<Element>::queue_empty_error(const char *fname)
{
    cerr << "Queue: " << fname << " - queue empty\n";
    exit(EXIT_FAILURE);
}

/****************************************************************************/

#endif /* D_QUEUE_DEFINED */
