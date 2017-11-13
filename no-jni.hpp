#ifndef NO_JNI_HPP
#define NO_JNI_HPP

#include "../fundamental-machines/constexpr_string.hpp"

#include <jni.h>

inline JavaVM *vm = nullptr;
inline JNIEnv *env = nullptr;

template <size_t N> struct jpackage : cexprstr<char, N> {
  constexpr jpackage(const char (&s)[N]) : cexprstr<char, N>{s} {}
  constexpr jpackage(const cexprstr<char, N> s) : cexprstr<char, N>{s} {}
};

template <size_t M, size_t N>
constexpr jpackage<M + N + 1> operator/(jpackage<M> l, jpackage<N> r) {
  return l + "/" + r;
}
template <size_t M, size_t N>
constexpr jpackage<M + N + 1> operator/(cexprstr<char, M> l, jpackage<N> r) {
  return l + "/" + r;
}
template <size_t M, size_t N>
constexpr jpackage<M + N + 1> operator/(jpackage<M> l, cexprstr<char, N> r) {
  return l + "/" + r;
}
template <size_t M, size_t N>
constexpr jpackage<M + N> operator/(const char (&l)[M], jpackage<N> r) {
  return l + "/" + r;
}
template <size_t M, size_t N>
constexpr jpackage<M + N> operator/(jpackage<M> l, const char (&r)[N]) {
  return l + "/" + r;
}

class jreference {
  jobjectRefType type = JNIInvalidRefType;
  jobject obj = nullptr;

public:
  jreference(jobject o) : type(env->GetObjectRefType(o)) {
    switch (type) {
    default:
      break;
    case JNILocalRefType:
      obj = env->NewLocalRef(o);
      break;
    case JNIGlobalRefType:
      obj = env->NewGlobalRef(o);
      break;
    case JNIWeakGlobalRefType:
      obj = env->NewWeakGlobalRef(o);
      break;
    case JNIInvalidRefType:
      break;
    }
  }
  ~jreference() {
    switch (type) {
    default:
      break;
    case JNILocalRefType:
      env->DeleteLocalRef(obj);
      break;
    case JNIGlobalRefType:
      env->DeleteGlobalRef(obj);
      break;
    case JNIWeakGlobalRefType:
      env->DeleteWeakGlobalRef(obj);
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
};

#endif