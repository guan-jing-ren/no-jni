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

constexpr auto java_lang = java / "lang";

class Object : public jObject<Object> {
public:
  static constexpr auto signature = java_lang / "Object";

  constexpr static Enum method_signatures{
      method<Object()>("clone"), //
      method<jboolean(Object)>("equals"),
      method<jvoid()>("finalize"),
      method<jClass<Object>()>("getClass"),
      method<int()>("hashCode"),
      method<jvoid()>("notify"),
      method<jvoid()>("notifyAll"),
      method<jvoid()>("wait"),
      method<jvoid(jlong)>("wait"),
      method<jvoid(jlong, jint)>("wait"),
  };
};

constexpr auto swt_widgets = org / "eclipse" / "swt" / "widgets";

class Widget : public jObject<Widget> {
public:
  static constexpr auto signature = swt_widgets / "Widget";
};

class Display : public jObject<Display> {
public:
  static constexpr auto signature = swt_widgets / "Display";

  constexpr static Enum method_signatures{
      method<Display()>("getCurrent"),
      method<Widget(jlong, jlong)>("findWidget")};
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
  cout << jvm.vm << ' ' << jvm.env << "\n";

  Display display;
  std::cout << "Display findWidget: "
            << display.call<Widget>("findWidget", jlong{}, jlong{}) << "\n";
  std::cout << "Display getWidget: "
            << display.call<Widget>("getWidget", jlong{}, jlong{}) << "\n";

  std::cout << "Display getClass: " << Display::getClass() << "\n";

  std::cout << "Method index findWidget fail: "
            << std::integral_constant<
                   size_t, Display::method_index<Display(jlong, jlong)>(
                               "findWidget")>::value
            << '\n';
  std::cout << "Method index findWidget success: "
            << std::integral_constant<
                   size_t, Display::method_index<Widget(jlong, jlong)>(
                               "findWidget")>::value
            << '\n';

  return 0;
}
