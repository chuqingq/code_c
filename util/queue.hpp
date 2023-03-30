#pragma once

#include <boost/thread/concurrent_queues/sync_bounded_queue.hpp>

// https://www.boost.org/doc/libs/1_56_0/doc/html/thread/sds.html

// namespace boost
// {
//   template <typename ValueType>
//   class sync_bounded_queue
//   {
//   public:
//     typedef ValueType value_type;
//     typedef std::size_t size_type;

//     sync_bounded_queue(sync_bounded_queue const&) = delete;
//     sync_bounded_queue& operator=(sync_bounded_queue const&) = delete;
//     explicit sync_bounded_queue(size_type max_elems);
//     template <typename Range>
//     sync_bounded_queue(size_type max_elems, Range range);
//     ~sync_bounded_queue();

//     // Observers
//     bool empty() const;
//     bool full() const;
//     size_type capacity() const;
//     size_type size() const;
//     bool closed() const;

//     // Modifiers
//     void push_back(const value_type& x);
//     void push_back(value_type&& x);

//     queue_op_status try_push_back(const value_type& x);
//     queue_op_status try_push_back(value_type&&) x);

//     queue_op_status nonblocking_push_back(const value_type& x);
//     queue_op_status nonblocking_push_back(value_type&& x);

//     void pull_front(value_type&);
//     value_type pull_front();

//     queue_op_status try_pull_front(value_type&);
//     queue_op_status nonblocking_pull_front(value_type&);


//     void close();
//   };
// }

template<class T>
using Queue = boost::sync_bounded_queue<T>;
