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

[[maybe_unused]] constexpr jpackage com{"com"};
[[maybe_unused]] constexpr jpackage org{"org"};
[[maybe_unused]] constexpr jpackage java{"java"};
[[maybe_unused]] constexpr jpackage javax{"javax"};

constexpr auto swt_widgets = org / "eclipse" / "swt";

class Widget : public jhandle<Widget> {
public:
  static constexpr jsignature_t signature = swt_widgets / "Widget";
};

class Display : public jhandle<Display> {
public:
  static constexpr jsignature_t signature = swt_widgets / "Display";

  constexpr const static Enum method_signatures{
      jfunction<Display, Display>("getCurrent"),
      jfunction<Widget, Display, jlong, jint>("findWidget")};
};

class DisplayArray : public jhandle<Display[]> {
public:
};

class IntArrayArray : public jhandle<jint *[]> {
public:
};

int main(int c, char **v) {
  cout << "SWT package name: " << swt_widgets << ";\n";
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
  cout << jsignature<DisplayArray> << "\n";
  cout << jsignature<IntArrayArray> << "\n";
  cout << Display::method_signatures << "\n";

  JavaVirtualMachine jvm(c, v);
  cout << jvm.vm << ' ' << jvm.env << '\n';

  Display display;
  display.call<Widget>("findWidget", jlong{}, jint{});
  display.call<Widget>("getWidget", jlong{}, jlong{});

  return 0;
}
