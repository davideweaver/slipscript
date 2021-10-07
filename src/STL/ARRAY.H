// array.h
#ifndef D_ARRAY_DEFINED
#define D_ARRAY_DEFINED

#include <iostream.h>
#include <stdlib.h>
#include "basic.h"

// Assumes Element has a default constructor and operator=().

template <class Element>
class Array {
    public:
        Array();
        Array(int size);
        Array(int size, const Element &init_element);
        Array(const Array<Element> &array);
        virtual ~Array();
        Array<Element> &operator=(const Array<Element> &array);
        Element &operator[](int index);
        const Element &operator[](int index) const;
        int size() const;
        int is_empty() const;
        void resize(int new_size);
        void resize(int new_size, const Element &init_element);
        void set_all_to(const Element &element);
    protected:
        static void error(const char *msg);
        int my_size;
        Element *my_elements;
};

// Assumes Element has the above plus operator==().

template <class Element>
class DComparableArray: public Array<Element> {
    public:
        DComparableArray();
        DComparableArray(int size);
        DComparableArray(int size, const Element &init_element);
        DComparableArray(const DComparableArray<Element> &array);
        DComparableArray(const Array<Element> &array);
        virtual ~DComparableArray();
        DComparableArray<Element> &operator=(const Array<Element> &array);
        int operator==(const DComparableArray<Element> &array) const;
        int operator!=(const DComparableArray<Element> &array) const;
        int contains(const Element &element) const;
        int occurrences_of(const Element &element) const;
        int index_of(const Element &element) const;
    protected:
        static void error(const char *msg);
};

// Assumes Element has the above plus operator<().  Note that this operator
// must place a total ordering on the set of Elements.

template <class Element>
class DSortableArray: public DComparableArray<Element> {
    public:
        DSortableArray();
        DSortableArray(int size);
        DSortableArray(int size, const Element &init_element);
        DSortableArray(const DSortableArray<Element> &array);
        DSortableArray(const Array<Element> &array);
        virtual ~DSortableArray();
        DSortableArray<Element> &operator=(const Array<Element> &array);
        void sort();
        Element &min();
        const Element &min() const;
        Element &max();
        const Element &max() const;
    protected:
        static void error(const char *msg);
        int findpivot(int i, int j);
        int partition(int l, int r, const Element &pivot);
        void quicksort(int i, int j);
};

/****************************************************************************/

template <class Element>
inline
Array<Element>::Array(): my_size(0)
{
    /* do nothing */
}

/****************************************************************************/

template <class Element>
Array<Element>::Array(int size)
{
    if (size < 0)
        error("Array() - size of Array must be non-negative");

    my_size = size;
    my_elements = new Element[size];
    check_mem(my_elements);
}

/****************************************************************************/

template <class Element>
Array<Element>::Array(int size, const Element &init_element)
{
    if (size < 0)
        error("Array() - size of Array must be non-negative");

    my_size = size;
    if (my_size > 0) {
        my_elements = new Element[size];
        check_mem(my_elements);
        for (int i=0; i<size; i++)
            my_elements[i] = init_element;
    }
}

/****************************************************************************/

template <class Element>
Array<Element>::Array(const Array<Element> &array)
{
    my_size = array.my_size;
    if (my_size > 0) {
        my_elements = new Element[my_size];
        check_mem(my_elements);
        for (int i=0; i<my_size; i++)
            my_elements[i] = array.my_elements[i];
    }
}

/****************************************************************************/

template <class Element>
inline
Array<Element>::~Array()
{
    if (my_size > 0)
        delete [] my_elements;
}

/****************************************************************************/

template <class Element>
Array<Element> &
Array<Element>::operator=(const Array<Element> &array)
{
    if (this == &array)
        return *this;

    int resize = (my_size != array.my_size);

    if (my_size > 0 && resize)
        delete [] my_elements;

    my_size = array.my_size;
    if (my_size > 0) {
        if (resize) {
            my_elements = new Element[my_size];
            check_mem(my_elements);
        }
        for (int i=0; i<my_size; i++)
            my_elements[i] = array.my_elements[i];
    }
    return *this;
}

/****************************************************************************/

template <class Element>
inline Element &
Array<Element>::operator[](int index)
{
    if (index < 0 || index >= my_size)
        error("operator[] - invalid index");

    return my_elements[index];
}

/****************************************************************************/

template <class Element>
inline const Element &
Array<Element>::operator[](int index) const
{
    if (index < 0 || index >= my_size)
        error("operator[] - invalid index");

    return my_elements[index];
}

/****************************************************************************/

template <class Element>
inline int
Array<Element>::size() const
{
    return my_size;
}

/****************************************************************************/

template <class Element>
inline int
Array<Element>::is_empty() const
{
    return (my_size == 0);
}

/****************************************************************************/

template <class Element>
void
Array<Element>::resize(int new_size)
{
    if (my_size == new_size)
        return;

    if (new_size < 0)
        error("resize() - size of Array must be non-negative");

    if (new_size > 0) {
        Element *new_space = new Element[new_size];
        check_mem(new_space);

        const int savable_elements = ::min(my_size, new_size);
        for (int i=0; i<savable_elements; i++)
            new_space[i] = my_elements[i];

        if (my_size > 0)
            delete [] my_elements;
        my_elements = new_space;
    }
    else
        delete [] my_elements;

    my_size = new_size;
}

/****************************************************************************/

template <class Element>
void
Array<Element>::resize(int new_size, const Element &init_element)
{
    if (my_size == new_size)
        return;

    if (new_size < 0)
        error("resize() - size of Array must be non-negative");

    if (new_size > 0) {
        Element *new_space = new Element[new_size];
        check_mem(new_space);

        int i;
        const int savable_elements = ::min(my_size, new_size);
        for (i=0; i<savable_elements; i++)
            new_space[i] = my_elements[i];

        if (my_size > 0)
            delete [] my_elements;
        my_elements = new_space;

        for (i=my_size; i<new_size; i++)
            my_elements[i] = init_element;
    }
    else
        delete [] my_elements;

    my_size = new_size;
}

/****************************************************************************/

template <class Element>
void
Array<Element>::set_all_to(const Element &element)
{
    for (int i=0; i<my_size; i++)
        my_elements[i] = element;
}

/****************************************************************************/

template <class Element>
void
Array<Element>::error(const char *msg)
{
    cerr << "Array: " << msg << '\n';
    exit(EXIT_FAILURE);
}

/****************************************************************************/

template <class Element>
inline
DComparableArray<Element>::DComparableArray(): Array<Element>()
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DComparableArray<Element>::DComparableArray(int size): Array<Element>(size)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DComparableArray<Element>::DComparableArray(int size,
            const Element &init_element): Array<Element>(size, init_element)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DComparableArray<Element>::DComparableArray(
            const DComparableArray<Element> &array): Array<Element>(array)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DComparableArray<Element>::DComparableArray(const Array<Element> &array):
                                                Array<Element>(array)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DComparableArray<Element>::~DComparableArray()
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline DComparableArray<Element> &
DComparableArray<Element>::operator=(const Array<Element> &array)
{
    Array<Element>::operator=(array);
    return *this;
}

/****************************************************************************/

template <class Element>
int
DComparableArray<Element>::operator==(const DComparableArray<Element> &array)
                                                                        const
{
    if (this == &array)
        return true;
    
    if (my_size != array.my_size)
        return false;

    for (int i=0; i<my_size; i++)
        if (!(my_elements[i] == array.my_elements[i]))
            return false;
    
    return true;
}

/****************************************************************************/

template <class Element>
inline int
DComparableArray<Element>::operator!=(const DComparableArray<Element> &array)
                                                                        const
{
    return !(*this == array);
}

/****************************************************************************/

template <class Element>
inline int
DComparableArray<Element>::contains(const Element &element) const
{
    return (index_of(element) != -1);
}

/****************************************************************************/

template <class Element>
int
DComparableArray<Element>::occurrences_of(const Element &element) const
{
    int occurrences = 0;

    for (int i=0; i<my_size; i++)
        if (my_elements[i] == element)
            occurrences++;

    return occurrences;
}

/****************************************************************************/

template <class Element>
int
DComparableArray<Element>::index_of(const Element &element) const
{
    for (int i=0; i<my_size; i++)
        if (my_elements[i] == element)
            return i;

    return -1;
}

/****************************************************************************/

template <class Element>
void
DComparableArray<Element>::error(const char *msg)
{
    cerr << "DComparableArray: " << msg << '\n';
    exit(EXIT_FAILURE);
}

/****************************************************************************/

template <class Element>
inline
DSortableArray<Element>::DSortableArray(): DComparableArray<Element>()
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DSortableArray<Element>::DSortableArray(int size):
                                            DComparableArray<Element>(size)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DSortableArray<Element>::DSortableArray(int size,
    const Element &init_element): DComparableArray<Element>(size, init_element)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DSortableArray<Element>::DSortableArray(const DSortableArray<Element>
                                    &array): DComparableArray<Element>(array)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DSortableArray<Element>::DSortableArray(const Array<Element> &array):
                                            DComparableArray<Element>(array)
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline
DSortableArray<Element>::~DSortableArray()
{ /* do nothing new. */ }

/****************************************************************************/

template <class Element>
inline DSortableArray<Element> &
DSortableArray<Element>::operator=(const Array<Element> &array)
{
    DComparableArray<Element>::operator=(array);
    return *this;
}

/****************************************************************************/

template <class Element>
int
DSortableArray<Element>::findpivot(int i, int j)
{
    const Element &first = my_elements[i];
    for (int k=i+1; k<=j; k++) {
        if (first < my_elements[k])
            return k;
        else if (my_elements[k] < first)
            return i;
    }
    return -1;
}

template <class Element>
int
DSortableArray<Element>::partition(int l, int r, const Element &pivot)
{
    do {
        swap(my_elements[l], my_elements[r]);
        while (my_elements[l] < pivot)
            l++;
        while (!(my_elements[r] < pivot))
            r--;
    } while (l <= r);
    return l;
}

template <class Element>
void
DSortableArray<Element>::quicksort(int i, int j)
{
    Element pivot;
    int k, index;

    index = findpivot(i, j);
    if (index != -1) {
        pivot = my_elements[index];
        k = partition(i, j, pivot);
        quicksort(i, k-1);
        quicksort(k, j);
    }
}

template <class Element>
inline void
DSortableArray<Element>::sort()
{
    quicksort(0, my_size-1);
}

/****************************************************************************/

template <class Element>
Element &
DSortableArray<Element>::min()
{
    if (my_size < 1)
        error("min() - empty array");

    int min_index = 0;
    for (int i=1; i<my_size; i++)
        if (my_elements[i] < my_elements[min_index])
            min_index = i;

    return my_elements[min_index];
}

/****************************************************************************/

template <class Element>
const Element &
DSortableArray<Element>::min() const
{
    if (my_size < 1)
        error("min() - empty array");

    int min_index = 0;
    for (int i=1; i<my_size; i++)
        if (my_elements[i] < my_elements[min_index])
            min_index = i;

    return my_elements[min_index];
}

/****************************************************************************/

template <class Element>
Element &
DSortableArray<Element>::max()
{
    if (my_size < 1)
        error("max() - empty array");

    int max_index = 0;
    for (int i=1; i<my_size; i++)
        if (my_elements[max_index] < my_elements[i])
            max_index = i;

    return my_elements[max_index];
}

/****************************************************************************/

template <class Element>
const Element &
DSortableArray<Element>::max() const
{
    if (my_size < 1)
        error("max() - empty array");

    int max_index = 0;
    for (int i=1; i<my_size; i++)
        if (my_elements[max_index] < my_elements[i])
            max_index = i;

    return my_elements[max_index];
}

/****************************************************************************/

template <class Element>
void
DSortableArray<Element>::error(const char *msg)
{
    cerr << "DSortableArray: " << msg << '\n';
    exit(EXIT_FAILURE);
}

/****************************************************************************/

#endif /* D_ARRAY_DEFINED */
