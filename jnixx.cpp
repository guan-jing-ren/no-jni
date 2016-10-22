/*
Use `grep "..." jnixx.h` to compile this file, replace ... with compiler
time /home/kykwan/gcc/bin/g++ -x c++ -o libjnixx.so -O3 -std=c++14 -Wall -Wextra -Werror -pedantic-errors -Wl,-rpath=/home/kykwan/gcc/lib64' -m64 -shared -fPIC -I /usr/lib/jvm/java-1.8.0/include/ -I /usr/lib64/jvm/java-1.7.0/include/ -I/usr/lib64/jvm/java-1.7.0/include/linux -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/ -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/server jnixx.h
time clang++                  -x c++ -o libjnixx.so -O3 -std=c++14 -Wall -Wextra -Werror -pedantic-errors -Wl,-rpath=/home/kykwan/gcc/lib64' -m64 -shared -fPIC -I /usr/lib/jvm/java-1.8.0/include/ -I /usr/lib64/jvm/java-1.7.0/include/ -I/usr/lib64/jvm/java-1.7.0/include/linux -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/ -L/usr/lib64/jvm/java-1.7.0-openjdk-1.7.0/jre/lib/amd64/server -fcolor-diagnostics jnixx.h
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
constexpr bool is_jhandle = decltype(deduce_jhandle(std::declval<T>()))::value;

template<typename T>
constexpr bool is_jhandle<jhandle<T>> = true;

template<typename T>
constexpr bool is_jhandle_array = false;

template<typename T>
constexpr bool is_jhandle_array<jhandle<T>> = std::is_convertible<T, native_signature_t>{} || is_jhandle<T>;

template<typename T>
constexpr bool is_jhandle_string = false;

template<>
constexpr bool is_jhandle_string<jhandle<jstring>> = true;

// jstrings and jarrays are accessed through specialized JNI functions and not through jmethodID or jfieldID
template<typename T>
constexpr bool is_jhandle_plain = is_jhandle<T> && !is_jhandle_string<T> && !is_jhandle_array<T>;

template<typename T>
constexpr bool is_jhandle_class = false;

template<>
constexpr bool is_jhandle_class<jhandle<jclass>> = true;

template<typename T>
class jhandle
{
public:
  using value_type = T;

private:
  template<typename> friend class jhandle;
  jobject object{nullptr};

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
    std::enable_if_t<is_jhandle_plain<jhandle>, value_type> object = nullptr;		// Because field_proxy's can't be copied or moved after being returned, we can keep a raw jobject
    std::enable_if_t<is_jhandle_plain<jhandle>, jfieldID> id = nullptr;

    field_proxy(value_type o, jfieldID f) : object(o), id(f) {}

    // cast operators cannot be overridden and SFINAE disabled so it needs to defer to functions that can
    template<typename V = void>
    auto cast() const -> std::enable_if_t<std::is_arithmetic<U>{} && for_sfinae<V>, U>
    {
      return static_cast<U>(invoker<U>::get(object, id));
    }

    template<typename V = void>
    auto cast() const -> std::enable_if_t<is_jhandle<U> && for_sfinae<V>, U>
    {
      return U::steal(invoke_jni<jobject>::get(object, id));
    }

    // Due to the separation of the public field function and its private implementation necessitated by clang
    // jhandle needs to be able to return field_proxy's through move construction
    // Move construction still needs to be hidden to forbid dangling references
    field_proxy(field_proxy&&) = default;
  public:
    field_proxy() = delete;
    field_proxy(const field_proxy&) = delete;
    field_proxy& operator=(const field_proxy&) = delete;

    void operator=(field_proxy&& f)
    {
      *this = static_cast<U>(std::move(f));
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
    auto method(const char* name, Args&&... args) && -> std::enable_if_t<is_jhandle_plain<U>, R>
    {
      return cast().template method<R>(name, std::forward<Args>(args)...);
    }

    template<typename F, typename V = void>
    auto field(const char* name) && -> std::enable_if_t<is_jhandle_plain<U> && for_sfinae<V>, field_proxy<F>>
    {
      return cast().template field<F>(name);
    }
  };

  class element_proxy final
  {
    friend class jhandle;
    friend class java_array_iterator;
    using array_type = typename invoker<value_type>::array_type;
    std::enable_if_t<is_jhandle_array<jhandle>, array_type> array;
    jsize index;

    element_proxy(array_type a, jsize i) : array(a), index(i) {}
    element_proxy(element_proxy&&) = default;

    template<typename V = void>
    auto cast() const -> std::enable_if_t<std::is_arithmetic<value_type>{} && for_sfinae<V>, value_type>
    {
      return static_cast<value_type>(invoker<value_type>::get(array, index));
    }

    template<typename V = void>
    auto cast() const -> std::enable_if_t<is_jhandle<value_type> && for_sfinae<V>, value_type>
    {
      return value_type::steal(invoke_jni<jobject>::get(array, index));
    }

  public:
    element_proxy() = delete;
    element_proxy(const element_proxy&) = delete;
    element_proxy operator=(const element_proxy& e) = delete;

    void operator=(element_proxy&& e)
    {
      *this = static_cast<value_type>(std::move(e));
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

    template<typename F, typename V = void>
    auto field(const char* name) && -> std::enable_if_t<is_jhandle_plain<value_type> && for_sfinae<V>, field_proxy<F>>
    {
      return cast().template field<F>(name);
    }

    friend bool operator==(const element_proxy& l, const element_proxy& r)
    {
      return ::jni->IsSameObject(l.array, r.array) && l.index == r.index;
    }
  };

  // Methods and fields are implemented privately because clang complains about the signatures being the is_same
  // Overload resolution in clang seems to happen before functions are SFINAE removed from the overload resolution candidates
  // This only happens after specializing jhandle for jstring and Java arrays, but gcc is still okay with it
  // The trick is to have a dummy tag argument of integral type which differentiates the signature such that SFINAE removal can continue
  // The end result is still the same -- the compiler chooses the correct function to call
  // The side effect is that the public interface looks much simpler with only one method<>(...) function
  template<typename U, typename... Args>
  auto method_impl(char, const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && std::is_convertible<U, native_signature_t>{}, U>
  {
    auto m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U{}, method_args, args...).c_str());
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    return static_cast<U>(invoker<U>::call(object, m, convert_if_jobject(args)...));
  }

  template<typename U, typename... Args>
  auto method_impl(short, const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_class<jhandle> && std::is_convertible<U, native_signature_t>{}, U>
  {
    auto m = ::jni->GetStaticMethodID(static_cast<jclass>(object), name, make_signature(U{}, method_args, args...).c_str());
    if(m) return static_cast<U>(invoker<U>::call(static_cast<jclass>(object), m, convert_if_jobject(args)...));
    ::jni->ExceptionClear();
    m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U{}, method_args, args...).c_str());
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    return static_cast<U>(invoker<U>::call(object, m, convert_if_jobject(args)...));
  }

  template<typename U, typename... Args>
  auto method_impl(int, const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && is_jhandle<U>, U>
  {
    auto m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U{}, method_args, args...).c_str());
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    return U::steal(static_cast<typename U::value_type>(invoke_jni<jobject>::call(object, m, convert_if_jobject(args)...)));
  }

  template<typename U, typename... Args>
  auto method_impl(long, const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_class<jhandle> && is_jhandle<U>, U>
  {
    auto m = ::jni->GetStaticMethodID(static_cast<jclass>(object), name, make_signature(U{}, method_args, args...).c_str());
    if(m) return U::steal(static_cast<typename U::value_type>(invoke_jni<jobject>::call(static_cast<jclass>(object), m, convert_if_jobject(args)...)));
    ::jni->ExceptionClear();
    m = ::jni->GetMethodID(static_cast<jclass>(object_class()), name, make_signature(U{}, method_args, args...).c_str());
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    return U::steal(static_cast<typename U::value_type>(invoke_jni<jobject>::call(object, m, convert_if_jobject(args)...)));
  }

  template<typename U>
  auto field_impl(char, const char* name) const -> std::enable_if_t<is_jhandle_plain<jhandle> && !is_jhandle_class<jhandle> && (std::is_arithmetic<U>{} || is_jhandle<U>), field_proxy<U>>
  {
    auto f = ::jni->GetFieldID(object_class(), name, signature_of(U{}).c_str());
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    return {object, f};
  }

  template<typename U>
  auto field_impl(short, const char* name) const -> std::enable_if_t<is_jhandle_class<jhandle> && (std::is_arithmetic<U>{} || is_jhandle<U>), field_proxy<U>>
  {
    auto f = ::jni->GetStaticFieldID(static_cast<jclass>(object), name, signature_of(U{}).c_str());
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    return {static_cast<jclass>(object), f};
  }

  template<typename U>
  auto length_impl(char) const -> std::enable_if_t<is_jhandle_string<jhandle> && for_sfinae<U>, jsize>
  {
    return object ? ::jni->GetStringUTFLength(static_cast<jstring>(object)) : 0;
  }

  template<typename U>
  auto length_impl(short) const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, jsize>
  {
    return object ? ::jni->GetArrayLength(static_cast<jarray>(object)) : 0;
  }

  class java_array_iterator : public std::iterator<std::random_access_iterator_tag, value_type>
  {
    friend class jhandle;
    std::enable_if_t<is_jhandle_array<jhandle>, element_proxy> proxy;

    java_array_iterator(element_proxy&& p) : proxy(std::move(p)) {}
  public:
    java_array_iterator(const java_array_iterator& other) : proxy{other.proxy.array, other.proxy.index} {}

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

    decltype(proxy) operator[](std::make_signed_t<jsize> n) const
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
  jhandle() = default;

  // The use of dummy template argument U = void throughout the class is to turn functions into template functions
  // This gives the enable_if_t a dependent type and allow it to hide a function for inappropriate value_type's
  // Non-template functions cannot be hidden and cause template instantiation errors for innappropriate value_type's
  template<typename U = void, typename = std::enable_if_t<is_jhandle_plain<jhandle> && for_sfinae<U>>, typename... Args>
  jhandle(const std::string& name, Args&&... args)
  {
    auto klass = jhandle<jclass>::find_class(name);
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    auto m = ::jni->GetMethodID(klass, "<init>", make_signature(jvoid_arg, method_args, args...).c_str());
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
    object = ::jni->NewObject(klass, m, convert_if_jobject(args)...);
    if(::jni->ExceptionCheck()) LOG(""); ::jni->ExceptionDescribe();
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
    LOG("Object array: " << get_signature());
    LOG("Element: " << initial.object_class().template method<String>("getCanonicalName").to_string());
    LOG((void*)initial.object_class());
    object = invoke_jni<jobject>::array(len, initial.object_class(), initial);
  }

  template<typename H, typename = std::enable_if_t<std::is_base_of<jhandle, H>{}>>
  H& operator=(const H& other)
  {
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
    return *this;
  }

  template<typename H, typename = std::enable_if_t<std::is_base_of<jhandle, H>{}>>
  H& operator=(H&& other)
  {
    object = other.release();
    return *this;
  }

  template<typename H, typename = std::enable_if_t<std::is_base_of<jhandle, H>{}>>
  jhandle(const H& other)
  {
    (*this) = other;
  }

  template<typename H, typename = std::enable_if_t<std::is_base_of<jhandle, H>{}>>
  jhandle(H&& other)
  {
    (*this) = std::move(other);
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

  std::string get_signature() const
  {
    return is_jhandle_array<jhandle> ? "[" + signature_of(value_type{}) : std::string{"[nondescript object]"};
  }

  template<typename U, typename = std::enable_if_t<std::is_base_of<jhandle, U>{}>> operator U() const
  {
    return U{*this};
  }

  template<typename U>
  static jhandle steal(U&& t) {return steal(t);}

  template<typename U>
  static auto steal(U& t) -> std::enable_if_t<is_jhandle<U>, jhandle>
  {
    static_assert((is_jhandle_array<jhandle> && (is_jhandle_array<U> || std::is_same<typename U::value_type, jobject>{}))
		|| std::is_same<value_type, typename U::value_type>{}, "Can only acquire exact wrappee type");
    jhandle handle;
    handle.object = t.object;
    t.object = nullptr;
    return handle;
  }

  template<typename U>
  static auto steal(U& t) -> std::enable_if_t<std::is_pointer<U>{} && std::is_convertible<U, jobject>{}, jhandle>
  {
    static_assert((is_jhandle_array<jhandle> && (std::is_same<U, jobject>{} || std::is_convertible<U, jarray>{}))
		|| std::is_same<value_type, U>{}, "Can only acquire exact wrappee type");
    jhandle handle;
    handle.object = t;
    t = nullptr;
    return handle;
  }

  auto release()
  {
    auto released = object;
    object = nullptr;
    return static_cast<std::conditional_t<is_jhandle_array<jhandle>, jobject, value_type>>(released);
  }

  jhandle& globalize()
  {
    if(ref_type() == JNILocalRefType)
    {
      auto old = object;
      object = static_cast<value_type>(::jni->NewGlobalRef(old));
      ::jni->DeleteLocalRef(old);
    }
    return *this;
  }

  jhandle& localize()
  {
    if(ref_type() == JNIGlobalRefType)
    {
      auto old = object;
      object = static_cast<value_type>(::jni->NewLocalRef(old));
      ::jni->DeleteGlobalRef(old);
    }
    return *this;
  }

  ~jhandle()
  {
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

  template<typename U = void>
  static auto define_class(std::string name) -> std::enable_if_t<is_jhandle_class<jhandle> && for_sfinae<U>, jhandle>
  {
    static_assert(is_jhandle_class<jhandle>, "Must be called from jhandle<jclass> type");
    std::ifstream class_file{name + ".class", std::ifstream::binary | std::ifstream::in};
    std::stringbuf buffer;
    class_file >> &buffer;
    auto bytes = buffer.str();
    LOG("Bytes read: " << bytes.size());
    struct ClassLoader : jhandle<jobject>
    {
      using jhandle::jhandle;

      std::string get_signature() const
      {
	return make_signature(qualified_name, "java", "lang", "ClassLoader");
      }
    };
    return jhandle::steal(::jni->DefineClass(name.c_str(), jhandle::find_class("java/lang/ClassLoader").template method<ClassLoader>("getSystemClassLoader"), reinterpret_cast<const jbyte*>(bytes.c_str()), bytes.size()));
  }

  template<typename U = void>
  static auto find_class(const std::string& name) -> std::enable_if_t<is_jhandle_class<jhandle> && for_sfinae<U>, jhandle>
  {
    static_assert(is_jhandle_class<jhandle>, "Must be called from jhandle<jclass> type");
    return jhandle::steal(::jni->FindClass(name.c_str()));
  }

  auto object_class() const
  {
    return jhandle<jclass>::steal(object ? ::jni->GetObjectClass(object) : nullptr);
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
    return jhandle<jclass>::steal(object ? ::jni->GetSuperclass(static_cast<jclass>(object)) : nullptr);
  }

  template<typename U = void>
  auto assignable_from(const jhandle<jclass>& other) -> std::enable_if_t<is_jhandle_class<jhandle> && for_sfinae<U>, bool>
  {
    return ::jni->IsAssignableFrom(static_cast<jclass>(object), other);
  }

  template<typename U = jvoid, typename... Args>
  auto method(const char* name, Args&&... args) const -> std::enable_if_t<is_jhandle_plain<jhandle>, U>
  {
    return method_impl<U>(0, name, std::forward<Args>(args)...);
  }

  template<typename U>
  auto field(const char* name) const -> std::enable_if_t<is_jhandle_plain<jhandle>, field_proxy<U>>
  {
    return field_impl<U>(0, name);
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
    return length_impl<U>(0);
  }

  template<typename U = void>
  auto operator[](jsize i) const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, element_proxy>
  {
    return element_proxy{static_cast<typename element_proxy::array_type>(object), i};
  }

  template<typename U = void>
  auto begin() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return {{static_cast<typename element_proxy::array_type>(object), 0}};
  }

  template<typename U = void>
  auto end() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return begin() + length();
  }

  template<typename U = void>
  auto cbegin() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return begin();
  }

  template<typename U = void>
  auto cend() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return end();
  }

  template<typename U = void>
  auto rbegin() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return std::reverse_iterator<decltype(end())>(end());
  }

  template<typename U = void>
  auto rend() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return std::reverse_iterator<decltype(begin())>(begin());
  }

  template<typename U = void>
  auto crbegin() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return rbegin();
  }

  template<typename U = void>
  auto crend() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, java_array_iterator>
  {
    return rend();
  }

  template<typename U = void>
  auto to_vector() const -> std::enable_if_t<is_jhandle_array<jhandle> && for_sfinae<U>, std::vector<value_type>>
  {
    std::vector<value_type> v;
    std::copy(begin(), end(), std::back_inserter(v));
    return v;
  }
};

static_assert(sizeof(jhandle<jobject>) == sizeof(jobject), "jhandle<jobject> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jclass>) == sizeof(jobject), "jhandle<jclass> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jstring>) == sizeof(jobject), "jhandle<jstring> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jthread>) == sizeof(jobject), "jhandle<jthread> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jthreadGroup>) == sizeof(jobject), "jhandle<jthreadGroup> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jbyte>) == sizeof(jobject), "jhandle<jbyte> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jchar>) == sizeof(jobject), "jhandle<jchar> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jshort>) == sizeof(jobject), "jhandle<jshort> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jint>) == sizeof(jobject), "jhandle<jint> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jlong>) == sizeof(jobject), "jhandle<jlong> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jfloat>) == sizeof(jobject), "jhandle<jfloat> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jdouble>) == sizeof(jobject), "jhandle<jdouble> is not a thin wrapper of jobject");
static_assert(sizeof(jhandle<jhandle<jobject>>) == sizeof(jobject), "jhandle<jhandle<jobject>> is not a thin wrapper of jobject");

#ifndef JVMTI_ERROR_NONE
JNIEXPORT void JNICALL Java_JNIXXTest_do_1stuff(JNIEnv* env, jclass klass)
{

  ::jni = env;
  auto l = jhandle<jclass>::steal(klass);
  LOG((void*)klass);

  struct Object : jhandle<jobject>
  {
    using jhandle::jhandle;

    std::string get_signature() const
    {
      return make_signature(qualified_name, "java", "lang", "Object");
    }
  };

  struct PrintStream : jhandle<jobject>
  {
    using jhandle::jhandle;

    std::string get_signature() const
    {
      return make_signature(qualified_name, "java", "io", "PrintStream");
    }
  };

  struct String : jhandle<jstring>
  {
    using jhandle::jhandle;

    std::string get_signature() const
    {
      return make_signature(qualified_name, "java", "lang", "String");
    }
  };

  struct IntArray : jhandle<jint>
  {
    using jhandle::jhandle;
  };

  auto system = jhandle<jclass>::find_class("java/lang/System");
  system.field<PrintStream>("err") = system.field<PrintStream>("out");
  system.field<PrintStream>("out").method("println", jdouble{3.14159265358979});
  system.field<PrintStream>("out").method("println", l.method<String>("getCanonicalName"));
  system.field<PrintStream>("out").method("println", l.object_class().method<String>("getCanonicalName"));
  system.field<PrintStream>("out").method("println", system.method<String>("getCanonicalName"));

  l.method<jvoid>("do_other_stuff");
  l.field<jint>("i") = 200;
  system.field<PrintStream>("out").method("println", l.method<String>("return_string"));
  system.field<PrintStream>("out").method("println", *l.field<jint>("i"));
  PrintStream out = system.field<PrintStream>("out");
  LOG(std::boolalpha << "Output streams equal? " << (*system.field<PrintStream>("err") == *system.field<PrintStream>("out")));
  LOG(std::boolalpha << "Output streams equal? " << (*system.field<PrintStream>("err") == out));

  system.field<PrintStream>("out").method("println", Object{"JNIXXTest"});
  system.field<PrintStream>("out").method("println", String{"JNIXXTest as string"});
  system.field<PrintStream>("out").method("println", String{"JNIXXTest as string"}.length());
  LOG(String{"Converted jstring"}.to_string());

  LOG(signature_of(IntArray{12}));
  IntArray array{12};
  std::generate(array.begin(), array.end(), [i = 1]() mutable{return i *= 2;});
  system.field<PrintStream>("out").method("println", jhandle<jclass>::find_class("java/util/Arrays").method<String>("toString", array));
  array[1] = 42;
  array[9] = array[1];
  system.field<PrintStream>("out").method("println", jhandle<jclass>::find_class("java/util/Arrays").method<String>("toString", array));

  struct JNIXXTest : jhandle<jobject>
  {
    using jhandle::jhandle;

    std::string get_signature() const
    {
      return make_signature(qualified_name, "JNIXXTest");
    }
  };

//   (void)system[5];
  jhandle<JNIXXTest> test{17, JNIXXTest{"JNIXXTest"}};
  (*test[5]).field<jfloat>("j") = 2.7818;
  system.field<PrintStream>("out").method("println", test[5].method<String>("instance", jdouble{-1}));
  system.field<PrintStream>("out").method("println", jhandle<jclass>::find_class("java/util/Arrays").method<String>("toString", jhandle<Object>::steal(test)));
  LOG("Test stealing: " << (void*)test);
  jhandle<jhandle<jhandle<JNIXXTest>>> test2{5, jhandle<jhandle<JNIXXTest>> {3, jhandle<JNIXXTest>{7, JNIXXTest{"JNIXXTest"}}}};
  LOG("Multid array: " << (void*)test2 << " length " << test2.length());
  int t = 0;
  // FIXME: Converting to vector causes field "j" not to be found in JNIXXTest?
  for(jhandle<jhandle<JNIXXTest>> i : test2.to_vector())
    for(jhandle<JNIXXTest> j : i.to_vector())
      for(JNIXXTest k: j.to_vector())
      {
	/*LOG(k.method<String>("toString").to_string());*/ k.field<jfloat>("j") = t++;
      }
  for(jhandle<jhandle<JNIXXTest>> i : test2)
    for(jhandle<JNIXXTest> j : i)
      for(JNIXXTest k: j)
	LOG(k.method<String>("instance", jdouble{-5}).to_string());

  auto e = [test2 = std::move(test2)]()
  {
    return test2.begin();
  }();

  jhandle<jhandle<JNIXXTest>> f = *e;
  system.field<PrintStream>("out").method("println", jhandle<jclass>::find_class("java/util/Arrays").method<String>("toString", jhandle<Object>::steal(f)));

  system.field<PrintStream>("out").method("println", jhandle<jclass>::find_class("java/util/Arrays").method<String>("toString", jhandle<Object>::steal(test2)));
  LOG("Test stealing: " << (void*)test2 << " length " << test2.length());

  ::jni->ExceptionClear();

  struct Shell : jhandle<jobject>
  {
    using jhandle::jhandle;

    std::string get_signature() const
    {
      return make_signature(qualified_name, "org", "eclipse", "swt", "widgets", "Shell");
    }
  };

  static_assert(is_jhandle<Shell>, "Shell is not a jhandle?");

  auto fake_menu = jhandle<jclass>::define_class("gui_test/FakeMenu");
  LOG((void*) fake_menu);
  ::jni->ExceptionDescribe();
  jhandle<jobject> menu{"gui_test/FakeMenu", Shell{}};
  ::jni->ExceptionClear();
  system.field<PrintStream>("out").method("println", JNIXXTest{"JNIXXTest"}.object_class().method<String>("getCanonicalName"));
}
#endif
