// clang-format off
/*
clang++ -std=c++17 -Wall -I /usr/lib/jvm/default-java/include -I /usr/lib/jvm/default-java/include/linux/ -L /usr/lib/jvm/default-java/jre/lib/amd64/server/ -o nojni no-jni.cpp -ljvm
env LD_LIBRARY_PATH=/usr/lib/jvm/default-java/jre/lib/amd64/server/ ./nojni -verbose:jni -Drun
*/
// clang-format on

#include "no-jni.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main(int c, char **v) {
  constexpr auto swt = jpackage{"org"} / "eclipse" / "swt";
  cout << "SWT package name: " << swt << ";\n";

  vector<JavaVMOption> options;
  transform(v + 1, v + c, back_inserter(options), [](auto v) {
    JavaVMOption opt;
    opt.optionString = v;
    return opt;
  });
  JavaVMInitArgs args;
  args.version = JNI_VERSION_1_8;
  args.nOptions = c - 1;
  args.options = options.data();
  auto rc = JNI_CreateJavaVM(&vm, reinterpret_cast<void **>(&env), &args);
  cout << rc << '\n';
  cout << vm << ' ' << env << '\n';

  return 0;
}