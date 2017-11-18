#ifndef NO_JNI_HPP
#define NO_JNI_HPP

#include "no-jvm.hpp"

#include "../fundamental-machines/constexpr_string.hpp"

#include <functional>

template <size_t N> struct jPackage : cexprstr<char, N> {
  constexpr jPackage(const char (&s)[N]) : cexprstr<char, N>{s} {}
  constexpr jPackage(const cexprstr<char, N> s) : cexprstr<char, N>{s} {}
};
template <size_t N> jPackage(const char (&s)[N])->jPackage<N - 1>;

template <size_t M, size_t N>
constexpr auto operator/(jPackage<M> l, jPackage<N> r) {
  return jPackage{l + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(cexprstr<char, M> l, jPackage<N> r) {
  return jPackage{l + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(jPackage<M> l, cexprstr<char, N> r) {
  return jPackage{l + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(const char (&l)[M], jPackage<N> r) {
  return jPackage{jPackage{l} + "/" + r};
}
template <size_t M, size_t N>
constexpr auto operator/(jPackage<M> l, const char (&r)[N]) {
  return jPackage{l + "/" + jPackage{r}};
}

class jReference {
  static JNIEnv *env() { return JavaVirtualMachine::env; }
  jobjectRefType type = JNIInvalidRefType;
  jobject obj = nullptr;

  jReference() = default;

  friend class jMonitor;

public:
  jReference(jobject o) : type(env()->GetObjectRefType(o)) {
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
  ~jReference() {
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
    obj = nullptr;
  }

  jReference(const jReference &ref) : jReference(ref.obj) {}
  jReference(jReference &&ref) : type(ref.type), obj(ref.obj) {
    ref.type = JNIInvalidRefType, ref.obj = nullptr;
  }

  jReference &operator=(const jReference &ref) {
    this->~jReference();
    jReference tmp{ref};
    std::swap(obj, tmp.obj);
    return *this;
  }

  jReference &operator=(jReference &&ref) {
    this->~jReference();
    std::swap(obj, ref.obj);
    return *this;
  }

  bool is_local() const { return type == JNILocalRefType; }
  bool is_global() const { return type == JNIGlobalRefType; }
  bool is_weak() const { return type == JNIWeakGlobalRefType; }
  bool is_invalid() const { return type == JNIInvalidRefType; }

  jReference to_local() const {
    jReference ref;
    ref.obj = env()->NewLocalRef(obj);
    return ref;
  }
  jReference to_global() const {
    jReference ref;
    ref.obj = env()->NewGlobalRef(obj);
    return ref;
  }
  jReference to_weak() const {
    jReference ref;
    ref.obj = env()->NewWeakGlobalRef(obj);
    return ref;
  }
};

template <size_t N> struct jSignature_t : cexprstr<char, N> {
  constexpr jSignature_t(const char (&s)[N]) : cexprstr<char, N>{s} {}
  constexpr jSignature_t(cexprstr<char, N> s) : cexprstr<char, N>{s} {}
};

template <size_t N> jSignature_t(const char (&)[N])->jSignature_t<N - 1>;
template <size_t N> jSignature_t(cexprstr<char, N>)->jSignature_t<N>;

struct jvoid final {
  constexpr jvoid() = default;
};

template <typename T> struct make_signature {
  static constexpr bool is_member_function = false;
  constexpr make_signature() = default;
  constexpr auto operator()() const {
    if constexpr (std::is_same<T, jboolean>::value)
      return jSignature_t{"Z"};
    else if constexpr (std::is_same<T, jbyte>::value)
      return jSignature_t{"B"};
    else if constexpr (std::is_same<T, jchar>::value)
      return jSignature_t{"C"};
    else if constexpr (std::is_same<T, jshort>::value)
      return jSignature_t{"S"};
    else if constexpr (std::is_same<T, jint>::value)
      return jSignature_t{"I"};
    else if constexpr (std::is_same<T, jlong>::value)
      return jSignature_t{"J"};
    else if constexpr (std::is_same<T, jfloat>::value)
      return jSignature_t{"F"};
    else if constexpr (std::is_same<T, jdouble>::value)
      return jSignature_t{"D"};
    else if constexpr (std::is_same<T, jvoid>::value)
      return jSignature_t{"V"};
    else if constexpr (std::is_array<T>::value)
      return "[" + make_signature<std::remove_extent_t<T>>{}();
    else if constexpr (std::is_pointer<T>::value)
      return "[" + make_signature<std::remove_pointer_t<T>>{}();
    else if constexpr (std::is_same<decltype(T::ref), jReference>::value) {
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
  static constexpr bool is_member_function = true;
  constexpr make_signature() {}
  constexpr auto operator()() const {
    if constexpr (sizeof...(Args) == 0)
      return make_signature<T>{}() + "()" + make_signature<R>{}();
    else
      return make_signature<T>{}() + "(" + concat(make_signature<Args>{}()...) +
             ")" + make_signature<R>{}();
  }
};

template <typename T, size_t N> constexpr auto jFunction(const char (&s)[N]) {
  static_assert(make_signature<T>::is_member_function);
  return jSignature_t{s} + "\0" + make_signature<T>{}() + "\0";
}

template <typename, typename> struct to_mem_t;
template <typename C, typename R, typename... Args>
struct to_mem_t<C, R(Args...)> {
  using type = R C::*(Args...);
};
template <typename C, typename F> using to_mem = typename to_mem_t<C, F>::type;

template <typename C, typename T, size_t N>
constexpr auto jFunction(const char (&s)[N]) {
  return jFunction<to_mem<C, T>>(s);
}

template <typename T> class jClass {
  static JNIEnv *env() { return JavaVirtualMachine::env; }
  jReference ref;

  template <typename> friend class jObject;

public:
  using class_type = T;
  jClass() {
    if (!env())
      return;
    constexpr auto sig = make_signature<T>{}() + "\0";
    ref = jReference{env()->FindClass(sig.s)};
  }

  operator jclass() { return static_cast<jclass>(ref.obj); };
};

template <typename T> class jObject {
  static JNIEnv *env() { return JavaVirtualMachine::env; }

  jReference ref;

  template <typename> friend struct make_signature;
  friend class jMonitor;
  template <typename> friend class jObject;

  template <size_t N> static auto get_method(cexprstr<char, N> sig) {
    return env()->GetMethodID(getClass(), sig.s,
                              std::find(sig.s, sig.s + sig.size(), 0) + 1);
  }

  template <size_t... I>
  static void init_methods(std::index_sequence<I...>,
                           jmethodID (&m)[sizeof...(I)]) {
    [[maybe_unused]] auto ms = {
        (m[I] = get_method(T::method_signatures.template at<I>()))...};
  }

  template <size_t N> static jmethodID find_method(size_t i) {
    static jmethodID methods[N] = {0};
    if (!methods[i])
      init_methods(std::make_index_sequence<N>{},
                   methods); // Must be initialized at runtime, after JNI
                             // environment is established.

    return methods[i];
  }

public:
  using class_type = T;

  template <typename F, size_t N>
  constexpr static auto method(const char (&s)[N]) {
    return jFunction<class_type, F>(s);
  }

  static jClass<T> getClass() {
    static jClass<T> clazz;
    if (!clazz.ref.obj)
      clazz = jClass<T>{};
    return clazz;
  }

  operator void *() { return ref.obj; }

  template <typename R, size_t N, typename... Args>
  constexpr R call(const char (&s)[N], Args &&... args) const {
    static_assert(T::method_signatures.size());
    size_t method_index = T::method_signatures[jFunction<R T::*(Args...)>(s)];
    find_method<T::method_signatures.size()>(method_index);
    return {};
  }
};

template <typename T> constexpr auto jSignature = make_signature<T>{}();
template <typename T>
constexpr auto jSignature<jObject<T>> = make_signature<T>{}();

class jMonitor {
  const std::reference_wrapper<const jReference> ref;

public:
  jMonitor(const jMonitor &) = delete;
  jMonitor(jMonitor &&) = delete;
  jMonitor &operator=(const jMonitor &) = delete;
  jMonitor &operator=(jMonitor &&) = delete;

  jMonitor(const jReference &r) : ref(r) {
    JavaVirtualMachine::env->MonitorEnter(ref.get().obj);
  }
  template <typename T> jMonitor(const jObject<T> &h) : jMonitor(h.ref) {}
  ~jMonitor() { JavaVirtualMachine::env->MonitorExit(ref.get().obj); }
};

#endif
