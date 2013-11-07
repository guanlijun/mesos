#ifndef __PROCESS_SHARED_HPP__
#define __PROCESS_SHARED_HPP__

#include <glog/logging.h>

#include <boost/shared_ptr.hpp>

#include <stout/owned.hpp>

#include <process/future.hpp>

namespace process {

// Represents a shared pointer and therefore enforces 'const' access.
template <typename T>
class Shared
{
public:
  Shared();
  explicit Shared(T* t);

  bool operator == (const Shared<T>& that) const;
  bool operator < (const Shared<T>& that) const;

  // Enforces const access semantics.
  const T& operator * () const;
  const T* operator -> () const;
  const T* get() const;

  bool unique() const;

  void reset();
  void reset(T* t);
  void swap(Shared<T>& that);

  // Upgrading from a shared pointer to an owned pointer. This shared
  // pointer will be reset after this function is invoked. If two
  // shared pointers pointing to the same object both want to be
  // upgraded, only one of them may succeed and the other one will get
  // a failed future.
  Future<Owned<T> > upgrade();

private:
  struct Data
  {
    Data(T* _t);
    ~Data();

    T* t;
    volatile bool upgraded;
    Promise<Owned<T> > promise;
  };

  boost::shared_ptr<Data> data;
};


template <typename T>
Shared<T>::Shared() {}


template <typename T>
Shared<T>::Shared(T* t)
{
  if (t != NULL) {
    data.reset(new Data(t));
  }
}


template <typename T>
bool Shared<T>::operator == (const Shared<T>& that) const
{
  return data == that.data;
}


template <typename T>
bool Shared<T>::operator < (const Shared<T>& that) const
{
  return data < that.data;
}


template <typename T>
const T& Shared<T>::operator * () const
{
  return *CHECK_NOTNULL(get());
}


template <typename T>
const T* Shared<T>::operator -> () const
{
  return CHECK_NOTNULL(get());
}


template <typename T>
const T* Shared<T>::get() const
{
  if (data.get() == NULL) {
    return NULL;
  } else {
    return data->t;
  }
}


template <typename T>
bool Shared<T>::unique() const
{
  return data.unique();
}


template <typename T>
void Shared<T>::reset()
{
  data.reset();
}


template <typename T>
void Shared<T>::reset(T* t)
{
  if (t == NULL) {
    data.reset();
  } else {
    data.reset(new Data(t));
  }
}


template <typename T>
void Shared<T>::swap(Shared<T>& that)
{
  data.swap(that.data);
}


template <typename T>
Future<Owned<T> > Shared<T>::upgrade()
{
  // If two threads simultaneously access this object and at least one
  // of them is a write, the behavior is undefined. This is similar to
  // boost::shared_ptr. For more details, please refer to the boost
  // shared_ptr document (section "Thread Safety").
  if (data.get() == NULL) {
    return Owned<T>(NULL);
  }

  if (!__sync_bool_compare_and_swap(&data->upgraded, false, true)) {
    return Future<Owned<T> >::failed("An upgrade is already being performed");
  }

  Future<Owned<T> > future = data->promise.future();
  data.reset();
  return future;
}


template <typename T>
Shared<T>::Data::Data(T* _t)
  : t(CHECK_NOTNULL(_t)), upgraded(false) {}


template <typename T>
Shared<T>::Data::~Data()
{
  if (upgraded) {
    promise.set(Owned<T>(t));
  } else {
    delete t;
  }
}

} // namespace process {

#endif // __PROCESS_SHARED_HPP__