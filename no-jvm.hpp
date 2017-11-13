#ifndef NO_JVM_HPP
#define NO_JVM_HPP

#include <jni.h>

#include <algorithm>
#include <vector>

struct JavaVirtualMachine {
  static JavaVM *vm;
  static JNIEnv *env;

  JavaVirtualMachine(int c, char **v);
  ~JavaVirtualMachine();
};

jint JNI_OnLoad(JavaVM *, void *);
void JNI_OnUnLoad(JavaVM *, void *);

#endif