/****************************************************************************
**
** Copyright (C) 2015 Corentin Chary <corentin.chary@gmail.cm>
**
** This file could be part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPRIORITY_QUEUE_H
#define QPRIORITY_QUEUE_H

#include <algorithm>
#include <QList>

template <class T> class QList;

/* Fallback class used when stl is not available */
template <class T, typename LessThan = std::less < T* > >
class QPriorityQueuePrivate {
public:
    inline QPriorityQueuePrivate(LessThan l) : lessThan(l), d() {}
    inline ~QPriorityQueuePrivate() {}

    inline void clear() { qDeleteAll(d); return d.clear(); }
    inline int size() const { return d.size(); }
    inline bool empty() const { return d.empty(); }

    inline T *top() { return d.front(); }

    void pop();
    void push(T *value);

    QList<T *> toList() {return d;}
private:
    inline int parent(int i) {
        return (i - 1) / 2;
    }
    inline int leftChild(int i) {
        return 2 * i + 1;
    }
    inline int rightChild(int i) {
        return 2 * i + 2;
    }

    LessThan lessThan;
    QList < T *> d;
};

template <class T, typename LessThan = std::less < T* > >
class Q_CORE_EXPORT QPriorityQueue
{
public:
    inline QPriorityQueue(LessThan l = std::less < T* >())
        : lessThan(l), d(lessThan) { }
    inline QPriorityQueue(const QPriorityQueue<T> &q)
        : lessThan(q.lessThan), d(q.d) { }
    inline ~QPriorityQueue() { }

    QPriorityQueue<T> &operator=(const QPriorityQueue<T> &q) {
        d = q.d; return *this;
    }

    inline int size() const { return d.size(); }
    inline bool isEmpty() const { return empty(); }

    // like qlist
    void clear();
    void append(T *t) { return enqueue(t); }
    void append(const QList<T*> t);
    T* takeFirst() { return dequeue(); }
    inline int length() const { return size(); } // Same as count()
    inline T* first() { return head(); }
    inline const T* first() const { return head(); }
    inline void removeFirst() { pop(); }
    inline bool startsWith(const T &t) const
    { return !isEmpty() && first() == t; }

    // like qqueue
    inline void enqueue(T* t) { push(t); }
    inline T* dequeue() { T* t = d.top(); d.pop(); return t; }
    inline T* head() { return top(); }
    inline const T* head() const { return top(); }

    // stl compatibility
    typedef int size_type;
    typedef T* value_type;

    inline bool empty() const { return d.empty(); }
    inline value_type top() { Q_ASSERT(!isEmpty()); return d.top(); }
    inline void push(value_type x) { return d.push(x); }
    inline void pop() { Q_ASSERT(!isEmpty()); d.pop(); }
    inline QList<T*> toList() { return d.toList(); }

    // comfort
    inline QPriorityQueue<T> &operator+=(T* t) {
        enqueue(t); return *this;
    }
    inline QPriorityQueue<T> &operator<< (T* t) {
        enqueue(t); return *this;
    }
    inline QPriorityQueue<T> &operator>> (T* t) {
        t = d.top(); d.pop(); return *this;
    }


private:
    LessThan lessThan;

    QPriorityQueuePrivate <T> d;
};



template <typename T, typename LessThan>
Q_INLINE_TEMPLATE void QPriorityQueue<T, LessThan>::clear()
{
    d.clear();
}


template <typename T, typename LessThan>
Q_OUTOFLINE_TEMPLATE void QPriorityQueue<T, LessThan>::append(const QList<T *> t)
{
    foreach (T * e, t)
        push(e);
}

/*!
 *  Pop an element from the queue and reorder it using an
 *  inlined binary heap.
 *
 *  \internal
 */
template <typename T, typename LessThan>
Q_OUTOFLINE_TEMPLATE void QPriorityQueuePrivate<T, LessThan>::pop()
{
    int i = 0;
    ssize_t size = d.size();;

    if(!size)
        return;
    if(size == 1)
        return d.clear();

    d[0] = d.takeLast();

    while(i < size - 1) {
        int left = leftChild(i);
        int right = rightChild(i);
        bool validLeft = left < size;
        bool validRight = right < size;

        if(validLeft && lessThan(d.at(i), d.at(left)))
            if(validRight && !lessThan(d.at(right), d.at(left))) {
                d.swap(i, right);
                i = right;
            } else {
                d.swap(i, left);
                i = left;
            }
        else if(validRight && lessThan(d.at(i), d.at(right))) {
            d.swap(i, right);
            i = right;
        }
        else
            break;
    }
}

/*!
 *  Push an element with a given priority to the right place.
 *
 *  \internal
 */
template <typename T, typename LessThan>
Q_OUTOFLINE_TEMPLATE void QPriorityQueuePrivate<T, LessThan>::push(T *value)
{
    int i = d.size();
    d.append(value);
    while(i != 0 && !lessThan(d.at(i), d.at(parent(i)))) {
        d.swap(i, parent(i));
        i = parent(i);
    }
}


#endif // QPRIORITY_QUEUE_H
