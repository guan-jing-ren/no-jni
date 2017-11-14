// clang-format off
/*
clang++ -std=c++17 -Wall -I /usr/lib/jvm/default-java/include -I /usr/lib/jvm/default-java/include/linux/ -L /usr/lib/jvm/default-java/jre/lib/amd64/server/ -o nojni no-jvm.cpp no-jni.cpp -ljvm
env LD_LIBRARY_PATH=/usr/lib/jvm/default-java/jre/lib/amd64/server/ ./nojni -Drun=run
*/
// clang-format on

#include "no-jni.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

constexpr jpackage com{"com"};
constexpr jpackage org{"org"};
constexpr jpackage java{"java"};
constexpr jpackage javax{"javax"};

constexpr auto swt = org / "eclipse" / "swt";

class Display : public jhandle<Display> {
public:
  static constexpr jsignature_t signature = swt / "Display";
};

int main(int c, char **v) {
  cout << "SWT package name: " << swt << ";\n";
  cout << jsignature<jboolean> << ";\n";
  cout << jsignature<jbyte> << ";\n";
  cout << jsignature<jchar> << ";\n";
  cout << jsignature<jshort> << ";\n";
  cout << jsignature<jint> << ";\n";
  cout << jsignature<jlong> << ";\n";
  cout << jsignature<jfloat> << ";\n";
  cout << jsignature<jdouble> << ";\n";
  cout << jsignature<jvoid> << ";\n";
  cout << jsignature<Display> << "\n";
  cout << jsignature<jhandle<Display>> << "\n";

  JavaVirtualMachine jvm(c, v);
  cout << jvm.vm << ' ' << jvm.env << '\n';

  return 0;
}
