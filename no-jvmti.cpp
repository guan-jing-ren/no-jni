// clang-format off
/*
clang++ -fPIC -shared -g3 -O3 -std=c++17 -DJAVA -ferror-limit=200 -Wall -I /usr/lib/jvm/default-java/include -I /usr/lib/jvm/default-java/include/linux/ -L /usr/lib/jvm/default-java/jre/lib/amd64/server/ -o libnojvmti.so no-jvmti.cpp no-jvm.cpp -ljvm
env LD_LIBRARY_PATH=/usr/lib/jvm/default-java/jre/lib/amd64/server/ ./nojni -Drun=run
*/
// clang-format on

#include "no-jni.hpp"

#include "java/lang/reflect/*-jfwd.jpp"
#include "java/lang/reflect/*"

#include "java/lang/annotation/Annotation.jpp"
#include "java/lang/Deprecated.jpp"

#include <jvmti.h>

#include <algorithm>
#include <cstdio>
#include <numeric>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

extern ostream std::cout;

jvmtiEnv *tenv;

void VMInit(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread);
void VMDeath(jvmtiEnv *jvmti_env, JNIEnv *jni_env);

string agent_options;

jint Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
  JavaVirtualMachine::vm = vm;
  vm->GetEnv(reinterpret_cast<void **>(&tenv), JVMTI_VERSION_1_2);
  agent_options = options ? options : "";

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
    if constexpr (is_pointer<J>::value)
      for_each(j, j + count, [](J &j) {
        if constexpr (is_convertible<J, jobject>::value)
          jReference::steal(reinterpret_cast<jobject &>(j));
        else if constexpr (!is_trivially_destructible<J>::value)
          j.~J();
        else if constexpr (!is_same<J, jmethodID>::value &&
                           !is_same<J, jfieldID>::value)
          env()->Deallocate(reinterpret_cast<unsigned char *>(j));
      });
    env()->Deallocate(reinterpret_cast<unsigned char *>(j));
  }

  operator jint *() { return &count; }
  operator J **() { return &j; }
  template <typename JJ> operator JJ **() {
    static_assert(is_convertible<J, JJ>::value && sizeof(J) == sizeof(JJ));
    return reinterpret_cast<JJ **>(&j);
  }

  auto begin() const { return j; }
  auto end() const { return j + count; }
};

class tObject : public jObject<tObject> {
protected:
  static jvmtiEnv *env() { return tenv; }

public:
  static constexpr jSignature_t signature{"\0"};
  static constexpr Enum method_signatures{cexprstr{"\0"}};

  tObject() = default;
  tObject(jobject o) : jObject(o) {}

  static auto instance() {
    jobject o;
    env()->GetLocalInstance(nullptr, 0, &o);
    return tObject{jReference::steal(o)};
  }
};

class tClassLoader : public tObject {
public:
  using tObject::tObject;

  tClassLoader(jobject o) : tObject(o) {}

  auto classes() const {
    tAlloc<jclass> cls;
    env()->GetClassLoaderClasses(cast(*this), cls, cls);
    return cls;
  }
};

class tClass : public tObject {

  template <typename G, jvmtiError (jvmtiEnv::*getter)(jclass, jint *, G **)>
  auto get_members() const {
    tAlloc<G> members;
    (env()->*getter)(*this, members, members);
    return members;
  }

  jint modifiers;

public:
  operator jclass() const { return static_cast<jclass>(cast(*this)); }

  using tObject::tObject;

  string signature() const {
    if (!cast(*this))
      return "";
    tAlloc<char> sig, gen;
    env()->GetClassSignature(*this, sig, gen);
    return sig.j ? sig.j : "";
  }

  tClassLoader classloader() const {
    jobject cl;
    env()->GetClassLoader(*this, &cl);
    return tClassLoader{jReference::steal(cl)};
  }

  bool is_public() const {
    jint modifiers;
    env()->GetClassModifiers(*this, &modifiers);
    return modifiers & 0x1;
  }

  auto get_methods() const {
    return get_members<jmethodID, &jvmtiEnv::GetClassMethods>();
  }

  auto get_fields() const {
    return get_members<jfieldID, &jvmtiEnv::GetClassFields>();
  }

  auto implemented_interfaces() const {
    tAlloc<jclass> interfaces;
    env()->GetImplementedInterfaces(*this, interfaces, interfaces);
    return interfaces;
  }

  auto superclass() const {
    if (!cast(*this))
      return tClass{};
    auto scls = JavaVirtualMachine::env->GetSuperclass(*this);
    if (!scls)
      return tClass{};
    return tClass{jReference::steal(scls)};
  }
};

template <typename M> class tMember {
protected:
  M id;

  jint modifiers;
  string nm, sig, gen;

  auto set_name(tAlloc<char> &n, tAlloc<char> &s, tAlloc<char> &g) {
    if (n.j)
      nm = n.j;
    if (s.j)
      sig = s.j;
    if (g.j)
      gen = g.j;
  }

public:
  operator M() const { return id; }

  const string &name() const { return nm; }
  const string &signature() const { return sig; }
  const string &generic() const { return gen; }
};

template <typename T>
T (jvmtiEnv::*get_var)
(jthread, jint, jint, T) = &jvmtiEnv::GetLocalObject;
template <> auto get_var<jint> = &jvmtiEnv::GetLocalInt;
template <> auto get_var<jlong> = &jvmtiEnv::GetLocalLong;
template <> auto get_var<jfloat> = &jvmtiEnv::GetLocalFloat;
template <> auto get_var<jdouble> = &jvmtiEnv::GetLocalDouble;

template <typename T>
T (jvmtiEnv::*set_var)
(jthread, jint, jint, T) = &jvmtiEnv::SetLocalObject;
template <> auto set_var<jint> = &jvmtiEnv::SetLocalInt;
template <> auto set_var<jlong> = &jvmtiEnv::SetLocalLong;
template <> auto set_var<jfloat> = &jvmtiEnv::SetLocalFloat;
template <> auto set_var<jdouble> = &jvmtiEnv::SetLocalDouble;

class tMethod : public tMember<jmethodID> {
  static jvmtiEnv *env() { return tenv; }

public:
  tMethod(jmethodID i) {
    id = i;
    tAlloc<char> n, s, g;
    env()->GetMethodName(id, n, s, g);
    set_name(n, s, g);
    env()->GetMethodModifiers(id, &modifiers);
  }

  bool is_static() const { return modifiers & 0x8; }
  bool is_public() const { return modifiers & 0x1; }

  auto declaring_class() const {
    jclass cls;
    env()->GetMethodDeclaringClass(id, &cls);
    return tClass{jReference::steal(cls)};
  }

  auto breakpoint(bool brk, jlocation loc) const {
    if (brk)
      env()->SetBreakpoint(*this, loc);
    else
      env()->ClearBreakpoint(*this, loc);
  }

  struct tVariable {
    jlocation first, last;
    string name, signature, generic;
    jint slot;

    template <typename T> operator T() const {
      T t;
      (env()->*get_var<T>)(nullptr, 0, slot, &t);
      return t;
    }

    template <typename T> tVariable &operator=(T &&t) {
      (env()->*set_var<T>)(nullptr, 0, slot, t);
      return *this;
    }
  };

  auto local_variables() const {
    struct Entry : jvmtiLocalVariableEntry {
      Entry(const Entry &) = delete;
      ~Entry() {
        tAlloc<char> n, s, g;
        n.j = name;
        s.j = signature;
        g.j = generic_signature;
      }
    };
    static_assert(sizeof(Entry) == sizeof(jvmtiLocalVariableEntry));
    tAlloc<Entry> entries;

    env()->GetLocalVariableTable(id, entries, entries);

    vector<tVariable> vars;
    transform(begin(entries), end(entries), back_inserter(vars),
              [](const Entry &entry) {
                tVariable var;
                var.first = entry.start_location;
                var.last = var.first + entry.length;
                if (entry.name)
                  var.name = entry.name;
                if (entry.signature)
                  var.signature = entry.signature;
                if (entry.generic_signature)
                  var.generic = entry.generic_signature;
                var.slot = entry.slot;
                return var;
              });
    return vars;
  }

  java::lang::reflect::Method reflected() const {
    jclass declare;
    env()->GetMethodDeclaringClass(id, &declare);
    jobject reflect = JavaVirtualMachine::env->ToReflectedMethod(
        tClass{jReference::steal(declare)}, id, is_static());
    return jReference::steal(reflect);
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
    env()->GetFieldModifiers(clazz, id, &modifiers);
  }

  bool is_static() const { return modifiers & 0x8; }
  bool is_public() const { return modifiers & 0x1; }

  auto declaring_class() const {
    jclass cls;
    env()->GetFieldDeclaringClass(clazz, id, &cls);
    return tClass{jReference::steal(cls)};
  }

  auto watch_access(bool watch) const {
    if (watch)
      env()->SetFieldAccessWatch(clazz, *this);
    else
      env()->ClearFieldAccessWatch(clazz, *this);
  }

  auto watch_modify(bool watch) const {
    if (watch)
      env()->SetFieldModificationWatch(clazz, *this);
    else
      env()->ClearFieldModificationWatch(clazz, *this);
  }

  java::lang::reflect::Field reflected() const {
    jobject reflect =
        JavaVirtualMachine::env->ToReflectedField(clazz, id, is_static());
    return jReference::steal(reflect);
  }
};

class tThread : public tObject {
public:
  using tObject::tObject;
  operator jthread() const { return cast(*this); }

  static auto current() {
    jthread thread;
    env()->GetCurrentThread(&thread);
    return tThread{jReference::steal(thread)};
  }

  static auto all() {
    tAlloc<jthread> threads;
    env()->GetAllThreads(threads, threads);
    vector<tThread> all_threads;
    copy(threads.j, threads.j + threads.count, back_inserter(all_threads));
    return all_threads;
  }

  auto interrupt() const { return env()->InterruptThread(*this); }
  auto suspend() const { return env()->SuspendThread(*this); }
  auto resume() const { return env()->ResumeThread(*this); }

  auto frame_count() const {
    jint count;
    env()->GetFrameCount(*this, &count);
    return count;
  }

  auto stack_frames() const {
    auto count = frame_count();
    jint actual;
    vector<jvmtiFrameInfo> frames(count);
    env()->GetStackTrace(*this, 0, count, frames.data(), &actual);
    frames.resize(actual);
    return frames;
  }

  static auto all_stack_frames() {
    tAlloc<jvmtiStackInfo> info;
    auto threads = all();
    env()->GetAllStackTraces(
        accumulate(std::begin(threads), std::end(threads), 0,
                   [](jint max, const tThread &thread) {
                     return std::max(max, thread.frame_count());
                   }),
        info, info);

    struct StackInfo {
      tThread thread;
      jint state;
      vector<jvmtiFrameInfo> frame_buffer;
    };
    vector<StackInfo> stack_info;
    transform(info.j, info.j + info.count, back_inserter(stack_info),
              [](jvmtiStackInfo &si) {
                StackInfo info;
                info.thread = tThread{jReference::steal(si.thread)};
                info.state = si.state;
                info.frame_buffer.resize(si.frame_count);
                copy(si.frame_buffer, si.frame_buffer + si.frame_count,
                     back_inserter(info.frame_buffer));
                return info;
              });

    return stack_info;
  }

  auto pop_frame() const { env()->PopFrame(*this); }
};

string demangle(string sig) {
  regex token_rx{"(\\[*?)([ZBCSIJFDV()]|L.+?;)"};
  sig = regex_replace(sig, regex{"namespace"}, "namespace_");
  return accumulate(sregex_iterator{begin(sig), end(sig), token_rx}, {},
                    string{}, [](string sig, const smatch &sub) {
                      auto token = sub.str();
                      auto sep = (sig.empty() || sig.back() == '(' ? "" : ", ");
                      switch (token[0]) {
                      case 'Z':
                        return sig + sep + "::jboolean";
                      case 'B':
                        return sig + sep + "::jbyte";
                      case 'C':
                        return sig + sep + "::jchar";
                      case 'S':
                        return sig + sep + "::jshort";
                      case 'I':
                        return sig + sep + "::jint";
                      case 'J':
                        return sig + sep + "::jlong";
                      case 'F':
                        return sig + sep + "::jfloat";
                      case 'D':
                        return sig + sep + "::jdouble";
                      case 'V':
                        return sig + sep + "::jvoid";
                      case '(':
                      case ')':
                        return sig + token[0];
                      case '[':
                        return sig + sep + demangle(sub[2].str()) + " " +
                               string(sub[1].length(), '*');
                      case 'L':
                        rotate(begin(token), ++begin(token), end(token));
                        token.pop_back();
                        token.pop_back();
                        return sig + sep +
                               "::" + regex_replace(token, regex{"/"}, "::");
                      };
                      return string{};
                    });
}

#include "java/util/*-jfwd.jpp"
#include "java/util/zip/*-jfwd.jpp"

#include "java/util/*"
#include "java/util/zip/*"

#include "java/lang/Object.jpp"
#include "java/util/Enumeration.jpp"
#include "java/util/zip/ZipEntry.jpp"
#include "java/util/zip/ZipFile.jpp"
using java::util::Enumeration;
using java::util::zip::ZipEntry;
using java::util::zip::ZipFile;

void VMInit(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread) {
  JavaVirtualMachine::env = jni_env;

  unordered_map<string, tClass> classes;
  vector<string> csignatures;

  regex classpath_rx{"-Djava.class.path=(.+)"};
  regex option_rx{","};
  auto classpath_arg =
      find_if(sregex_token_iterator{begin(agent_options), end(agent_options),
                                    option_rx, -1},
              {}, [classpath_rx](const ssub_match &sub) {
                smatch classpath;
                auto arg = sub.str();
                return regex_match(arg, classpath, classpath_rx);
              });

  vector<string> packages;
  if (classpath_arg != sregex_token_iterator{}) {
    regex path_rx{":"};
    auto paths = classpath_arg->str();
    paths.erase(0, paths.find('=') + 1);
    unordered_set<string> pkgs;
    for_each(sregex_token_iterator{begin(paths), end(paths), path_rx, -1}, {},
             [&csignatures, &classes, &pkgs](const ssub_match &sub) {
               ZipFile zip{::String{sub.str().c_str()}};
               auto entries = zip.call<Enumeration>("entries");

               regex sig_rx{R"((.+)/(.+?)\.class)"};
               smatch signature;
               while (entries.call<jboolean>("hasMoreElements")) {
                 java::lang::Object entry{
                     entries.call<java::lang::Object>("nextElement")};
                 string name = entry;
                 regex_match(name, signature, sig_rx);
                 auto package = signature[1].str();
                 auto clazz = signature[2].str();
                 if (clazz.empty() || clazz == "package-info" ||
                     package.find("/internal") != string::npos)
                   continue;

                 for (auto p = 0; p != string::npos; p = package.find('/', ++p))
                   pkgs.insert(package.substr(0, p));
                 pkgs.insert(package);

                 auto sig = package + "/" + clazz;
                 csignatures.push_back(sig);
                 tClass tc{JavaVirtualMachine::env->FindClass(sig.c_str())};
                 if (tc.is_public())
                   classes[sig] = tc;
               }
             });
    pkgs.erase("");
    packages = {begin(pkgs), end(pkgs)};
    sort(begin(packages), end(packages));
    for (auto &pkg : packages)
      pkg = regex_replace(pkg, regex{"namespace"}, "namespace_");
  }

  ofstream dout{"jfwd_decl.hpp"};
  dout << "#ifndef JFWD_DECL_HPP\n"
       << "#define JFWD_DECL_HPP\n\n";

  unordered_map<string, pair<string, string>> pkg_to_nspace_pkg_var;

  auto prefixed = [](const auto &l, const auto &r) {
    auto pref_size = min(l.size(), r.size());
    return lexicographical_compare(begin(l), begin(l) + pref_size, begin(r),
                                   begin(r) + pref_size);
  };
  auto java =
      equal_range(begin(packages), end(packages), string{"java"}, prefixed);
  auto non_java = rotate(begin(packages), java.first, java.second);
  auto java_lang =
      equal_range(begin(packages), non_java, string{"java/lang"}, prefixed);
  rotate(++begin(packages), java_lang.first, java_lang.second);

  for (const auto &pkg : packages) {
    system(("rm " + pkg + "/*").c_str());
    system(("rm " + pkg + "/*-jfwd.jpp").c_str());
    auto nspace = regex_replace(pkg, regex{"/"}, "::");
    auto pkg_var = regex_replace(pkg, regex{"/"}, "_");
    pkg_to_nspace_pkg_var[pkg] = {nspace, pkg_var};
    auto pkg_parent = pkg_var.substr(
        0, pkg_var.back() == '_' ? pkg_var.rfind('_', pkg_var.rfind('_') - 1)
                                 : pkg_var.rfind('_'));
    auto pkg_child =
        pkg_var.substr(pkg_parent.size() + (pkg_parent != pkg_var));
    auto pkg_sig =
        "[[maybe_unused]] inline constexpr jPackage " + pkg_var +
        &"_"[!pkg_child.empty()] +
        (pkg_child.empty() ? "{\"" + pkg_parent + "\"};"
                           : (" = " + pkg_parent +
                              &"_ / \""[pkg_parent.find('_') != string::npos] +
                              pkg_child + "\";"));
    dout << "namespace " << nspace << " {}\n" << pkg_sig << "\n";
  }
  dout << "\n\n"
       << "#endif\n";

  sort(begin(csignatures), end(csignatures));
  for (auto &csig : csignatures)
    csig = regex_replace(csig, regex{"namespace"}, "namespace_");
  java = equal_range(begin(csignatures), end(csignatures), string{"java"},
                     prefixed);
  non_java = rotate(begin(csignatures), java.first, java.second);
  java_lang =
      equal_range(begin(csignatures), non_java, string{"java/lang"}, prefixed);
  auto non_java_lang =
      rotate(begin(csignatures), java_lang.first, java_lang.second);
  stable_partition(
      begin(csignatures), non_java_lang,
      [fund = regex{"java/lang/Class|java/lang/Object|java/lang/String"}](
          const auto &sig) { return regex_match(sig, fund); });

  for (const auto &pkg : packages) {
    std::ofstream dout{pkg + "/*-jfwd.jpp", std::ofstream::trunc};
    auto def = std::regex_replace(pkg, std::regex{"/"}, "_");
    dout << "#ifndef " << def << "_JPP\n";
    dout << "#define " << def << "_JPP\n\n";
  }
  for (const auto &sig : csignatures) {
    auto pkg = sig.substr(0, sig.rfind("/"));
    auto cls = sig.substr(pkg.size() + (pkg != sig));

    std::ofstream dout{pkg + "/*-jfwd.jpp", std::ofstream::app};

    dout << "namespace " << regex_replace(pkg, regex{"/"}, "::") << " { class "
         << cls << "; }\n";
    dout << "template<> constexpr auto signature<::"
         << regex_replace(sig, regex{"/"}, "::")
         << "> = ::" << regex_replace(pkg, regex{"/"}, "_") << " / \"" << cls
         << "\";\n";
  for (const auto &pkg : packages) {
    std::ofstream dout{pkg + "/*-jfwd.jpp", std::ofstream::app};
    dout << "\n"
         << "#endif\n";
  }

  java::lang::Class depr{jReference{java::lang::Deprecated::getClass()}};
  for (const auto &sig : csignatures) {
    auto pkg = sig.substr(0, sig.rfind("/"));

    system(("mkdir -p " + pkg).c_str());
    system(("touch " + pkg + "/*").c_str());
    system(("touch " + sig + ".jpp").c_str());
    ofstream fout{sig + ".jpp"};

    fout << "#ifndef " << regex_replace(sig, regex{"/"}, "_") << "_HPP\n"
         << "#define " << regex_replace(sig, regex{"/"}, "_") << "_HPP\n\n"
         << "#include \"" << regex_replace(pkg + "/", regex{".*?/"}, "../")
         << "jfwd_decl.hpp\"\n\n";

    auto cls = sig.substr(pkg.size() + (pkg != sig));
    ofstream aout{pkg + "/*", ofstream::app};
    aout << "#include \"" << cls << ".jpp\"\n";

    auto clazz = classes[sig];
    auto ssig = clazz.superclass().signature();
    if (ssig.find("/internal") != string::npos)
      ssig.clear();
    auto scls = regex_replace(demangle(ssig), regex{"/"}, "::");
    if (scls.empty())
      scls = "::java::lang::Object";

    fout << "class ::" << pkg_to_nspace_pkg_var[pkg].first << "::" << cls
         << " : public jObject<::" << pkg_to_nspace_pkg_var[pkg].first
         << "::" << cls << ", " << scls << "> {\n"
         << "public:\n"
         << "\tusing jObject::jObject;\n\n"
         << "\tstatic constexpr auto signature = ::"
         << pkg_to_nspace_pkg_var[pkg].second << " / \"" << cls << "\";\n\n";

    auto process_members = [depr](const auto &members, auto &signatures,
                                  auto &&factory) {
      transform(begin(members), end(members), back_inserter(signatures),
                [factory, depr](const auto &m) mutable {
                  auto member = factory(m);
                  auto deprecated = member.reflected().getAnnotation(depr);

                  return tuple{member.name(), member.signature(),
                               member.is_static(),
                               member.is_public() && !deprecated};
                });
    };

    auto fields = clazz.get_fields();
    vector<tuple<string, string, bool, bool>> fsignatures;
    auto field_from_id = [clazz](jfieldID id) { return tField{clazz, id}; };
    process_members(fields, fsignatures, field_from_id);
    for (tClass iface : clazz.implemented_interfaces())
      process_members(iface.get_fields(), fsignatures, field_from_id);
    fsignatures.erase(
        remove_if(begin(fsignatures), end(fsignatures),
                  [](const auto &sig) {
                    return !get<3>(sig) ||
                           get<1>(sig).find("/internal") != string::npos ||
                           get<1>(sig).find("java/awt/peer/") != string::npos;
                  }),
        end(fsignatures));
    sort(begin(fsignatures), end(fsignatures));
    fsignatures.erase(unique(begin(fsignatures), end(fsignatures)),
                      end(fsignatures));
    fout << "\tconstexpr static Enume field_signatures{\n";
    if (fsignatures.empty())
      fout << "\t\tcexprstr{\"\\0\"}, //\n";
    else
      for (const auto &sig : fsignatures)
        fout << "\t\tjField<" << demangle(get<1>(sig)) << ">(\"" << get<0>(sig)
             << "\"), //\n";

    fout << "\t};\n\n";

    regex keywords{
        "\\b(virtual|and|not|xor|or|export|namespace|signed|delete|union|"
        "register|signature|null|NULL|BIG_ENDIAN|LITTLE_ENDIAN|"
        "OVERFLOW|EACCESS|EAGAIN|ASCII|UNDERFLOW)\\b"};
    for (const auto &sig : fsignatures) {
      auto name = regex_replace(get<0>(sig), keywords, "$1_");
      fout << "\ttemplate<typename F = " << demangle(get<1>(sig)) << ">\n"
           << "\t" << &"\0static "[get<2>(sig)] << "auto " << name << "() "
           << &"\0const "[!get<2>(sig)] << "{\n"
           << "\t\tstatic_assert(field_signatures[jField<F>(\"" << get<0>(sig)
           << "\")] != -1);\n"
           << "\t\treturn " << &"\0s"[get<2>(sig)] << "at<F>(\"" << get<0>(sig)
           << "\");\n"
           << "\t}\n\n";
    }

    auto methods = clazz.get_methods();
    vector<tuple<string, string, bool, bool>> msignatures;
    auto method_from_id = [](jmethodID id) { return tMethod{id}; };

    process_members(methods, msignatures, method_from_id);
    for (tClass iface : clazz.implemented_interfaces())
      process_members(iface.get_methods(), msignatures, method_from_id);
    msignatures.erase(
        remove_if(begin(msignatures), end(msignatures),
                  [](const auto &sig) {
                    return !get<3>(sig) ||
                           get<1>(sig).find("/internal") != string::npos ||
                           get<1>(sig).find("java/awt/peer/") != string::npos;
                  }),
        end(msignatures));
    sort(begin(msignatures), end(msignatures));
    msignatures.erase(unique(begin(msignatures), end(msignatures)),
                      end(msignatures));
    fout << "\tconstexpr static Enume method_signatures{\n";
    if (msignatures.empty())
      fout << "\t\tcexprstr{\"\\0\"}, //\n";
    else
      for (auto &sig : msignatures) {
        rotate(begin(get<1>(sig)),
               ++find(begin(get<1>(sig)), end(get<1>(sig)), ')'),
               end(get<1>(sig)));
        if (get<0>(sig) == "<init>")
          fout << "\t\tjConstructor<" + demangle(get<1>(sig)) + ">(), //\n";
        else
          fout << "\t\tjMethod<" + demangle(get<1>(sig)) + ">(\"" +
                      get<0>(sig) + "\"), //\n";
      }
    fout << "\t};\n\n";

    unordered_set<string> overloaded;

    for (const auto &sig : msignatures) {
      if (get<0>(sig).find("init>") != string::npos)
        continue;
      auto name = regex_replace(get<0>(sig), keywords, "$1_");
      if (overloaded.find(get<0>(sig)) != overloaded.cend())
        continue;
      overloaded.insert(get<0>(sig));
      auto return_type = demangle(get<1>(sig).substr(0, get<1>(sig).find('(')));
      fout << "\ttemplate<typename R = " << return_type
           << ", typename... Args>\n"
           << "\t" << &"\0static "[get<2>(sig)] << "auto " << name
           << "(Args &&...args) " << &"\0const "[!get<2>(sig)] << "{\n"
           << "\t\tstatic_assert(method_signatures[jMethod<R(std::decay_t<"
              "Args>...)>(\""
           << get<0>(sig) << "\")] != -1);\n"
           << "\t\treturn " << &"\0s"[get<2>(sig)] << "call<R>(\""
           << get<0>(sig) << "\", std::forward<Args>(args)...);\n"
           << "\t}\n\n";
    }

    fout << "};\n\n"
         << "#endif\n";
  }

  JavaVirtualMachine::env->ExceptionClear();
}

void VMDeath(jvmtiEnv *jvmti_env, JNIEnv *jni_env) {
  JavaVirtualMachine::vm = nullptr;
  JavaVirtualMachine::env = nullptr;
  tenv = nullptr;
}
