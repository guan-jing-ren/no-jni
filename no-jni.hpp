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

template <typename R, typename... Args> struct make_signature<R(Args...)> {
  static constexpr bool is_member_function = true;
  constexpr make_signature() {}
  constexpr auto operator()() const {
    if constexpr (sizeof...(Args) == 0)
      return "()" + make_signature<R>{}();
    else
      return "(" + concat(make_signature<Args>{}()...) + ")" +
             make_signature<R>{}();
  }
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

class Object;
template <typename Class, typename SuperClass = Object> class jClass {
  static JNIEnv *env() { return JavaVirtualMachine::env; }
  jReference ref;

  template <typename> friend struct make_signature;
  friend class jMonitor;
  template <typename, typename> friend class jObject;

public:
  using class_type = Class;
  using superclass_type = SuperClass;
  static constexpr auto signature = class_type::signature;

  jClass() {
    if (!env())
      return;
    constexpr auto sig = class_type::signature + "\0";
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

static void *cast(void *o) { return o; }
template <typename T>
static auto cast(T t) -> std::enable_if_t<std::is_arithmetic<T>::value, T> {
  return t;
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
  operator F() const { return (env()->*get)(obj, id); }
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

public:
  Element(Element &&) = delete;
  Element &operator=(const Element &) = delete;
  Element &operator=(Element &&) = delete;

  template <typename EE> E operator=(EE &&elem) {
    if constexpr (A)
      (env()->*set)(obj, idx, 1, &elem);
    else
      (env()->*set)(obj, idx, elem.ref.obj);
    return elem;
  }

  operator E() const {
    E elem;
    if constexpr (A)
      (env()->*get)(obj, idx, 1, &elem);
    else
      elem.ref = jReference{(env()->*get)(obj, idx)};
    return elem;
  }

  E operator*() const { return static_cast<E>(*this); }

  bool operator==(const Element &e) const {
    return obj == e.obj && idx == e.idx;
  }

  bool operator!=(const Element &e) const { return !(*this == e); }
};

template <typename E, bool A = std::is_arithmetic<E>::value> class Iterator {
  Element<E, A> element;

  Iterator(const Element<E, A> elem) : element(elem) {}

  template <typename, typename> friend class jObject;

public:
  Iterator(const Iterator &) = default;
  Iterator(Iterator &&) = delete;
  Iterator &operator=(const Iterator &) = delete;
  Iterator &operator=(Iterator &&) = delete;

  Element<E, A> operator*() const { return element; }
  Iterator &operator++() {
    ++element.idx;
    return *this;
  }

  bool operator==(const Iterator &e) const {
    return element.obj == e.element.obj && element.idx == e.element.idx;
  }

  bool operator!=(const Iterator &e) const { return !(*this == e); }
};

template <typename E, bool A> struct std::iterator_traits<Iterator<E, A>> {
  using iterator_category = std::forward_iterator_tag;
  using difference_type = ptrdiff_t;
  using value_type = E;
  using reference = E &;
};

template <typename Class, typename SuperClass = Object> class jObject {
  static JNIEnv *env() { return JavaVirtualMachine::env; }

  jReference ref;

  template <typename> friend struct make_signature;
  friend class jMonitor;
  template <typename, typename> friend class jObject;
  template <typename, bool> friend class Element;

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *), size_t N>
  static auto get_member(cexprstr<char, N> sig) {
    return (env()->*getter)(getClass(), sig.s,
                            std::find(sig.s, sig.s + sig.size(), 0) + 1);
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            size_t... I, typename E>
  static void init_members(std::index_sequence<I...>, G (&m)[sizeof...(I)],
                           E e) {
    [[maybe_unused]] auto ms = {
        (m[I] = get_member<G, getter>(e.template at<I>()))...};
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            typename E>
  static G find_member(size_t i, E e) {
    constexpr size_t N = E::size();
    static_assert(N);
    static G members[N] = {0};
    if (!members[i])
      init_members<G, getter>(std::make_index_sequence<N>{}, members,
                              e); // Must be initialized at runtime, after JNI
                                  // environment is established.
    return members[i];
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            typename F, size_t N, typename E, typename SE>
  static G get_member(const char (&s)[N], E e, SE se) {
    static_assert(E::size());
    auto m = find_member<G, getter>(member_index<G, F>(s, e), e);
    if constexpr (!std::is_same<class_type, superclass_type>::value)
      if (!m)
        m = superclass_type::template find_member<G, getter>(
            superclass_type::template member_index<G, F>(s, se), se);
    return m;
  }

  template <typename G,
            G (JNIEnv ::*getter)(jclass, const char *, const char *),
            typename R, size_t N, typename F, typename C, typename... Args>
  static R call_(const char (&s)[N], F f, C &&context, Args &&... args) {
    auto m = get_member<G, getter, R(std::decay_t<Args>...)>(
        s, class_type::method_signatures, superclass_type::method_signatures);
    if (!m)
      return {};
    return {(env()->*f)(context, m, cast(args)...)};
  }

  jObject(jobject o) : ref(o) {}

public:
  using class_type = Class;
  using superclass_type = SuperClass;
  jObject() = default;

  template <typename G, typename F, size_t N, typename E>
  constexpr static auto member_index(const char (&s)[N], E e) {
    return e[jMember<std::is_same<G, jmethodID>::value, F>(s)];
  }

  static const jClass<class_type> &getClass() {
    static jClass<class_type> clazz;
    if (!clazz.ref.obj)
      clazz = jClass<class_type>{};
    return clazz;
  }

  operator void *() const { return ref.obj; }

  template <typename F, size_t N>
  static Field<true, F> sat(const char (&s)[N]) {
    static_assert(!std::is_array<class_type>::value);
    auto f = get_member<jfieldID, &JNIEnv::GetStaticFieldID, F>(
        s, class_type::field_signatures, superclass_type::field_signatures);
    return Field<true, F>{getClass(), f};
  }

  template <typename F, size_t N> Field<false, F> at(const char (&s)[N]) const {
    static_assert(!std::is_array<class_type>::value);
    auto f = get_member<jfieldID, &JNIEnv::GetFieldID, F>(
        s, class_type::field_signatures, superclass_type::field_signatures);
    return Field<false, F>{ref.obj, f};
  }

  template <typename R, size_t N, typename... Args>
  static R scall(const char (&s)[N], Args &&... args) {
    static_assert(!std::is_array<class_type>::value);
    return call_<jmethodID, &JNIEnv::GetStaticMethodID, R>(
        s, static_caller<R>(), getClass(), std::forward<Args>(args)...);
  }

  template <typename R, size_t N, typename... Args>
  R call(const char (&s)[N], Args &&... args) const {
    static_assert(!std::is_array<class_type>::value);
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

#endif
