#ifndef NO_JNI_HPP
#define NO_JNI_HPP

#include "no-jvm.hpp"

#include "../fundamental-machines/constexpr_string.hpp"

#include <functional>

template <size_t N> struct jpackage : cexprstr<char, N> {
  constexpr jpackage(const char (&s)[N]) : cexprstr<char, N>{s} {}
  constexpr jpackage(const cexprstr<char, N> s) : cexprstr<char, N>{s} {}
};
template <size_t N> jpackage(const char (&s)[N])->jpackage<N - 1>;

template <size_t M, size_t N>
constexpr auto operator/(jpackage<M> l, jpackage<N> r) {
  return jpackage{l + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(cexprstr<char, M> l, jpackage<N> r) {
  return jpackage{l + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(jpackage<M> l, cexprstr<char, N> r) {
  return jpackage{l + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(const char (&l)[M], jpackage<N> r) {
  return jpackage{jpackage{l} + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(jpackage<M> l, const char (&r)[N]) {
  return jpackage{l + "/" + jpackage{r}};
}

class jreference {
  static JNIEnv *env() { return JavaVirtualMachine::env; }
  jobjectRefType type = JNIInvalidRefType;
  jobject obj = nullptr;

  jreference() = default;

  friend class jmonitor;

public:
  jreference(jobject o) : type(env()->GetObjectRefType(o)) {
    switch (type) {
    default:
      break;
    case JNILocalRefType:
      obj = env()->NewLocalRef(o);
      break;
    case JNIGlobalRefType:
      obj = env()->NewGlobalRef(o);
      break;
    case JNIWeakGlobalRefType:
      obj = env()->NewWeakGlobalRef(o);
      break;
    case JNIInvalidRefType:
      break;
    }
  }
  ~jreference() {
    if (!obj)
      return;
    switch (type) {
    default:
      break;
    case JNILocalRefType:
      env()->DeleteLocalRef(obj);
      break;
    case JNIGlobalRefType:
      env()->DeleteGlobalRef(obj);
      break;
    case JNIWeakGlobalRefType:
      env()->DeleteWeakGlobalRef(obj);
      break;
    case JNIInvalidRefType:
      break;
    }
  }

  jreference(const jreference &ref) : jreference(ref.obj) {}
  jreference(jreference &&ref) : type(ref.type), obj(ref.obj) {
    ref.type = JNIInvalidRefType, ref.obj = nullptr;
  }

  bool is_local() const { return type == JNILocalRefType; }
  bool is_global() const { return type == JNIGlobalRefType; }
  bool is_weak() const { return type == JNIWeakGlobalRefType; }
  bool is_invalid() const { return type == JNIInvalidRefType; }

  jreference to_local() const {
    jreference ref;
    ref.obj = env()->NewLocalRef(obj);
    return ref;
  }
  jreference to_global() const {
    jreference ref;
    ref.obj = env()->NewGlobalRef(obj);
    return ref;
  }
  jreference to_weak() const {
    jreference ref;
    ref.obj = env()->NewWeakGlobalRef(obj);
    return ref;
  }
};

template <size_t N> struct jsignature_t : cexprstr<char, N> {
  constexpr jsignature_t(const char (&s)[N]) : cexprstr<char, N>{s} {}
  constexpr jsignature_t(const cexprstr<char, N> s) : cexprstr<char, N>{s} {}
};

template <size_t N> jsignature_t(const char (&s)[N])->jsignature_t<N - 1>;
struct jvoid final {
  constexpr jvoid() = default;
};

template <typename T> struct make_signature {
  constexpr make_signature() = default;
  constexpr auto operator()() const {
    if constexpr (std::is_same<T, jboolean>::value)
      return jsignature_t{"Z"};
    else if constexpr (std::is_same<T, jbyte>::value)
      return jsignature_t{"B"};
    else if constexpr (std::is_same<T, jchar>::value)
      return jsignature_t{"C"};
    else if constexpr (std::is_same<T, jshort>::value)
      return jsignature_t{"S"};
    else if constexpr (std::is_same<T, jint>::value)
      return jsignature_t{"I"};
    else if constexpr (std::is_same<T, jlong>::value)
      return jsignature_t{"J"};
    else if constexpr (std::is_same<T, jfloat>::value)
      return jsignature_t{"F"};
    else if constexpr (std::is_same<T, jdouble>::value)
      return jsignature_t{"D"};
    else if constexpr (std::is_same<T, jvoid>::value)
      return jsignature_t{"V"};
    else if constexpr (std::is_array<T>::value)
      return "[" + make_signature<std::remove_extent_t<T>>{}();
    else if constexpr (std::is_pointer<T>::value)
      return "[" + make_signature<std::remove_pointer_t<T>>{}();
    else if constexpr (std::is_same<decltype(T::ref), jreference>::value) {
      if constexpr (std::is_array<typename T::class_type>::value ||
                    std::is_pointer<typename T::class_type>::value)
        return make_signature<typename T::class_type>{}();
      else
        return "L" + T::signature + ";";
    } else {
      struct unsupported {};
      return unsupported{};
    }
  }
};

template <typename R, typename T, typename... Args>
struct make_signature<R T::*(Args...)> {
  constexpr make_signature() {}
  constexpr auto operator()() const {
    if constexpr (sizeof...(Args) == 0)
      return make_signature<T>{}() + "()" + make_signature<R>{}();
    else
      return make_signature<T>{}() + "(" + concat(make_signature<Args>{}()...) +
             ")" + make_signature<R>{}();
  }
};

template <typename R, typename T, typename... Args, size_t N>
constexpr auto jfunction(const char (&s)[N]) {
  return jsignature_t{s} + " " + make_signature<R T::*(Args...)>{}();
}

template <typename T> class jhandle {
  using class_type = T;
  jreference ref;

  template <typename> friend struct make_signature;
  friend class jmonitor;
  template <typename> friend class jhandle;

  template <size_t N> static jmethodID find_method(size_t i) {
    static jmethodID methods[N] = {0};
    if (!methods[i])
      ;
    return methods[i];
  }

public:
  template <typename R, size_t N, typename... Args>
  constexpr R call(const char (&s)[N], Args &&... args) const {
    static_assert(T::method_signatures.size());
    std::cout << jfunction<R, T, Args...>(s) << '\n';
    size_t method_index = T::method_signatures[jfunction<R, T, Args...>(s)];
    find_method<T::method_signatures.size()>(method_index);
    return {};
  }
};

template <typename T> constexpr auto jsignature = make_signature<T>{}();
template <typename T>
constexpr auto jsignature<jhandle<T>> = make_signature<T>{}();

class jmonitor {
  const std::reference_wrapper<const jreference> ref;

public:
  jmonitor(const jmonitor &) = delete;
  jmonitor(jmonitor &&) = delete;
  jmonitor &operator=(const jmonitor &) = delete;
  jmonitor &operator=(jmonitor &&) = delete;

  jmonitor(const jreference &r) : ref(r) {
    JavaVirtualMachine::env->MonitorEnter(ref.get().obj);
  }
  template <typename T> jmonitor(const jhandle<T> &h) : jmonitor(h.ref) {}
  ~jmonitor() { JavaVirtualMachine::env->MonitorExit(ref.get().obj); }
};

#endif
