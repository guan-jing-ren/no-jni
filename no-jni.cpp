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

constexpr auto swt = org / "eclipse" / "swt";
constexpr auto swt_widgets = swt / "widgets";
constexpr auto swt_graphics = swt / "graphics";

class SWT : public jObject<SWT> {
public:
  static constexpr auto signature = swt / "SWT";
  constexpr static Enum field_signatures{
      jField<jint>("ALT"),        //
      jField<jint>("ARROW_UP"),   //
      jField<jint>("ARROW_DOWN"), //
      jField<jint>("ARROW_LEFT"), //
      jField<jint>("ARROW_RIGHT"),
  };
};

class Widget : public jObject<Widget> {
public:
  static constexpr auto signature = swt_widgets / "Widget";

  constexpr static Enum method_signatures{
      jMethod<jboolean()>("isDisposed"),
  };
};

class Point : public jObject<Point> {
public:
  static constexpr auto signature = swt_graphics / "Point";

  constexpr static Enum field_signatures{
      jField<jint>("x"),
      jField<jint>("y"),
  };

  constexpr static Enum method_signatures{
      jConstructor<int, int>(),
  };

  using jObject::jObject;
};

class PointArray : public jObject<Point[]> {
public:
};

class Shell : public jObject<Shell, Widget> {
public:
  static constexpr auto signature = swt_widgets / "Shell";

  constexpr static Enum method_signatures{
      jConstructor<Shell>(),
      jMethod<Point()>("getLocation"),
      jMethod<jvoid()>("open"),
      jMethod<jvoid(String)>("setText"),
  };
};
class ShellArray : public jObject<Shell[]> {};

class Display : public jObject<Display> {
public:
  static constexpr auto signature = swt_widgets / "Display";

  constexpr static Enum method_signatures{
      jMethod<jvoid()>("dispose"),
      jMethod<Display()>("getCurrent"),
      jMethod<Display()>("getDefault"),
      jMethod<Widget(jlong, jlong)>("findWidget"),
      jMethod<Point()>("getCursorLocation"),
      jMethod<ShellArray()>("getShells"),
      jMethod<PointArray()>("getIconSizes"),
      jMethod<jboolean()>("readAndDispatch"),
      jMethod<jboolean()>("sleep"),
  };
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

  std::cout << "Display getCurrent: " << Display::scall<Display>("getCurrent")
            << "\n";
  std::cout << "Display getDefault: " << Display::scall<Display>("getDefault")
            << "\n";
  std::cout << "Display getDefault hashCode: "
            << Display::scall<Display>("getDefault").call<jint>("hashCode")
            << "\n";
  std::cout << "Display getDefault getCursorLocation x,y: "
            << Display::scall<Display>("getDefault")
                   .call<Point>("getCursorLocation")
                   .at<jint>("x")
            << ","
            << Display::scall<Display>("getDefault")
                   .call<Point>("getCursorLocation")
                   .at<jint>("y")
            << "\n";

  auto point =
      Display::scall<Display>("getDefault").call<Point>("getCursorLocation");
  auto point2 = point;
  std::cout << "Point 2: " << point2.at<jint>("x") << ","
            << point2.at<jint>("y") << "\n";
  point2.at<jint>("x") = 42;
  point2.at<jint>("y") = 43;
  std::cout << "Point: " << point.at<jint>("x") << "," << point.at<jint>("y")
            << "\n";
  std::cout << "Point 2: " << point2.at<jint>("x") << ","
            << point2.at<jint>("y") << "\n";

  std::cout << "SWT ALT: " << SWT::sat<jint>("ALT") << "\n";
  std::cout << "SWT ARROW_UP: " << SWT::sat<jint>("ARROW_UP") << "\n";
  std::cout << "SWT ARROW_DOWN: " << SWT::sat<jint>("ARROW_DOWN") << "\n";
  std::cout << "SWT ARROW_LEFT: " << SWT::sat<jint>("ARROW_LEFT") << "\n";
  std::cout << "SWT ARROW_RIGHT: " << SWT::sat<jint>("ARROW_RIGHT") << "\n";

  Display display;
  std::cout << "Display findWidget: "
            << display.call<Widget>("findWidget", jlong{}, jlong{}) << "\n";
  std::cout << "Display getWidget: "
            << display.call<Widget>("getWidget", jlong{}, jlong{}) << "\n";

  std::cout << "Display getClass: " << Display::getClass() << "\n";

  auto icon_sizes =
      Display::scall<Display>("getDefault").call<PointArray>("getIconSizes");
  std::cout << "Icon sizes: " << icon_sizes << "\n";
  auto num_icon_sizes = icon_sizes.size();
  icon_sizes[0] = point;
  std::cout << "Icon sizes size: " << num_icon_sizes << "\n";
  for (Point icon_size : icon_sizes) {
    std::cout << "Icon size: " << icon_size.at<jint>("x") << ","
              << icon_size.at<jint>("y") << "\n";
  }

  std::vector<Point> icon_size_vector(begin(icon_sizes), end(icon_sizes));
  for (Point icon_size : icon_size_vector) {
    std::cout << "Icon size: " << icon_size.at<jint>("x") << ","
              << icon_size.at<jint>("y") << "\n";
  }

  Point pnew(84, point2.at<jint>("y"));
  std::cout << "pnew: " << pnew.at<jint>("x") << "," << pnew.at<jint>("y")
            << "\n";

  PointArray snew{icon_sizes.size() + 1};
  std::copy(make_reverse_iterator(end(icon_sizes)),
            make_reverse_iterator(begin(icon_sizes)), begin(snew));
  snew[icon_sizes.size()] = pnew;
  for (Point icon_size : snew) {
    std::cout << "snew Icon size: " << icon_size << "\n";
  }

  Shell shell{Display::scall<Display>("getDefault")};
  shell.call<jvoid>("setText", String{"NoJNI shell test!"});
  shell.call<jvoid>("open");

  while (!shell.call<jboolean>("isDisposed")) {
    auto shells =
        Display::scall<Display>("getDefault").call<ShellArray>("getShells");
    std::cout << "Shells: " << shells << "\n";
    auto num_shells = shells.size();
    std::cout << "Shells size: " << num_shells << "\n";
    for (auto i = 0; i < num_shells; ++i)
      std::cout << "Shell " << i << ": " << shells[i] << "\n";
    if (!Display::scall<Display>("getDefault")
             .call<jboolean>("readAndDispatch"))
      Display::scall<Display>("getDefault").call<jboolean>("sleep");
  }
  Display::scall<Display>("getDefault").call<jvoid>("dispose");

  return 0;
}
