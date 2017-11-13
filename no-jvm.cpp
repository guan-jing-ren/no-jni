#include "no-jvm.hpp"

JavaVM *JavaVirtualMachine::vm = nullptr;
JNIEnv *JavaVirtualMachine::env = nullptr;

JavaVirtualMachine::JavaVirtualMachine(int c, char **v) {
  std::vector<JavaVMOption> options;
  std::transform(v + 1, v + c, std::back_inserter(options), [](auto v) {
    JavaVMOption opt;
    opt.optionString = v;
    return opt;
  });
  JavaVMInitArgs args;
  args.version = JNI_VERSION_1_8;
  args.nOptions = c - 1;
  args.options = options.data();
  JNI_CreateJavaVM(&vm, reinterpret_cast<void **>(&env), &args);
}

JavaVirtualMachine::~JavaVirtualMachine() {
  if (vm)
    vm->DestroyJavaVM();
  vm = nullptr;
  env = nullptr;
}

jint JNI_OnLoad(JavaVM *vm, void *) {
  JavaVirtualMachine::vm = vm;
  vm->GetEnv(reinterpret_cast<void **>(&JavaVirtualMachine::env),
             JNI_VERSION_1_8);
  return JNI_VERSION_1_8;
}
void JNI_OnUnLoad(JavaVM *, void *) {
  JavaVirtualMachine::vm = nullptr;
  JavaVirtualMachine::env = nullptr;
}