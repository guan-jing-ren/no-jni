/*
Use `grep "..." jnixx.h` to compile this file, replace ... with compiler
time /home/kykwan/gcc/bin/g++ -x c++ -o libjnixx.so -O3 -std=c++14 -Wall -Wextra -Werror -pedantic-errors -Wl,-rpath=/home/kykwan/gcc/lib64 -m64 -shared -fPIC -I /usr/lib64/jvm/java-1.7.0/include/ -I/usr/lib64/jvm/java-1.7.0/include/linux -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/ -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/server -I /usr/lib/jvm/java-1.7.0/include/ -I/usr/lib64/jvm/java-1.7.0/include/linux -L/usr/lib/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/ -L/usr/lib/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/server jnixx.h
time clang++                  -x c++ -o libjnixx.so -O3 -std=c++14 -Wall -Wextra -Werror -pedantic-errors -Wl,-rpath=/home/kykwan/gcc/lib64 -m64 -shared -fPIC -I /usr/lib64/jvm/java-1.7.0/include/ -I/usr/lib64/jvm/java-1.7.0/include/linux -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/ -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/server -I /usr/lib/jvm/java-1.7.0/include/ -I/usr/lib64/jvm/java-1.7.0/include/linux -L/usr/lib/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/ -L/usr/lib/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/server -fcolor-diagnostics jnixx.h
*/

#include "JNIXXTest.h"
#include <type_traits>
#include <utility>
#include <cstdio>

thread_local JNIEnv* jni;

#include "test_utils.h"
#include "signature_helpers.h"
#include "jni_invokers.h"

template<typename T>
constexpr bool for_sfinae = true;

template<typename T>
class jhandle;

std::false_type deduce_jhandle(...);

template<typename T>
std::true_type deduce_jhandle(const jhandle<T>&);

template<typename T>
struct is_jhandle_t : decltype(deduce_jhandle(std::declval<T>())){};

template<typename T>
struct is_jhandle_t<jhandle<T>> : std::conditional_t<std::is_arithmetic<T>{} || (std::is_pointer<T>{} && std::is_convertible<T, jobject>{}) || is_jhandle_t<T>{}, std::true_type, std::false_type>{};

template<typename T>
struct is_jhandle_array_t : std::false_type{};

template<typename T>
struct is_jhandle_array_t<jhandle<T>> : std::conditional_t<std::is_arithmetic<T>{} || is_jhandle_t<T>{}, std::true_type, std::false_type>{};

template<typename T>
struct is_jhandle_string_t : std::false_type{};

template<>
struct is_jhandle_string_t<jhandle<jstring>> : std::true_type{};

// jstrings and jarrays are accessed through specialized JNI functions and not through jmethodID or jfieldID
// This type trait excludes them so only plain jhandles can call methods and access fields
template<typename T>
struct is_jhandle_plain_t : std::conditional_t<is_jhandle_t<T>{} && !is_jhandle_string_t<T>{} && !is_jhandle_array_t<T>{}, std::true_type, std::false_type>{};

template<typename T>
struct is_jhandle_class_t : std::false_type{};

template<>
struct is_jhandle_class_t<jhandle<jclass>> : std::true_type{};

template<typename T>
constexpr bool is_jhandle = is_jhandle_t<T>::value;

template<typename T>
constexpr bool is_jhandle_array = is_jhandle_array_t<T>::value;

template<typename T>
constexpr bool is_jhandle_string = is_jhandle_string_t<T>::value;

// jstrings and jarrays are accessed through specialized JNI functions and not through jmethodID or jfieldID
template<typename T>
constexpr bool is_jhandle_plain = is_jhandle_plain_t<T>::value;

template<typename T>
constexpr bool is_jhandle_class = is_jhandle_class_t<T>::value;

template<typename U, typename V, typename W = void>
auto steal(V& u) -> std::enable_if_t<is_jhandle<U> && is_jhandle<V> && for_sfinae<W>, U>
{
  static_assert((is_jhandle_array<U> && (is_jhandle_array<V> || std::is_same<typename V::value_type, jobject>{}))
	      || std::is_same<typename U::value_type, typename V::value_type>{}, "Can only acquire exact wrappee type");
  U t{nullptr};
  t.object = u.object;
  u.object = nullptr;
  return t;
}

template<typename U, typename V, typename W = void>
auto steal(V& u) -> std::enable_if_t<is_jhandle<U> && std::is_pointer<V>{} && std::is_convertible<V, jobject>{}&& for_sfinae<W>, U>
{
  static_assert((is_jhandle_array<U> && (std::is_same<V, jobject>{} || std::is_convertible<V, jarray>{}))
	      || std::is_same<typename U::value_type, V>{}, "Can only acquire exact wrappee type");
  U t{nullptr};
  t.object = u;
  u = nullptr;
  return t;
}

template<typename U, typename V>
U steal(V&& u)
{
  return steal<U>(u);
}

jhandle<jclass> find_class(const std::string& name);
jhandle<jclass> find_loaded_class(const std::string& name);

auto exception_describe()
{
  ::jni->ExceptionDescribe();
}

auto exception_clear()
{
  ::jni->ExceptionClear();
}

auto fatal_error(const std::string& message)
{
  ::jni->FatalError(message.c_str());
}

auto exception_check()
{
  return ::jni->ExceptionCheck();
}

template<typename T>
class jhandle
{
  static_assert(is_jhandle<jhandle>, "jhandle instantiation must be arithmetic, a jobject derivation, or a jhandle of either");
  template<typename> friend class jhandle;
  using value_type = T;
  jobject object{nullptr};

  template<typename U, typename V, typename W>
  friend auto steal(V& u) -> std::enable_if_t<is_jhandle<U> && is_jhandle<V> && for_sfinae<W>, U>;

  template<typename U, typename V, typename W>
  friend auto steal(V& u) -> std::enable_if_t<is_jhandle<U> && std::is_pointer<V>{} && std::is_convertible<V, jobject>{}&& for_sfinae<W>, U>;

  template<typename U, typename V>
  friend U steal(V&& u);

  template<typename U>
  static auto convert_if_jobject(const jhandle<U>& t)
  {
    return static_cast<jobject>(static_cast<void*>(t));
  }

  template<typename U>
  static auto convert_if_jobject(const U& t) -> std::enable_if_t<std::is_convertible<U, jobject>{}, jobject>
  {
    return t;
  }

  template<typename U>
  static auto convert_if_jobject(const U& t) -> std::enable_if_t<std::is_convertible<U, native_signature_t>{}, U>
  {
    return t;
  }

  template<typename U>
  using invoker = invoke_jni<decltype(convert_if_jobject(std::declval<U>()))>;

  template<typename U>
  class field_proxy final
  {
    friend class jhandle;
    std::enable_if_t<is_jhandle_plain<jhandle>, jhandle> object;
    std::enable_if_t<is_jhandle_plain<jhandle>, jfieldID> id = nullptr;

    field_proxy(const jhandle& o, jfieldID f) : object(o), id(f) {}

    // cast operators cannot be overridden and SFINAE disabled so it needs to defer to functions that can
    template<typename V = void>
    auto cast() const& -> std::enable_if_t<std::is_arithmetic<U>{} && for_sfinae<V>, U>
    {
      return static_cast<U>(invoker<U>::get(object, id));
    }

    template<typename V = void>
    auto cast() const& -> std::enable_if_t<is_jhandle<U> && for_sfinae<V>, U>
    {
      return steal<U>(static_cast<typename U::value_type>(invoke_jni<jobject>::get(object, id)));
    }

  public:
    field_proxy() = delete;
    field_proxy(const field_proxy&) = default;
    field_proxy(field_proxy&&) = default;
    void operator=(const field_proxy& f)
    {
      invoker<U>::set(object, id, *f);
    }

    operator U() const&
    {
      return cast();
    }

    U operator*() const&
    {
      return cast();
    }

    void operator=(const U& value) const&
    {
      invoker<U>::set(object, id, value);
    }

    template<typename R = jvoid, typename... Args>
    auto method(const char* name, Args&&... args) && -> std::enable_if_t<is_jhandle_plain<U> && for_sfinae<R>, R>
    {
      return cast().template method<R>(name, std::forward<Args>(args)...);
    }

    template<typename F, typename V = void>
    auto field(const char* name) && -> std::enable_if_t<is_jhandle_plain<U> && for_sfinae<V>, field_proxy<F>>
    {
      return cast().template field<F>(name);
    }

    template<typename V = void>
    auto length() && -> std::enable_if_t<(is_jhandle_string<U> || is_jhandle_array<U>) && for_sfinae<V>, jint>
    {
      return cast().length();
    }

    template<typename V = void>
    auto to_string() && -> std::enable_if_t<is_jhandle_string<U> && for_sfinae<V>, std::string>
    {
      return cast().to_string();
    }

//     template<typename V = void>
//     auto to_vector() && -> std::enable_if_t<is_jhandle_array<U> && for_sfinae<V>, std::vector<typename U::value_type>>
//     {
//       return cast().to_vector();
//     }

    template<typename V>
    friend bool operator==(const field_proxy& l, const field_proxy<V>& r)
    {
      return *l == *r;
    }
  };

//   template<typename> friend class field_proxy;	// Allows field_proxy to declare its friend class jhandle<U>::element_proxy

  class element_proxy final
  {
    friend class jhandle;
    friend class java_array_iterator;
    using array_type = typename invoker<value_type>::array_type;
    std::enable_if_t<is_jhandle_array<jhandle>, jhandle> array;
    jsize index{0};

    element_proxy(const jhandle& a, jsize i) : array(a), index(i) {}

    template<typename V = void>
    auto cast() const& -> std::enable_if_t<std::is_arithmetic<value_type>{} && for_sfinae<V>, value_type>
    {
      return static_cast<value_type>(invoker<value_type>::get(array, index));
    }

    template<typename V = void>
    auto cast() const& -> std::enable_if_t<is_jhandle<value_type> && for_sfinae<V>, value_type>
    {
      return steal<value_type>(invoke_jni<jobject>::get(array, index));
    }

  public:
    element_proxy() = delete;
    element_proxy(const element_proxy&) = default;
    element_proxy(element_proxy&&) = default;
    void operator=(const element_proxy& e)
    {
      *this = static_cast<value_type>(e);
    }

    operator value_type() const&
    {
      return cast();
    }

    value_type operator*() const&
    {
      return cast();
    }

    void operator=(const value_type& value) const&
    {
      invoker<value_type>::set(array, value, index);
    }

    template<typename R = jvoid, typename... Args>
    auto method(const char* name, Args&&... args) && -> std::enable_if_t<is_jhandle_plain<value_type>, R>
    {
      return cast().template method<R>(name, std::forward<Args>(args)...);
    }

    template<typename F, typename V = void, typename = std::enable_if_t<is_jhandle_plain<value_type> && for_sfinae<V>>>
    auto field(const char* name) &&
    {
      return cast().template field<F>(name);
    }

    template<typename V = void>
    auto length() && -> std::enable_if_t<(is_jhandle_string<value_type> || is_jhandle_array<value_type>) && for_sfinae<V>, jint>
    {
      return cast().length();
    }

    template<typename V = void>
    auto to_string() && -> std::enable_if_t<is_jhandle_string<value_type> && for_sfinae<V>, std::string>
    {
      return cast().to_string();
    }

    template<typename V = void>
    auto to_vector() && -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<V>, std::vector<value_type>>
    {
      return cast().to_vector();
    }

    friend bool operator==(const element_proxy& l, const element_proxy& r)
    {
      return ::jni->IsSameObject(l.array, r.array) && l.index == r.index;
    }
  };

//   friend class element_proxy;

  template<typename U, typename... Args>
  auto method_impl(const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && std::is_convertible<U, native_signature_t>{}, U>
  {
    auto m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U{}, method_args, args...).c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return static_cast<U>(invoker<U>::call(object, m, convert_if_jobject(args)...));
  }

  template<typename U, typename... Args>
  auto method_impl(const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_class<jhandle> && std::is_convertible<U, native_signature_t>{}, U>
  {
    auto m = ::jni->GetStaticMethodID(static_cast<jclass>(object), name, make_signature(U{}, method_args, args...).c_str());
    if(m) return static_cast<U>(invoker<U>::call(static_cast<jclass>(object), m, convert_if_jobject(args)...));
//     if(exception_check()) LOG(""); exception_describe();
    m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U{}, method_args, args...).c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return static_cast<U>(invoker<U>::call(object, m, convert_if_jobject(args)...));
  }

  template<typename U, typename... Args>
  auto method_impl(const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && is_jhandle<U>, U>
  {
    auto m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U::get_signature(), method_args, args...).c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return steal<U>(static_cast<std::conditional_t<is_jhandle_array<U>, jobject, typename U::value_type>>(invoke_jni<jobject>::call(object, m, convert_if_jobject(args)...)));
  }

  template<typename U, typename... Args>
  auto method_impl(const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_class<jhandle> && is_jhandle<U>, U>
  {
    auto m = ::jni->GetStaticMethodID(static_cast<jclass>(object), name, make_signature(U::get_signature(), method_args, args...).c_str());
    if(m) return steal<U>(static_cast<std::conditional_t<is_jhandle_array<U>, jobject, typename U::value_type>>(invoke_jni<jobject>::call(static_cast<jclass>(object), m, convert_if_jobject(args)...)));
//     if(exception_check()) LOG(""); exception_describe();
    m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U::get_signature(), method_args, args...).c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return steal<U>(static_cast<std::conditional_t<is_jhandle_array<U>, jobject, typename U::value_type>>(invoke_jni<jobject>::call(object, m, convert_if_jobject(args)...)));
  }

  template<typename U>
  auto field_impl(const char* name) const -> std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && std::is_arithmetic<U>{}, field_proxy<U>>
  {
    auto f = ::jni->GetFieldID(object_class(), name, signature_of(U{}).c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return {*this, f};
  }

  template<typename U>
  auto field_impl(const char* name) const -> std::enable_if_t<is_jhandle_class<jhandle> && std::is_arithmetic<U>{}, field_proxy<U>>
  {
    auto f = ::jni->GetStaticFieldID(static_cast<jclass>(object), name, signature_of(U{}).c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return {*this, f};
  }

  template<typename U>
  auto field_impl(const char* name) const -> std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && is_jhandle<U>, field_proxy<U>>
  {
    auto f = ::jni->GetFieldID(object_class(), name, U::get_signature().c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return {*this, f};
  }

  template<typename U>
  auto field_impl(const char* name) const -> std::enable_if_t<is_jhandle_class<jhandle> && is_jhandle<U>, field_proxy<U>>
  {
    auto f = ::jni->GetStaticFieldID(static_cast<jclass>(object), name, U::get_signature().c_str());
//     if(exception_check()) LOG(""); exception_describe();
    return {*this, f};
  }

  template<typename U>
  auto length_impl() const -> std::enable_if_t<is_jhandle_string<jhandle> && for_sfinae<U>, jsize>
  {
    return object ? ::jni->GetStringUTFLength(static_cast<jstring>(object)) : 0;
  }

  template<typename U>
  auto length_impl() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, jsize>
  {
    return object ? ::jni->GetArrayLength(static_cast<jarray>(object)) : 0;
  }

  class java_array_iterator final : public std::iterator<std::random_access_iterator_tag, value_type>
  {
    friend class jhandle;
    std::enable_if_t<is_jhandle_array<jhandle>, element_proxy> proxy;

    java_array_iterator(element_proxy&& p) : proxy(std::move(p)) {}
  public:
    java_array_iterator(const java_array_iterator& other) = default;

    element_proxy& operator*()
    {
      return proxy;
    }

    java_array_iterator& operator+=(std::make_signed_t<jsize> n)
    {
      proxy.index += n;
      return *this;
    }

    java_array_iterator& operator-=(std::make_signed_t<jsize> n)
    {
      proxy.index -= n;
      return *this;
    }

    java_array_iterator& operator++()
    {
      ++proxy.index;
      return *this;
    }

    java_array_iterator& operator--()
    {
      --proxy.index;
      return *this;
    }

    java_array_iterator operator+(std::make_signed_t<jsize> n) const
    {
      return {{proxy.array, proxy.index + n}};
    }

    java_array_iterator operator-(std::make_signed_t<jsize> n) const
    {
      return {{proxy.array, proxy.index - n}};
    }

    auto operator-(const java_array_iterator& other) const
    {
      return proxy.index - other.proxy.index;
    }

    element_proxy operator[](std::make_signed_t<jsize> n) const
    {
      return *(*this + n);
    }

    bool operator<(const java_array_iterator& other) const
    {
      return proxy.index < other.proxy.index;
    }

    friend bool operator==(const java_array_iterator& l, const java_array_iterator& r)
    {
      return l.proxy == r.proxy;
    }

    friend bool operator!=(const java_array_iterator& l, const java_array_iterator& r)
    {
      return !(l == r);
    }
  };

public:
  jhandle(std::nullptr_t) : object(nullptr) {}
  jhandle& operator=(std::nullptr_t other)
  {
    switch(ref_type())
    {
      default:
        break;
      case JNILocalRefType:
        ::jni->DeleteLocalRef (object);
        break;
      case JNIGlobalRefType:
        ::jni->DeleteGlobalRef(object);
        break;
    }
    object = nullptr;
    return *this;
  }
  jhandle() = default;

  // The use of dummy template argument U = void throughout the class is to turn functions into template functions
  // This gives the enable_if_t a dependent type and allow it to hide a function for inappropriate value_type's
  // Non-template functions cannot be hidden and cause template instantiation errors for innappropriate value_type's
  template<typename U = void, typename = std::enable_if_t<is_jhandle_plain<jhandle> && for_sfinae<U>>, typename... Args>
  jhandle(const std::string& name, Args&&... args)
  {
    auto klass = find_loaded_class(name);
    if(exception_check()) LOG(""); exception_describe();
    auto m = ::jni->GetMethodID(klass, "<init>", make_signature(jvoid_arg, method_args, args...).c_str());
    if(exception_check()) LOG(""); exception_describe();
    object = ::jni->NewObject(klass, m, convert_if_jobject(args)...);
    if(exception_check()) LOG(""); exception_describe();
  }

  template<typename U = void, typename = std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && for_sfinae<U>>, typename... Args>
  jhandle(const jhandle<jclass>& klass, Args&&... args)
  {
    if(exception_check()) LOG(""); exception_describe();
    auto m = ::jni->GetMethodID(klass, "<init>", make_signature(jvoid_arg, method_args, args...).c_str());
    if(exception_check()) LOG(""); exception_describe();
    object = ::jni->NewObject(klass, m, convert_if_jobject(args)...);
    if(exception_check()) LOG(""); exception_describe();
  }

  template<typename U = void, typename = std::enable_if_t<is_jhandle_string<jhandle> && for_sfinae<U>>, typename... Args>
  jhandle(const std::string& chars)
  {
    object = ::jni->NewStringUTF(chars.c_str());
  }

  template<typename U = void, typename = std::enable_if_t<std::is_arithmetic<value_type>{} && for_sfinae<U>>, typename... Args>
  jhandle(jsize len)
  {
    object = invoker<value_type>::array(len);
  }

  template<typename U = void, typename = std::enable_if_t<is_jhandle<value_type> && for_sfinae<U>>, typename... Args>
  jhandle(jsize len, const value_type& initial)
  {
    struct String : jhandle<jstring>
    {
      using jhandle::jhandle;

      std::string get_signature() const
      {
	return make_signature(qualified_name, "java", "lang", "String");
      }
    };
    object = invoke_jni<jobject>::array(len, initial.object_class(), initial);
  }

  jhandle& operator=(const jhandle& other)
  {
//     LOG("Copying from handle " << other.object);
//     if(object)
    switch(ref_type())
    {
      default:
	break;
      case JNILocalRefType:
	::jni->DeleteLocalRef (object);
	break;
      case JNIGlobalRefType:
	::jni->DeleteGlobalRef(object);
	break;
    }
    object = nullptr;
//     if(other.object)
    switch(other.ref_type())
    {
      default:
	break;
      case JNILocalRefType:
	object = ::jni->NewLocalRef (other.object);
	break;
      case JNIGlobalRefType:
	object = ::jni->NewGlobalRef(other.object);
	break;
    }
//         LOG("Copying handle to " << object);
    return *this;
  }

  jhandle& operator=(jhandle&& other)
  {
//     if(object)
    switch(ref_type())
    {
      default:
	break;
      case JNILocalRefType:
	::jni->DeleteLocalRef (object);
	break;
      case JNIGlobalRefType:
	::jni->DeleteGlobalRef(object);
	break;
    }
    object = other.release();
    return *this;
  }

  jhandle(const jhandle& other)
  {
//     LOG("Copying from handle " << other.object);
//     if(other.object)
    switch(other.ref_type())
    {
      default:
	break;
      case JNILocalRefType:
	object = ::jni->NewLocalRef (other.object);
	break;
      case JNIGlobalRefType:
	object = ::jni->NewGlobalRef(other.object);
	break;
    }
//     LOG("Copying handle to " << object);
  }

  jhandle(jhandle&& other)
  {
//     if(object)
    switch(ref_type())
    {
      default:
	break;
      case JNILocalRefType:
	::jni->DeleteLocalRef (object);
	break;
      case JNIGlobalRefType:
	::jni->DeleteGlobalRef(object);
	break;
    }
    object = other.release();
  }

  operator bool() const
  {
    return object != nullptr;
  }

  operator std::conditional_t<is_jhandle_array<jhandle>, typename invoker<value_type>::array_type, value_type>() const
  {
    return static_cast<std::conditional_t<is_jhandle_array<jhandle>, typename invoker<value_type>::array_type, value_type>>(object);
  }

  operator void*() const
  {
    return object;
  }

  template<typename U = void>
  static auto get_signature() -> std::enable_if_t<is_jhandle_array<jhandle> && !is_jhandle<value_type> && for_sfinae<U>, std::string>
  {
    return is_jhandle_array<jhandle> ? "[" + signature_of(value_type{}) : std::string{"[nondescript object]"};
  }

  template<typename U = void>
  static auto get_signature() -> std::enable_if_t<is_jhandle_array<jhandle> && is_jhandle<value_type> && for_sfinae<U>, std::string>
  {
    return is_jhandle_array<jhandle> ? "[" + value_type::get_signature() : std::string{"[nondescript object]"};
  }

  template<typename U = void>
  static auto get_signature() -> std::enable_if_t<is_jhandle_string<jhandle> && for_sfinae<U>, std::string>
  {
    return "Ljava/lang/String;";
  }

  template<typename U = void>
  static auto get_signature() -> std::enable_if_t<is_jhandle_plain<jhandle> && for_sfinae<U>, std::string>
  {
    return std::string{"[nondescript object]"};
  }

  template<typename U, typename = std::enable_if_t<std::is_base_of<jhandle, U>{}>> operator U() const
  {
    return *this;
  }

  auto release()
  {
    auto released = object;
    object = nullptr;
    return static_cast<std::conditional_t<is_jhandle_array<jhandle>, jarray, value_type>>(released);
  }

  auto& globalize()
  {
//     if(object)
    if(ref_type() == JNILocalRefType)
    {
      auto old = object;
      object = ::jni->NewGlobalRef(old);
      ::jni->DeleteLocalRef(old);
    }
    return *this;
  }

  auto& localize()
  {
//     if(object)
    if(ref_type() == JNIGlobalRefType)
    {
      auto old = object;
      object = ::jni->NewLocalRef(old);
      ::jni->DeleteGlobalRef(old);
    }
    return *this;
  }

  ~jhandle()
  {
//     if(object) LOG("Destroying " << object);
//     if(object)
    if(!::jni) return;
    switch(ref_type())
    {
      default:
	break;
      case JNILocalRefType:
	::jni->DeleteLocalRef(object);
	break;
      case JNIGlobalRefType:
	::jni->DeleteGlobalRef(object);
	break;
    }

    object = nullptr;
  }

  auto monitor_enter() const
  {
    return ::jni->MonitorEnter(object);
  }

  auto monitor_exit() const
  {
    return ::jni->MonitorExit(object);
  }

  auto object_class() const
  {
    return steal<jhandle<jclass>>(object ? ::jni->GetObjectClass(object) : nullptr);
  }

  auto ref_type() const
  {
    return ::jni->GetObjectRefType(object);
  }

  bool instance_of(const jhandle<jclass>& other) const
  {
    return ::jni->IsInstanceOf(object, other);
  }

  template<typename U>
  friend bool operator==(const jhandle<value_type>& l, const jhandle<U>& r)
  {
    return ::jni->IsSameObject(l.object, r.object);
  }

  template<typename U = void>
  auto super_class() const -> std::enable_if_t<is_jhandle_class<jhandle> && for_sfinae<U>, jhandle>
  {
    return steal<jhandle<jclass>>(object ? ::jni->GetSuperclass(static_cast<jclass>(object)) : nullptr);
  }

  template<typename U = void>
  auto assignable_from(const jhandle<jclass>& other) const -> std::enable_if_t<is_jhandle_class<jhandle> && for_sfinae<U>, bool>
  {
    return ::jni->IsAssignableFrom(static_cast<jclass>(object), other);
  }

  template<typename U = jvoid, typename... Args>
  auto method(const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_plain<jhandle> && for_sfinae<U>, U>
  {
    return method_impl<U>(name, std::forward<Args>(args)...);
  }

  template<typename U>
  auto field(const char* name) const -> std::enable_if_t<is_jhandle_plain<jhandle> && for_sfinae<U>, field_proxy<U>>
  {
    return field_impl<U>(name);
  }

  template<typename U = void, typename = std::enable_if_t<is_jhandle_string<jhandle> && for_sfinae<U>>>
  std::string to_string() const
  {
    if(!object) return "";
    auto len = static_cast<std::string::size_type>(length());
    auto utf_deleter = [this](auto chars)
    {
      ::jni->ReleaseStringUTFChars(static_cast<jstring>(object), chars);
    };
    std::unique_ptr<const std::string::value_type, decltype(utf_deleter)> chars{::jni->GetStringUTFChars(static_cast<jstring>(object), nullptr), utf_deleter};
    return {chars.get(), len};
  }

  template<typename U = void>
  auto length() const -> std::enable_if_t<(is_jhandle_string<jhandle> || is_jhandle_array<jhandle>) && for_sfinae<U>, jsize>
  {
    return length_impl<U>();
  }

  template<typename U = void>
  auto operator[](jsize i) const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, element_proxy>
  {
    return element_proxy{*this, i};
  }

  template<typename U>
  auto operator[](U) const = delete;

  template<typename U = void>
  auto begin() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return {(*this)[0]};
  }

  template<typename U = void>
  auto end() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return begin() + length();
  }

  template<typename U = void>
  auto cbegin() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return begin();
  }

  template<typename U = void>
  auto cend() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return end();
  }

  template<typename U = void>
  auto rbegin() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return std::reverse_iterator<decltype(end())>(end());
  }

  template<typename U = void>
  auto rend() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return std::reverse_iterator<decltype(begin())>(begin());
  }

  template<typename U = void>
  auto crbegin() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return rbegin();
  }

  template<typename U = void>
  auto crend() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return rend();
  }

  template<typename U = void>
  auto to_vector() const& -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, std::vector<value_type>>
  {
    std::vector<value_type> v;
    std::transform(begin(), end(), std::back_inserter(v), [](auto& i) {return *i;});
    return v;
  }

  template<typename U = void, typename = std::enable_if_t<std::is_same<value_type, jthrowable>{} && for_sfinae<U>>>
  auto throw_this() const
  {
    ::jni->Throw(static_cast<jthrowable>(object));
  }
};

struct jhandle_hash
{
  auto operator()(const jhandle<jobject>& object) const
  {
    return object.method<jint>("hashCode");
  }
};

jhandle<jclass> find_class(const std::string& name)
{
  return steal<jhandle<jclass>>(::jni->FindClass(name.c_str()));
}

template<typename T>
auto define_class(std::string name, const jhandle<jobject>& loader = {}) -> std::enable_if_t<is_jhandle_class<T> || std::is_base_of<jhandle<jclass>, T>{}, T>
{
  static_assert(is_jhandle_class<T> || std::is_base_of<jhandle<jclass>, T>{}, "Must be called for jhandle<jclass> type");
  std::ifstream class_file{name + ".class", std::ifstream::binary | std::ifstream::in};
  std::stringbuf buffer;
  class_file >> &buffer;
  auto bytes = buffer.str();
  LOG("Bytes read: " << bytes.size());
  struct ClassLoader : jhandle<jobject>
  {
    using jhandle::jhandle;

    static std::string get_signature()
    {
      return make_signature(qualified_name, "java", "lang", "ClassLoader");
    }
  };
  if(loader)
  return steal<T>(::jni->DefineClass(name.c_str(), loader, reinterpret_cast<const jbyte*>(bytes.c_str()), bytes.size()));
  return steal<T>(::jni->DefineClass(name.c_str(), find_class("java/lang/ClassLoader").template method<ClassLoader>("getSystemClassLoader"), reinterpret_cast<const jbyte*>(bytes.c_str()), bytes.size()));
}

auto throw_new(const std::string& name, const std::string& message)
{
  ::jni->ThrowNew(find_class(name), message.c_str());
}

auto exception_occurred()
{
  return steal<jhandle<jthrowable>>(::jni->ExceptionOccurred());
}

constexpr jint local_capacity_default = 32;

auto ensure_local_capacity(jint capacity = local_capacity_default)
{
  ::jni->EnsureLocalCapacity(capacity);
}

auto push_local_frame(jint capacity = local_capacity_default)
{
  ::jni->PushLocalFrame(capacity);
}

template<typename H = jhandle<jobject>>
auto pop_local_frame(H object = {}) -> std::enable_if_t<is_jhandle<H>, H>
{
  return steal<H>(::jni->PopLocalFrame(object.release()));
}

template<typename P = jhandle<jobject>, typename = std::enable_if_t<is_jhandle<P>>>
void exception_print(const jhandle<jthrowable>& e = {}, const P& p = {})
{
  auto t = e ? e : (exception_check() ? exception_occurred() : e);
  if(!t) return;
  if(p) t.method<jvoid>("printStackTrace", p);
  else t.method<jvoid>("printStackTrace");
}

template<typename C>
auto jscope(C&& c, jint capacity = local_capacity_default)
{
  if(::jni) push_local_frame(capacity);
  auto r = make_restorer([](){if(::jni) pop_local_frame();});
  (void) r;
  c();
}

struct catch_block
{
  template<typename C>
  catch_block jcatch(C&& c, jint capacity = local_capacity_default)
  {
    jscope([&]()
    {
      if(exception_check())
	if(c(exception_occurred()))
	  exception_clear();
    }, capacity);
    return *this;
  }

  template<typename C>
  void jfinally(C&& c)
  {
    c();
  }
};

template<typename C>
catch_block jtry(C&& c, jint capacity = local_capacity_default)
{
  jscope([&]()
  {
    c();
  }, capacity);
  return {};
}

static_assert(sizeof(jhandle<jobject>) == sizeof(jobject) && std::is_standard_layout<jhandle<jobject>>{}, "jhandle<jobject> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jclass>) == sizeof(jobject) && std::is_standard_layout<jhandle<jclass>>{}, "jhandle<jclass> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jstring>) == sizeof(jobject) && std::is_standard_layout<jhandle<jstring>>{}, "jhandle<jstring> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jthread>) == sizeof(jobject) && std::is_standard_layout<jhandle<jthread>>{}, "jhandle<jthread> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jthreadGroup>) == sizeof(jobject) && std::is_standard_layout<jhandle<jthreadGroup>>{}, "jhandle<jthreadGroup> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jthrowable>) == sizeof(jthrowable) && std::is_standard_layout<jhandle<jthrowable>>{}, "jhandle<jobject> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jbyte>) == sizeof(jobject) && std::is_standard_layout<jhandle<jbyte>>{}, "jhandle<jbyte> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jchar>) == sizeof(jobject) && std::is_standard_layout<jhandle<jchar>>{}, "jhandle<jchar> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jshort>) == sizeof(jobject) && std::is_standard_layout<jhandle<jshort>>{}, "jhandle<jshort> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jint>) == sizeof(jobject) && std::is_standard_layout<jhandle<jint>>{}, "jhandle<jint> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jlong>) == sizeof(jobject) && std::is_standard_layout<jhandle<jlong>>{}, "jhandle<jlong> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jfloat>) == sizeof(jobject) && std::is_standard_layout<jhandle<jfloat>>{}, "jhandle<jfloat> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jdouble>) == sizeof(jobject) && std::is_standard_layout<jhandle<jdouble>>{}, "jhandle<jdouble> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jhandle<jobject>>) == sizeof(jobject) && std::is_standard_layout<jhandle<jhandle<jobject>>>{}, "jhandle<jhandle<jobject>> is not a thin wrapper of jobject");

#ifndef JVMTI_ERROR_NONE
JNIEXPORT void JNICALL Java_JNIXXTest_do_1stuff(JNIEnv* env, jclass klass)
{
  ::jni = env;
  auto l = steal<jhandle<jclass>>(klass);
  LOG((void*)klass);

  struct Object : jhandle<jobject>
  {
    using jhandle::jhandle;

    static std::string get_signature()
    {
      return make_signature(qualified_name, "java", "lang", "Object");
    }
  };

  struct PrintStream : jhandle<jobject>
  {
    using jhandle::jhandle;

    static std::string get_signature()
    {
      return make_signature(qualified_name, "java", "io", "PrintStream");
    }
  };

  struct String : jhandle<jstring>
  {
    using jhandle::jhandle;

    static std::string get_signature()
    {
      return make_signature(qualified_name, "java", "lang", "String");
    }
  };

  struct IntArray : jhandle<jint>
  {
    using jhandle::jhandle;
  };

  auto system = find_class("java/lang/System");
  system.field<PrintStream>("err") = system.field<PrintStream>("out");
  system.field<PrintStream>("out").method("println", jdouble{3.14159265358979});
  system.field<PrintStream>("out").method("println", l.method<String>("getCanonicalName"));
  system.field<PrintStream>("out").method("println", l.object_class().method<String>("getCanonicalName"));
  system.field<PrintStream>("out").method("println", system.method<String>("getCanonicalName"));

  l.method<jvoid>("do_other_stuff");
  l.field<jint>("i") = 200;
  system.field<PrintStream>("out").method("println", l.method<String>("return_string"));
  system.field<PrintStream>("out").method("println", *l.field<jint>("i"));
  PrintStream out;
  out = system.field<PrintStream>("out");
  LOG(std::boolalpha << "Output streams equal? " << (*system.field<PrintStream>("err") == *system.field<PrintStream>("out")));
  LOG(std::boolalpha << "Output streams equal? " << (*system.field<PrintStream>("err") == out));

  system.field<PrintStream>("out").method("println", Object{"JNIXXTest"});
  system.field<PrintStream>("out").method("println", String{"JNIXXTest as string"});
  system.field<PrintStream>("out").method("println", String{"JNIXXTest as string"}.length());
  LOG(String{"Converted jstring"}.to_string());

  LOG(signature_of(IntArray{12}));
  IntArray array{12};
  std::generate(array.begin(), array.end(), [i = 1]() mutable{return i *= 2;});
  system.field<PrintStream>("out").method("println", find_class("java/util/Arrays").method<String>("toString", array));
  array[1] = 42;
  array[9] = array[1];
  system.field<PrintStream>("out").method("println", find_class("java/util/Arrays").method<String>("toString", array));

  struct JNIXXTest : jhandle<jobject>
  {
    using jhandle::jhandle;

    static std::string get_signature()
    {
      return make_signature(qualified_name, "JNIXXTest");
    }
  };

//   (void)system[5];
  jhandle<JNIXXTest> test{17, JNIXXTest{"JNIXXTest"}};
  test[5].field<jfloat>("j") = 2.7818;
  system.field<PrintStream>("out").method("println", test[5].method<String>("instance", jdouble{-1}));
  system.field<PrintStream>("out").method("println", find_class("java/util/Arrays").method<String>("toString", steal<jhandle<Object>>(test)));
  LOG("Test stealing: " << (void*)test);
  jhandle<jhandle<jhandle<JNIXXTest>>> test2{5, jhandle<jhandle<JNIXXTest>> {3, jhandle<JNIXXTest>{7, JNIXXTest{"JNIXXTest"}}}};
  LOG("Multid array: " << (void*)test2 << " length " << test2.length());
  int t = 0;
  for(jhandle<jhandle<JNIXXTest>> i : test2.to_vector())
    for(jhandle<JNIXXTest> j : i.to_vector())
      for(JNIXXTest k: j.to_vector())
	k.field<jfloat>("j") = t++;
  for(jhandle<jhandle<JNIXXTest>> i : test2)
    for(jhandle<JNIXXTest> j : i)
      for(JNIXXTest k: j)
	LOG(k.method<String>("instance", jdouble{-5}).to_string());

  auto e = [test2 = std::move(test2)]()
  {
    return test2.begin();
  }();

  exception_clear();
  jhandle<jhandle<JNIXXTest>> f = *e;
  system.field<PrintStream>("out").method("println", find_class("java/util/Arrays").method<String>("toString", steal<jhandle<Object>>(f)));

  system.field<PrintStream>("out").method("println", find_class("java/util/Arrays").method<String>("toString", steal<jhandle<Object>>(test2)));
  LOG("Test stealing: " << (void*)test2 << " length " << test2.length());

  exception_clear();

  struct Shell : jhandle<jobject>
  {
    using jhandle::jhandle;

    static std::string get_signature()
    {
      return make_signature(qualified_name, "org", "eclipse", "swt", "widgets", "Shell");
    }
  };

  static_assert(is_jhandle<Shell>, "Shell is not a jhandle?");

  struct FakeMenuClass : jhandle<jclass>
  {
    using jhandle::jhandle;

    static std::string get_signature()
    {
      return make_signature(qualified_name, "gui_test", "FakeMenu");
    }
  };

  auto fake_menu = define_class<FakeMenuClass>("gui_test/FakeMenu");
  LOG((void*) fake_menu);
//   exception_describe();

  jtry([]()
  {
    jhandle<jobject> menu{"gui_test/FakeMenu", Shell{}};
  })
  .jcatch([](jhandle<jthrowable> e)
  {
    LOG("Catch"); exception_print();
    return false;
  })
  .jcatch([&](jhandle<jthrowable> e)
  {
    LOG("Catch"); exception_print(e, *system.field<PrintStream>("out"));
    return true;
  })
  .jcatch([](jhandle<jthrowable> e)
  {
    LOG("Catch"); e.method<jvoid>("printStackTrace");
    return true;
  });

  system.field<PrintStream>("out").method("println", JNIXXTest{"JNIXXTest"}.object_class().method<String>("getCanonicalName"));
}
#endif
