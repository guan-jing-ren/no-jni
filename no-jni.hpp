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
  template <typename, typename> friend class jClass;
  template <typename, typename> friend class jObject;

public:
  jReference(jobject o) {
    if (!o)
      return;
    obj = env()->NewGlobalRef(o);
    type = env()->GetObjectRefType(obj);
  }
  ~jReference() {
    if (!obj)
      return;
    if (!env())
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
    type = JNIInvalidRefType;
    obj = nullptr;
  }

  jReference(const jReference &ref) : jReference(ref.obj) {}
  jReference(jReference &&ref) : type(ref.type), obj(ref.obj) {
    ref.type = JNIInvalidRefType, ref.obj = nullptr;
  }

  jReference &operator=(const jReference &ref) {
    this->~jReference();
    jReference tmp{ref};
    std::swap(type, tmp.type);
    std::swap(obj, tmp.obj);
    return *this;
  }

  jReference &operator=(jReference &&ref) {
    this->~jReference();
    std::swap(type, ref.type);
    std::swap(obj, ref.obj);
    return *this;
  }

  template <typename J> static jReference steal(J &j) {
    static_assert(std::is_convertible<J, jobject>::value);
    jobject o = nullptr;
    std::swap(o, reinterpret_cast<jobject &>(j));
    jReference ref;
    std::swap(ref.obj, o);
    ref.type = ref.obj ? env()->GetObjectRefType(ref.obj) : JNIInvalidRefType;
    return ref;
  }

  operator jobject() const { return obj; }
  operator bool() const { return obj; }

  bool is_local() const { return type == JNILocalRefType; }
  bool is_global() const { return type == JNIGlobalRefType; }
  bool is_weak() const { return type == JNIWeakGlobalRefType; }
  bool is_invalid() const { return type == JNIInvalidRefType; }

  jReference to_local() const {
    jReference ref;
    ref.obj = env()->NewLocalRef(obj);
    ref.type = env()->GetObjectRefType(obj);
    return ref;
  }
  jReference to_global() const {
    jReference ref;
    ref.obj = env()->NewGlobalRef(obj);
    ref.type = env()->GetObjectRefType(obj);
    return ref;
  }
  jReference to_weak() const {
    jReference ref;
    ref.obj = env()->NewWeakGlobalRef(obj);
    ref.type = env()->GetObjectRefType(obj);
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

template <typename T> constexpr auto signature = T::signature;

template <typename T> typename T::ref has_ref(std::nullptr_t = nullptr);
template <typename T> jvoid has_ref(std::nullptr_t = nullptr);

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
    else if constexpr (std::is_same<decltype(has_ref<T>()),
                                    jReference>::value) {
      if constexpr (std::is_array<typename T::class_type>::value ||
                    std::is_pointer<typename T::class_type>::value)
        return make_signature<typename T::class_type>{}();
      else
        return "L" + signature<T> + ";";
    } else {
      return "L" + signature<T> + ";";
    }
  }
};

template <typename R, typename... Args> struct make_signature<R(Args...)> {
  static constexpr bool is_member_function = true;
  constexpr make_signature() = default;
  constexpr auto operator()() const {
    if constexpr (sizeof...(Args) == 0)
      return "()" + make_signature<R>{}();
    else
      return "(" + concat(make_signature<Args>{}()...) + ")" +
             make_signature<R>{}();
  }
};

template <typename, bool> class Element;
template <typename T, bool A> struct make_signature<Element<T, A>> {
  constexpr make_signature() = default;
  constexpr auto operator()() const { return make_signature<T>{}(); }
};
template <bool, typename> class Field;
template <bool S, typename F> struct make_signature<Field<S, F>> {
  constexpr make_signature() = default;
  constexpr auto operator()() const { return make_signature<F>{}(); }
};

template <bool IsFunction, typename T, size_t N>
constexpr auto jMember(const char (&name)[N]) {
  static_assert(make_signature<T>::is_member_function == IsFunction);
  return jSignature_t{name} + "\0" + make_signature<T>{}() + "\0";
}

template <typename T, size_t N> constexpr auto jMethod(const char (&name)[N]) {
  return jMember<true, T>(name);
}

template <typename T, size_t N> constexpr auto jField(const char (&name)[N]) {
  return jMember<false, T>(name);
}

namespace java::lang {
class Object;
}
class String;
template <typename Class, typename SuperClass = java::lang::Object>
class jClass {
  static JNIEnv *env() { return JavaVirtualMachine::env; }
  jReference ref;

  template <typename> friend struct make_signature;
  friend class jMonitor;
  template <typename, typename> friend class jObject;

  static constexpr auto deduce_signature() {
    if constexpr (std::is_array<class_type>::value)
      return make_signature<superclass_type>{}();
    else
      return ::signature<class_type>;
    ;
  }

public:
  using class_type = Class;
  using superclass_type = SuperClass;
  static constexpr auto signature = deduce_signature();

  jClass() {
    if (!env())
      return;
    constexpr auto sig = signature + "\0";
    ref = jReference{env()->FindClass(sig.s)};
  }

  operator jclass() const & { return static_cast<jclass>(ref.obj); };

  static superclass_type getSuperClass() { return {}; }
};

template <typename R> static constexpr auto static_caller() {
  if constexpr (std::is_same<R, jboolean>::value)
    return &JNIEnv::CallStaticBooleanMethod;
  else if constexpr (std::is_same<R, jbyte>::value)
    return &JNIEnv::CallStaticByteMethod;
  else if constexpr (std::is_same<R, jchar>::value)
    return &JNIEnv::CallStaticCharMethod;
  else if constexpr (std::is_same<R, jshort>::value)
    return &JNIEnv::CallStaticShortMethod;
  else if constexpr (std::is_same<R, jint>::value)
    return &JNIEnv::CallStaticIntMethod;
  else if constexpr (std::is_same<R, jlong>::value)
    return &JNIEnv::CallStaticLongMethod;
  else if constexpr (std::is_same<R, jfloat>::value)
    return &JNIEnv::CallStaticFloatMethod;
  else if constexpr (std::is_same<R, jdouble>::value)
    return &JNIEnv::CallStaticDoubleMethod;
  else if constexpr (std::is_same<R, jvoid>::value)
    return &JNIEnv::CallStaticVoidMethod;
  else
    return &JNIEnv::CallStaticObjectMethod;
}

template <typename R> static constexpr auto caller() {
  if constexpr (std::is_same<R, jboolean>::value)
    return &JNIEnv::CallBooleanMethod;
  else if constexpr (std::is_same<R, jbyte>::value)
    return &JNIEnv::CallByteMethod;
  else if constexpr (std::is_same<R, jchar>::value)
    return &JNIEnv::CallCharMethod;
  else if constexpr (std::is_same<R, jshort>::value)
    return &JNIEnv::CallShortMethod;
  else if constexpr (std::is_same<R, jint>::value)
    return &JNIEnv::CallIntMethod;
  else if constexpr (std::is_same<R, jlong>::value)
    return &JNIEnv::CallLongMethod;
  else if constexpr (std::is_same<R, jfloat>::value)
    return &JNIEnv::CallFloatMethod;
  else if constexpr (std::is_same<R, jdouble>::value)
    return &JNIEnv::CallDoubleMethod;
  else if constexpr (std::is_same<R, jvoid>::value)
    return &JNIEnv::CallVoidMethod;
  else
    return &JNIEnv::CallObjectMethod;
}

template <bool S, typename F> static constexpr auto getter() {
  if constexpr (S) {
    if constexpr (std::is_same<F, jboolean>::value)
      return &JNIEnv::GetStaticBooleanField;
    else if constexpr (std::is_same<F, jbyte>::value)
      return &JNIEnv::GetStaticByteField;
    else if constexpr (std::is_same<F, jchar>::value)
      return &JNIEnv::GetStaticCharField;
    else if constexpr (std::is_same<F, jshort>::value)
      return &JNIEnv::GetStaticShortField;
    else if constexpr (std::is_same<F, jint>::value)
      return &JNIEnv::GetStaticIntField;
    else if constexpr (std::is_same<F, jlong>::value)
      return &JNIEnv::GetStaticLongField;
    else if constexpr (std::is_same<F, jfloat>::value)
      return &JNIEnv::GetStaticFloatField;
    else if constexpr (std::is_same<F, jdouble>::value)
      return &JNIEnv::GetStaticDoubleField;
    else
      return &JNIEnv::GetStaticObjectField;
  } else {
    if constexpr (std::is_same<F, jboolean>::value)
      return &JNIEnv::GetBooleanField;
    else if constexpr (std::is_same<F, jbyte>::value)
      return &JNIEnv::GetByteField;
    else if constexpr (std::is_same<F, jchar>::value)
      return &JNIEnv::GetCharField;
    else if constexpr (std::is_same<F, jshort>::value)
      return &JNIEnv::GetShortField;
    else if constexpr (std::is_same<F, jint>::value)
      return &JNIEnv::GetIntField;
    else if constexpr (std::is_same<F, jlong>::value)
      return &JNIEnv::GetLongField;
    else if constexpr (std::is_same<F, jfloat>::value)
      return &JNIEnv::GetFloatField;
    else if constexpr (std::is_same<F, jdouble>::value)
      return &JNIEnv::GetDoubleField;
    else
      return &JNIEnv::GetObjectField;
  }
}

template <bool S, typename F> static constexpr auto setter() {
  if constexpr (S) {
    if constexpr (std::is_same<F, jboolean>::value)
      return &JNIEnv::SetStaticBooleanField;
    else if constexpr (std::is_same<F, jbyte>::value)
      return &JNIEnv::SetStaticByteField;
    else if constexpr (std::is_same<F, jchar>::value)
      return &JNIEnv::SetStaticCharField;
    else if constexpr (std::is_same<F, jshort>::value)
      return &JNIEnv::SetStaticShortField;
    else if constexpr (std::is_same<F, jint>::value)
      return &JNIEnv::SetStaticIntField;
    else if constexpr (std::is_same<F, jlong>::value)
      return &JNIEnv::SetStaticLongField;
    else if constexpr (std::is_same<F, jfloat>::value)
      return &JNIEnv::SetStaticFloatField;
    else if constexpr (std::is_same<F, jdouble>::value)
      return &JNIEnv::SetStaticDoubleField;
    else
      return &JNIEnv::SetStaticObjectField;
  } else {
    if constexpr (std::is_same<F, jboolean>::value)
      return &JNIEnv::SetBooleanField;
    else if constexpr (std::is_same<F, jbyte>::value)
      return &JNIEnv::SetByteField;
    else if constexpr (std::is_same<F, jchar>::value)
      return &JNIEnv::SetCharField;
    else if constexpr (std::is_same<F, jshort>::value)
      return &JNIEnv::SetShortField;
    else if constexpr (std::is_same<F, jint>::value)
      return &JNIEnv::SetIntField;
    else if constexpr (std::is_same<F, jlong>::value)
      return &JNIEnv::SetLongField;
    else if constexpr (std::is_same<F, jfloat>::value)
      return &JNIEnv::SetFloatField;
    else if constexpr (std::is_same<F, jdouble>::value)
      return &JNIEnv::SetDoubleField;
    else
      return &JNIEnv::SetObjectField;
  }
}

template <typename F> static constexpr auto array_getter() {
  if constexpr (std::is_same<F, jboolean>::value)
    return &JNIEnv::GetBooleanArrayRegion;
  else if constexpr (std::is_same<F, jbyte>::value)
    return &JNIEnv::GetByteArrayRegion;
  else if constexpr (std::is_same<F, jchar>::value)
    return &JNIEnv::GetCharArrayRegion;
  else if constexpr (std::is_same<F, jshort>::value)
    return &JNIEnv::GetShortArrayRegion;
  else if constexpr (std::is_same<F, jint>::value)
    return &JNIEnv::GetIntArrayRegion;
  else if constexpr (std::is_same<F, jlong>::value)
    return &JNIEnv::GetLongArrayRegion;
  else if constexpr (std::is_same<F, jfloat>::value)
    return &JNIEnv::GetFloatArrayRegion;
  else if constexpr (std::is_same<F, jdouble>::value)
    return &JNIEnv::GetDoubleArrayRegion;
  else
    return &JNIEnv::GetObjectArrayElement;
}

template <typename F> static constexpr auto array_setter() {
  if constexpr (std::is_same<F, jboolean>::value)
    return &JNIEnv::SetBooleanArrayRegion;
  else if constexpr (std::is_same<F, jbyte>::value)
    return &JNIEnv::SetByteArrayRegion;
  else if constexpr (std::is_same<F, jchar>::value)
    return &JNIEnv::SetCharArrayRegion;
  else if constexpr (std::is_same<F, jshort>::value)
    return &JNIEnv::SetShortArrayRegion;
  else if constexpr (std::is_same<F, jint>::value)
    return &JNIEnv::SetIntArrayRegion;
  else if constexpr (std::is_same<F, jlong>::value)
    return &JNIEnv::SetLongArrayRegion;
  else if constexpr (std::is_same<F, jfloat>::value)
    return &JNIEnv::SetFloatArrayRegion;
  else if constexpr (std::is_same<F, jdouble>::value)
    return &JNIEnv::SetDoubleArrayRegion;
  else
    return &JNIEnv::SetObjectArrayElement;
}

template <typename F> static constexpr auto alloc() {
  if constexpr (std::is_same<F, jboolean>::value)
    return &JNIEnv::NewBooleanArray;
  else if constexpr (std::is_same<F, jbyte>::value)
    return &JNIEnv::NewByteArray;
  else if constexpr (std::is_same<F, jchar>::value)
    return &JNIEnv::NewCharArray;
  else if constexpr (std::is_same<F, jshort>::value)
    return &JNIEnv::NewShortArray;
  else if constexpr (std::is_same<F, jint>::value)
    return &JNIEnv::NewIntArray;
  else if constexpr (std::is_same<F, jlong>::value)
    return &JNIEnv::NewLongArray;
  else if constexpr (std::is_same<F, jfloat>::value)
    return &JNIEnv::NewFloatArray;
  else if constexpr (std::is_same<F, jdouble>::value)
    return &JNIEnv::NewDoubleArray;
  else
    return &JNIEnv::NewObjectArray;
}

static jobject cast(void *o) { return static_cast<jobject>(o); }
template <typename T>
static auto cast(T t) -> std::enable_if_t<std::is_arithmetic<T>::value, T> {
  return t;
}
template <typename T, bool A> auto cast(Element<T, A> e) { return cast(*e); }
template <bool S, typename T> auto cast(const Field<S, T> &f) {
  return cast(*f);
}

template <bool S, typename F> class Field {
  static JNIEnv *env() { return JavaVirtualMachine::env; }

  constexpr static auto get = getter<S, F>();
  constexpr static auto set = setter<S, F>();

  std::conditional_t<S, jclass, jobject> obj;
  jfieldID id;

  Field(decltype(obj) o, jfieldID f) : obj(o), id(f) {}

  template <typename, typename> friend class jObject;

public:
  Field(const Field &) = delete;
  Field(Field &&) = delete;
  Field &operator=(const Field &) = delete;
  Field &operator=(Field &&) = delete;

  template <typename FF> F operator=(FF &&f) {
    (env()->*set)(obj, id, cast(f));
    return f;
  }
  operator F const() { return **this; }
  F operator*() const { return (env()->*get)(obj, id); }
};

template <typename E, bool A = std::is_arithmetic<E>::value> class Element {
  static JNIEnv *env() { return JavaVirtualMachine::env; }

  constexpr static auto get = array_getter<E>();
  constexpr static auto set = array_setter<E>();

  std::conditional_t<A, jarray, jobjectArray> obj;
  jsize idx;

  Element(jobject o, jsize i) : obj(static_cast<decltype(obj)>(o)), idx(i) {}

  template <typename, typename> friend class jObject;
  template <typename, bool> friend class Iterator;

  Element(const Element &) = default;
  Element(Element &&) = default;

  auto ref() const { return jReference{(env()->*get)(obj, idx)}; }

public:
  Element &operator=(const Element &other) {
    (*this) = *other;
    return *this;
  }
  Element &operator=(Element &&other) {
    (*this) = *other;
    return *this;
  }

  E operator=(const E &elem) {
    if constexpr (A)
      (env()->*set)(obj, idx, 1, &elem);
    else
      (env()->*set)(obj, idx, cast(elem));
    return elem;
  }

  operator E() const { return **this; }

  E operator*() const {
    E elem;
    if constexpr (A)
      (env()->*get)(obj, idx, 1, &elem);
    else
      elem.ref = ref();
    return elem;
  }

  bool operator==(const Element &e) const {
    return obj == e.obj && idx == e.idx;
  }

  bool operator!=(const Element &e) const { return !(*this == e); }
};

template <typename E, bool A>
std::ostream &operator<<(std::ostream &out, const Element<E, A> &e) {
  return out << *e;
}

template <typename E, bool A = std::is_arithmetic<E>::value> class Iterator {
  Element<E, A> element;

  Iterator(const Element<E, A> elem) : element(elem) {}

  template <typename, typename> friend class jObject;

public:
  Iterator(const Iterator &) = default;
  Iterator(Iterator &&) = default;
  Iterator &operator=(const Iterator &) = default;
  Iterator &operator=(Iterator &&) = default;

  Element<E, A> operator*() const { return element; }
  Iterator &operator++() {
    ++element.idx;
    return *this;
  }
  Iterator &operator--() {
    --element.idx;
    return *this;
  }

  std::ptrdiff_t operator-(const Iterator &i) const {
    return element.idx - i.element.idx;
  }

  bool operator==(const Iterator &e) const {
    return element.obj == e.element.obj && element.idx == e.element.idx;
  }

  bool operator!=(const Iterator &e) const { return !(*this == e); }

  bool operator<(const Iterator &e) const {
    return element.idx < e.element.idx;
  }

  Iterator operator+(std::ptrdiff_t n) {
    Iterator i = *this;
    i.element.idx += n;
    return i;
  }

  Iterator &operator+=(std::ptrdiff_t n) { return (*this) = (*this) + n; }

  auto operator[](std::ptrdiff_t n) { return (*this) + n; }
};

template <typename E, bool A> struct std::iterator_traits<Iterator<E, A>> {
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = ptrdiff_t;
  using value_type = E;
  using reference = E;
  using pointer = Iterator<E, A>;
};

template <typename T> constexpr auto method_signatures = T::method_signatures;
template <typename T> constexpr auto field_signatures = T::field_signatures;

template <typename Class, typename SuperClass = java::lang::Object>
class jObject {
  jReference ref;

  template <typename> friend struct make_signature;
  friend class jMonitor;
  template <typename, typename> friend class jObject;
  template <typename, bool> friend class Element;
  friend class String;

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *), size_t N>
  static auto get_member(cexprstr<char, N> sig, jclass c) {
    return (env()->*getter)(c, sig.s,
                            std::find(sig.s, sig.s + sig.size(), 0) + 1);
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            size_t... I, typename E>
  static void init_members(std::index_sequence<I...>, jclass c,
                           G (&m)[sizeof...(I)], E e) {
    [[maybe_unused]] auto ms = {
        (m[I] = get_member<G, getter>(e.template at<I>(), c))...};
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            typename E>
  static G find_member(jclass c, size_t i, E e) {
    constexpr size_t N = E::size();
    static_assert(N);
    if (i >= N)
      return nullptr;
    static G members[N] = {0};
    if (!members[i])
      init_members<G, getter>(std::make_index_sequence<N>{}, c, members,
                              e); // Must be initialized at runtime, after JNI
                                  // environment is established.
    return members[i];
  }

  template <typename G, typename F, size_t N, typename E>
  constexpr static auto member_index(const char (&s)[N], E e) {
    return e[jMember<std::is_same<G, jmethodID>::value, F>(s)];
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            typename F, size_t N>
  static G get_member(const char (&s)[N], jclass c) {
    G m = nullptr;
    if constexpr (std::is_same<G, jmethodID>::value)
      m = find_member<G, getter>(
          c, member_index<G, F>(s, method_signatures<class_type>),
          method_signatures<class_type>);
    else if constexpr (std::is_same<G, jfieldID>::value)
      m = find_member<G, getter>(
          c, member_index<G, F>(s, field_signatures<class_type>),
          field_signatures<class_type>);
    if constexpr (!std::is_same<class_type, superclass_type>::value)
      if (!m)
        m = superclass_type::template get_member<G, getter, F>(s, c);
    if (!m) {
      const auto sig = jMember<std::is_same<G, jmethodID>::value, F>(s);
      m = get_member<G, getter>(sig, c);
    }
    return m;
  }

  template <bool S,
            jfieldID (JNIEnv::*getter)(jclass, const char *, const char *),
            typename F, size_t N, typename C>
  static Field<S, F> at_(const char (&s)[N], C &&context) {
    static_assert(!std::is_array<class_type>::value);
    auto f = get_member<jfieldID, getter, F>(s, getClass());
    return Field<S, F>{context, f};
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            typename R, size_t N, typename F, typename C, typename... Args>
  static R call_(const char (&s)[N], F f, C &&context, Args &&... args) {
    auto m = get_member<G, getter, R(std::decay_t<Args>...)>(s, getClass());
    if (!m)
      return {};
    if constexpr (std::is_same<R, jvoid>::value) {
      (env()->*f)(context, m, cast(args)...);
      return {};
    } else
      return {(env()->*f)(context, m, cast(args)...)};
  }

protected:
  static JNIEnv *env() { return JavaVirtualMachine::env; }
  operator void *() const { return ref.obj; }
  jObject(jobject o) : ref(o) {}

public:
  using class_type = Class;
  using superclass_type = SuperClass;
  jObject() = default;
  jObject(const jObject &) = default;
  jObject(jObject &&) = default;
  jObject &operator=(const jObject &) = default;
  jObject &operator=(jObject &&) = default;
  template <bool S> jObject(const Element<class_type, S> &e) : ref(e.ref()) {}
  template <bool S> jObject(Element<class_type, S> &&e) : ref(e.ref()) {}
  jObject(const class_type &o) : jObject(static_cast<jObject>(o)) {}
  jObject(class_type &&o) : jObject(static_cast<jObject>(o)) {}
  static void *operator new(std::size_t sz) = delete;

  template <typename... Args> constexpr static auto jConstructor() {
    return jMethod<jvoid(Args...)>("<init>");
  }

  static const jClass<class_type> &getClass() {
    static jClass<class_type> clazz;
    if (!clazz.ref.obj)
      clazz = jClass<class_type>{};
    return clazz;
  }

  template <typename... Args> jObject(Args &&... args) {
    if constexpr (std::is_array<class_type>::value) {
      static_assert(sizeof...(Args) == 1);
      static_assert(std::is_same<Args..., jsize>::value);
      using raw_elem_type = std::remove_extent_t<class_type>;
      using elem_type =
          std::conditional_t<std::is_pointer<raw_elem_type>::value,
                             std::remove_pointer_t<raw_elem_type>[],
                             raw_elem_type>;

      if constexpr (std::is_arithmetic<elem_type>::value)
        ref = jReference{(env()->*alloc<elem_type>())(args...)};
      else
        ref = jReference{
            (env()->*alloc<elem_type>())(args..., getClass(), nullptr)};
    } else {
      ref = jReference{env()->NewObject(
          getClass(),
          get_member<jmethodID, &JNIEnv::GetMethodID,
                     jvoid(std::decay_t<Args>...)>("<init>", getClass()),
          cast(args)...)};
    }
  }

  operator std::string() const {
    const auto print_null = [](auto dotted) {
      for (auto &c : dotted.s)
        if (c == '/')
          c = '.';
      return std::string{dotted.s, dotted.s + dotted.size()} + "@0";
    };
    if constexpr (std::is_array<class_type>::value) {
      if (!ref.obj)
        return print_null("L" + make_signature<class_type>{}() + ";");
      auto m =
          env()->GetMethodID(getClass(), "toString", "()Ljava/lang/String;");
      return std::string(String{env()->CallObjectMethod(ref.obj, m)});
    } else {
      if (!ref.obj) {
        return print_null(signature<class_type>);
      }
      return call<String>("toString");
    }
  }

  template <typename F, size_t N>
  static Field<true, F> sat(const char (&s)[N]) {
    return at_<true, &JNIEnv::GetStaticFieldID, F>(s, getClass());
  }

  template <typename F, size_t N> Field<false, F> at(const char (&s)[N]) const {
    return at_<false, &JNIEnv::GetFieldID, F>(s, ref.obj);
  }

  template <typename R, size_t N, typename... Args>
  static R scall(const char (&s)[N], Args &&... args) {
    static_assert(!std::is_array<class_type>::value);
    return call_<jmethodID, &JNIEnv::GetStaticMethodID, R>(
        s, static_caller<R>(), getClass(), std::forward<Args>(args)...);
  }

  template <typename R, size_t N, typename... Args>
  R call(const char (&s)[N], Args &&... args) const {
    return call_<jmethodID, &JNIEnv::GetMethodID, R>(
        s, caller<R>(), ref.obj, std::forward<Args>(args)...);
  }

  template <bool A = std::is_array<class_type>::value>
  auto size() const -> std::enable_if_t<A, jsize> {
    static_assert(std::is_array<class_type>::value);
    return env()->GetArrayLength(static_cast<jarray>(ref.obj));
  }

  auto operator[](jsize i) const {
    static_assert(std::is_array<class_type>::value);
    using raw_elem_type = std::remove_extent_t<class_type>;
    using elem_type = std::conditional_t<std::is_pointer<raw_elem_type>::value,
                                         std::remove_pointer_t<raw_elem_type>[],
                                         raw_elem_type>;
    return Element<elem_type>{ref.obj, i};
  }

  auto begin() const { return Iterator{(*this)[0]}; }

  auto end() const { return Iterator{(*this)[size()]}; }
};

template <typename Class, typename SuperClass>
auto begin(jObject<Class, SuperClass> &o) {
  return o.begin();
}

template <typename Class, typename SuperClass>
auto end(jObject<Class, SuperClass> &o) {
  return o.end();
}

template <typename C, typename SC>
std::ostream &operator<<(std::ostream &out, const jObject<C, SC> &o) {
  return out << std::string{o};
}

template <typename T> constexpr auto jSignature = make_signature<T>{}();
template <typename T>
constexpr auto jSignature<jObject<T>> = make_signature<T>{}();

class jMonitor {
  static JNIEnv *env() { return JavaVirtualMachine::env; }
  const std::reference_wrapper<const jReference> ref;

public:
  jMonitor(const jMonitor &) = delete;
  jMonitor(jMonitor &&) = delete;
  jMonitor &operator=(const jMonitor &) = delete;
  jMonitor &operator=(jMonitor &&) = delete;

  jMonitor(const jReference &r) : ref(r) { env()->MonitorEnter(ref.get().obj); }
  template <typename T> jMonitor(const jObject<T> &h) : jMonitor(h.ref) {}
  ~jMonitor() {
    if (!env())
      return;
    env()->MonitorExit(ref.get().obj);
  }
};

#include "jfwd_decl.hpp"

class String : public jObject<String> {
  template <typename, typename> friend class jObject;

  String(jobject o) { ref = jReference{o}; }

public:
  static constexpr auto signature = java_lang / "String";

  constexpr static Enum method_signatures{cexprstr{"\0"}};

  String() = default;
  String(const char *s)
      : jObject(static_cast<jobject>(env()->NewStringUTF(s))) {}
  template <size_t N>
  String(const char16_t (&s)[N])
      : jObject(static_cast<jobject>(env()->NewString(s, N - 1))) {}

  auto size() const {
    return env()->GetStringUTFLength(static_cast<jstring>(cast(*this)));
  }

  operator std::string() const {
    std::string s;
    s.resize(size());
    env()->GetStringUTFRegion(static_cast<jstring>(cast(*this)), 0, s.size(),
                              &s[0]);
    return s;
  }
};

#include "java/lang/*"

#endif
