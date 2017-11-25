// clang-format off
/*
clang++ -fPIC -shared -g3 -O3 -std=c++17 -Wall -I /usr/lib/jvm/default-java/include -I /usr/lib/jvm/default-java/include/linux/ -L /usr/lib/jvm/default-java/jre/lib/amd64/server/ -o libnojvmti.so no-jvmti.cpp no-jvm.cpp -ljvm
env LD_LIBRARY_PATH=/usr/lib/jvm/default-java/jre/lib/amd64/server/ ./nojni -Drun=run
*/
// clang-format on

#include "no-jni.hpp"

#include <jvmti.h>

#include <algorithm>
#include <cstdio>
#include <iosfwd>
#include <vector>

using namespace std;

extern ostream cout;

jvmtiEnv *tenv;

void VMInit(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread);
void VMDeath(jvmtiEnv *jvmti_env, JNIEnv *jni_env);

jint Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
  JavaVirtualMachine::vm = vm;
  vm->GetEnv(reinterpret_cast<void **>(&tenv), JVMTI_VERSION_1_2);
  std::cout << "JVMTI options: " << string{options ? options : ""} << "\n";

  static jvmtiCapabilities cap;
  cap.can_tag_objects = true;
  cap.can_generate_field_modification_events = true;
  cap.can_generate_field_access_events = true;
  cap.can_get_bytecodes = true;
  cap.can_get_synthetic_attribute = true;
  cap.can_get_owned_monitor_info = true;
  cap.can_get_current_contended_monitor = true;
  cap.can_get_monitor_info = true;
  cap.can_pop_frame = true;
  cap.can_redefine_classes = true;
  cap.can_signal_thread = true;
  cap.can_get_source_file_name = true;
  cap.can_get_line_numbers = true;
  cap.can_get_source_debug_extension = true;
  cap.can_access_local_variables = true;
  cap.can_maintain_original_method_order = true;
  cap.can_generate_single_step_events = true;
  cap.can_generate_exception_events = true;
  cap.can_generate_frame_pop_events = true;
  cap.can_generate_breakpoint_events = true;
  cap.can_suspend = true;
  cap.can_redefine_any_class = true;
  cap.can_get_current_thread_cpu_time = true;
  cap.can_get_thread_cpu_time = true;
  cap.can_generate_method_entry_events = true;
  cap.can_generate_method_exit_events = true;
  cap.can_generate_all_class_hook_events = true;
  cap.can_generate_compiled_method_load_events = true;
  cap.can_generate_monitor_events = true;
  cap.can_generate_vm_object_alloc_events = true;
  cap.can_generate_native_method_bind_events = true;
  cap.can_generate_garbage_collection_events = true;
  cap.can_generate_object_free_events = true;
  cap.can_force_early_return = true;
  cap.can_get_owned_monitor_stack_depth_info = true;
  cap.can_get_constant_pool = true;
  cap.can_set_native_method_prefix = true;
  cap.can_retransform_classes = true;
  cap.can_retransform_any_class = true;
  cap.can_generate_resource_exhaustion_heap_events = true;
  cap.can_generate_resource_exhaustion_threads_events = true;
  tenv->AddCapabilities(&cap);

  // clang-format off
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_MIN_EVENT_TYPE_VAL, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_START, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_THREAD_END, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_LOAD, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_START, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_EXCEPTION, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_EXCEPTION_CATCH, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_SINGLE_STEP, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_FRAME_POP, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_BREAKPOINT, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_FIELD_ACCESS, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_FIELD_MODIFICATION, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_NATIVE_METHOD_BIND, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_COMPILED_METHOD_LOAD, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_COMPILED_METHOD_UNLOAD, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_DYNAMIC_CODE_GENERATED, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_DATA_DUMP_REQUEST, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_MONITOR_WAIT, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_MONITOR_WAITED, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_MONITOR_CONTENDED_ENTER, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_MONITOR_CONTENDED_ENTERED, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_RESOURCE_EXHAUSTED, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, nullptr);
  tenv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_OBJECT_FREE, nullptr);
  // clang-format on

  static jvmtiEventCallbacks callbacks;
  callbacks.VMInit = VMInit;
  callbacks.VMDeath = VMDeath;
  tenv->SetEventCallbacks(&callbacks, sizeof(callbacks));

  return JNI_OK;
}

template <typename J> struct tAlloc {
  static jvmtiEnv *env() { return tenv; }

  jint count = 0;
  J *j = nullptr;

  tAlloc() = default;

  tAlloc(tAlloc &&other) : count(other.count), j(other.j) {
    other.count = 0;
    other.j = nullptr;
  }

  ~tAlloc() {
    if (!env())
      return;
    if constexpr (std::is_pointer<J>::value)
      std::for_each(j, j + count, [](J &j) {
        if constexpr (std::is_convertible<J, jobject>::value)
          jReference::steal(reinterpret_cast<jobject &>(j));
        else if constexpr (!std::is_same<J, jmethodID>::value &&
                           !std::is_same<J, jfieldID>::value)
          env()->Deallocate(reinterpret_cast<unsigned char *>(j));
      });
    env()->Deallocate(reinterpret_cast<unsigned char *>(j));
  }

  operator jint *() { return &count; }
  operator J **() { return &j; }

  J *begin() { return j; }
  J *end() { return j + count; }
};

class tObject : public jObject<tObject> {
protected:
  static jvmtiEnv *env() { return tenv; }

public:
  static constexpr jSignature_t signature{"\0"};
  static constexpr Enum method_signatures{cexprstr{"\0"}};

  tObject(jobject o) : jObject(o) {}
};

class tClassLoader : public tObject {
public:
  using tObject::tObject;

  tClassLoader(jobject o) : tObject(o) {}

  tAlloc<jclass> classes() const {
    tAlloc<jclass> cls;
    env()->GetClassLoaderClasses(cast(*this), cls, cls);
    return cls;
  }
};

class tClass : public tObject {

  template <typename G, jvmtiError (jvmtiEnv::*getter)(jclass, jint *, G **)>
  tAlloc<G> get_members() const {
    tAlloc<G> members;
    (env()->*getter)(*this, members, members);
    return members;
  }

public:
  operator jclass() const { return static_cast<jclass>(cast(*this)); }

  using tObject::tObject;

  std::string signature() const {
    tAlloc<char> sig, gen;
    env()->GetClassSignature(*this, sig, gen);
    return sig.j;
  }

  tClassLoader classloader() const {
    jobject cl;
    env()->GetClassLoader(*this, &cl);
    return tClassLoader{jReference::steal(cl)};
  }

  auto get_methods() const {
    return get_members<jmethodID, &jvmtiEnv::GetClassMethods>();
  }

  auto get_fields() const {
    return get_members<jfieldID, &jvmtiEnv::GetClassFields>();
  }
};

template <typename M> class tMember {
protected:
  M id;

  std::string nm, sig, gen;

  void set_name(tAlloc<char> &n, tAlloc<char> &s, tAlloc<char> &g) {
    if (n.j)
      nm = n.j;
    if (s.j)
      sig = s.j;
    if (g.j)
      gen = g.j;
  }

public:
  operator M() const { return id; }

  const std::string &name() const { return nm; }
  const std::string &signature() const { return sig; }
  const std::string &generic() const { return gen; }
};

class tMethod : public tMember<jmethodID> {
  static jvmtiEnv *env() { return tenv; }

public:
  tMethod(jmethodID i) {
    id = i;
    tAlloc<char> n, s, g;
    env()->GetMethodName(id, n, s, g);
    set_name(n, s, g);
  }
};

class tField : public tMember<jfieldID> {
  static jvmtiEnv *env() { return tenv; }
  tClass clazz;

public:
  tField(tClass c, jfieldID i) : clazz(c) {
    id = i;
    tAlloc<char> n, s, g;
    env()->GetFieldName(clazz, id, n, s, g);
    set_name(n, s, g);
  }

  void watch_access(bool watch) {
    if (watch)
      env()->SetFieldAccessWatch(clazz, *this);
    else
      env()->ClearFieldAccessWatch(clazz, *this);
  }

  void watch_modify(bool watch) {
    if (watch)
      env()->SetFieldModificationWatch(clazz, *this);
    else
      env()->ClearFieldModificationWatch(clazz, *this);
  }
};

void VMInit(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread) {
  JavaVirtualMachine::env = jni_env;
  std::cout << "VM Initialized\n";

  tAlloc<jclass> all_loaded;
  tenv->GetLoadedClasses(all_loaded, all_loaded);
  std::vector<std::string> signatures;
  std::transform(begin(all_loaded), end(all_loaded), back_inserter(signatures),
                 [](const tClass &clazz) { return clazz.signature(); });
  std::sort(begin(signatures), end(signatures));
  for (std::string sig : signatures)
    std::cout << sig << "\n";

  for (tClass clazz : all_loaded) {
    std::cout << clazz.signature() << "\n";

    auto fields = clazz.get_fields();
    std::vector<std::string> fsignatures;
    std::transform(begin(fields), end(fields), back_inserter(fsignatures),
                   [clazz](jfieldID id) {
                     tField field{clazz, id};
                     return field.name() + " " + field.signature();
                   });
    std::sort(begin(fsignatures), end(fsignatures));
    for (auto &sig : fsignatures)
      std::cout << "\t" << sig << "\n";
    std::cout << "\n";

    auto methods = clazz.get_methods();
    std::vector<std::string> msignatures;
    std::transform(begin(methods), end(methods), back_inserter(msignatures),
                   [](const tMethod &method) {
                     return method.name() + " " + method.signature();
                   });
    std::sort(begin(msignatures), end(msignatures));
    for (auto &sig : msignatures)
      std::cout << "\t" << sig << "\n";
  }
}

void VMDeath(jvmtiEnv *jvmti_env, JNIEnv *jni_env) {
  JavaVirtualMachine::vm = nullptr;
  JavaVirtualMachine::env = nullptr;
  tenv = nullptr;
}
