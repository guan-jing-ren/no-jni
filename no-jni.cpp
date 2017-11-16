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

[[maybe_unused]] constexpr jPackage com{"com"};
[[maybe_unused]] constexpr jPackage org{"org"};
[[maybe_unused]] constexpr jPackage java{"java"};
[[maybe_unused]] constexpr jPackage javax{"javax"};

constexpr auto swt_widgets = org / "eclipse" / "swt";

class Widget : public jObject<Widget> {
public:
  static constexpr jSignature_t signature = swt_widgets / "Widget";
};

class Display : public jObject<Display> {
public:
  static constexpr jSignature_t signature = swt_widgets / "Display";

  constexpr const static Enum method_signatures{
      jFunction<Display, Display>("getCurrent"),
      jFunction<Widget, Display, jlong, jint>("findWidget")};
};

class DisplayArray : public jObject<Display[]> {
public:
};

class IntArrayArray : public jObject<jint *[]> {
public:
};

int main(int c, char **v) {
  cout << "SWT package name: " << swt_widgets << ";\n";
  cout << jSignature<jboolean> << ";\n";
  cout << jSignature<jbyte> << ";\n";
  cout << jSignature<jchar> << ";\n";
  cout << jSignature<jshort> << ";\n";
  cout << jSignature<jint> << ";\n";
  cout << jSignature<jlong> << ";\n";
  cout << jSignature<jfloat> << ";\n";
  cout << jSignature<jdouble> << ";\n";
  cout << jSignature<jvoid> << ";\n";
  cout << jSignature<Display> << "\n";
  cout << jSignature<jObject<Display>> << "\n";
  cout << jSignature<DisplayArray> << "\n";
  cout << jSignature<IntArrayArray> << "\n";
  cout << Display::method_signatures << "\n";

  JavaVirtualMachine jvm(c, v);
  cout << jvm.vm << ' ' << jvm.env << '\n';

  Display display;
  display.call<Widget>("findWidget", jlong{}, jint{});
  display.call<Widget>("getWidget", jlong{}, jlong{});

  return 0;
}
