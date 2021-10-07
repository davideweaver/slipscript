// A module of Dlib v1.4.1.
// Written by Keith Pomakis during the summer of 1994.
// Released to the public domain on October 10, 1994.
// This version released on May 12, 1996.

#ifndef PRIORITY_QUEUE_DEFINED
#define PRIORITY_QUEUE_DEFINED

#include <stdlib.h>
#include "basic.h"
#include "list.h"

// Assumes Element has a default constructor, operator=(), operator==(),
// and operator<().  Note that operator<() must place a total ordering on
// the set of Elements.

template <class Element>
class PriorityQueue {
    public:
        PriorityQueue();
        PriorityQueue(const PriorityQueue<Element> &queue);
        PriorityQueue(const DList<Element> &list);
        PriorityQueue(const Element &element);
        ~PriorityQueue();
        operator DList<Element>() const;
        PriorityQueue<Element> &
                            operator=(const PriorityQueue<Element> &queue);
        PriorityQueue<Element> &operator=(const Element &element);
        PriorityQueue<Element> &enqueue(const Element &element);
        Element dequeue();
        Element &head();
        const Element &head() const;
        Element &tail();
        const Element &tail() const;
        bool contains(const Element &element) const;
        int occurrences_of(const Element &element) const;
        int size() const;
        bool is_empty() const;
        PriorityQueue<Element> &clear();
    protected:
        static void queue_empty_error(const char *fname);
        DSortableList<Element> my_list;
};

/****************************************************************************/

template <class Element>
inline
PriorityQueue<Element>::PriorityQueue(): my_list()
{ /* do nothing. */ }

/****************************************************************************/

template <class Element>
inline
PriorityQueue<Element>::PriorityQueue(const PriorityQueue<Element>
                                            &queue): my_list(queue.my_list)
{ /* do nothing. */ }

/****************************************************************************/

template <class Element>
inline
PriorityQueue<Element>::PriorityQueue(const DList<Element> &list):
                                                                my_list(list)
{
    my_list.sort();
}

/****************************************************************************/

template <class Element>
inline
PriorityQueue<Element>::PriorityQueue(const Element &element):
                                                            my_list(element)
{ /* do nothing. */ }

/****************************************************************************/

template <class Element>
inline
PriorityQueue<Element>::~PriorityQueue()
{
    my_list.clear();
}

/****************************************************************************/

//template <class Element>
//inline
//PriorityQueue<Element>::operator List<Element>() const
//{
//    return my_list;
//}

/****************************************************************************/

template <class Element>
inline PriorityQueue<Element> &
PriorityQueue<Element>::operator=(const PriorityQueue<Element> &queue)
{
    my_list = queue.my_list;
    return *this;
}

/****************************************************************************/

template <class Element>
inline PriorityQueue<Element> &
PriorityQueue<Element>::operator=(const Element &element)
{
    my_list = element;
    return *this;
}

/****************************************************************************/

template <class Element>
PriorityQueue<Element> &
PriorityQueue<Element>::enqueue(const Element &element)
{
    DIterator<Element> iter(my_list);

    iter.end();
    while (iter.ptr() && element < *iter)
        iter--;
    if (!iter.ptr())
        my_list.add_to_head(element);
    else
        iter.insert_after_current(element);

    return *this;
}

/****************************************************************************/

template <class Element>
inline Element
PriorityQueue<Element>::dequeue()
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
PriorityQueue<Element>::head()
{
    if (my_list.size() < 1)
        queue_empty_error("head()");

    return my_list.head();
}

/****************************************************************************/

template <class Element>
inline const Element &
PriorityQueue<Element>::head() const
{
    if (my_list.size() < 1)
        queue_empty_error("head()");

    return my_list.head();
}

/****************************************************************************/

template <class Element>
inline Element &
PriorityQueue<Element>::tail()
{
    if (my_list.size() < 1)
        queue_empty_error("tail()");

    return my_list.tail();
}

/****************************************************************************/

template <class Element>
inline const Element &
PriorityQueue<Element>::tail() const
{
    if (my_list.size() < 1)
        queue_empty_error("tail()");

    return my_list.tail();
}

/****************************************************************************/

template <class Element>
bool
PriorityQueue<Element>::contains(const Element &element) const
{
    DReadOnlyIterator<Element> iter(my_list);
 
    while (iter.ptr() && *iter < element)
        iter++;
    return (iter.ptr() && *iter == element);
}

/****************************************************************************/

template <class Element>
int
PriorityQueue<Element>::occurrences_of(const Element &element) const
{
    DReadOnlyIterator<Element> iter(my_list);
    int n = 0;
 
    while (iter.ptr() && *iter < element)
        iter++;
    while (iter.ptr() && *iter == element) {
        n++;
        iter++;
    }

    return n;
}

/****************************************************************************/

template <class Element>
inline int
PriorityQueue<Element>::size() const
{
    return my_list.size();
}

/****************************************************************************/

template <class Element>
inline bool
PriorityQueue<Element>::is_empty() const
{
    return my_list.is_empty();
}

/****************************************************************************/

template <class Element>
inline PriorityQueue<Element> &
PriorityQueue<Element>::clear()
{
    my_list.clear();
}

/****************************************************************************/

template <class Element>
void
PriorityQueue<Element>::queue_empty_error(const char *fname)
{
    cerr << "PriorityQueue: " << fname << " - queue empty\n";
    exit(EXIT_FAILURE);
}

/****************************************************************************/

#endif /* D_PRIORITY_QUEUE_DEFINED */
