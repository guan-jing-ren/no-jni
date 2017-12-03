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

#include "org/eclipse/swt/SWT.jpp"
#include "org/eclipse/swt/graphics/Device.jpp"
#include "org/eclipse/swt/graphics/Point.jpp"
#include "org/eclipse/swt/widgets/Canvas.jpp"
#include "org/eclipse/swt/widgets/Composite.jpp"
#include "org/eclipse/swt/widgets/Control.jpp"
#include "org/eclipse/swt/widgets/Decorations.jpp"
#include "org/eclipse/swt/widgets/Display.jpp"
#include "org/eclipse/swt/widgets/Scrollable.jpp"
#include "org/eclipse/swt/widgets/Shell.jpp"
#include "org/eclipse/swt/widgets/Widget.jpp"

using org::eclipse::swt::SWT;
using namespace org::eclipse::swt::widgets;
using org::eclipse::swt::graphics::Point;

struct PointArray : public jObject<Point[]> {
  static constexpr auto signature = make_signature<Point[]>{}();
};

using namespace std;

[[maybe_unused]] constexpr jPackage com{"com"};

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

void basic_test(bool b) {
  std::cout << "Display getCurrent: " << Display::getCurrent() << "\n";
  std::cout << "Display getDefault: " << Display::getDefault() << "\n";
  std::cout << "Display getDefault hashCode: "
            << Display::getDefault().call<jint>("hashCode") << "\n";
  std::cout << "Display getDefault getCursorLocation x,y: "
            << Display::getDefault().getCursorLocation().x() << ","
            << Display::getDefault().getCursorLocation().y() << "\n";

  auto point = Display::getDefault().getCursorLocation();
  auto point2 = point;
  std::cout << "Point 2: " << point2.x() << "," << point2.y() << "\n";
  point2.x() = 42;
  point2.y() = 43;
  std::cout << "Point: " << point.x() << "," << point.y() << "\n";
  std::cout << "Point 2: " << point2.x() << "," << point2.y() << "\n";

  std::cout << "SWT ALT: " << SWT::ALT() << "\n";
  std::cout << "SWT ARROW_UP: " << SWT::ARROW_UP() << "\n";
  std::cout << "SWT ARROW_DOWN: " << SWT::ARROW_DOWN() << "\n";
  std::cout << "SWT ARROW_LEFT: " << SWT::ARROW_LEFT() << "\n";
  std::cout << "SWT ARROW_RIGHT: " << SWT::ARROW_RIGHT() << "\n";

  Display display;
  std::cout << "Display findWidget: " << display.findWidget(jlong{}, jlong{})
            << "\n";
  std::cout << "Display getWidget: "
            << display.call<Widget>("getWidget", jlong{}, jlong{}) << "\n";

  std::cout << "Display getClass: " << Display::getClass() << "\n";

  auto icon_sizes = Display::getDefault().call<PointArray>("getIconSizes");
  std::cout << "Icon sizes: " << icon_sizes << "\n";
  auto num_icon_sizes = icon_sizes.size();
  icon_sizes[0] = point;
  std::cout << "Icon sizes size: " << num_icon_sizes << "\n";
  for (Point icon_size : icon_sizes) {
    std::cout << "Icon size: " << icon_size.x() << "," << icon_size.y() << "\n";
  }

  std::vector<Point> icon_size_vector(begin(icon_sizes), end(icon_sizes));
  for (Point icon_size : icon_size_vector) {
    std::cout << "Icon size: " << icon_size.x() << "," << icon_size.y() << "\n";
  }

  Point pnew(84, point2.y());
  std::cout << "pnew: " << pnew.x() << "," << pnew.y() << "\n";

  PointArray snew{icon_sizes.size() + 1};
  std::copy(make_reverse_iterator(end(icon_sizes)),
            make_reverse_iterator(begin(icon_sizes)), begin(snew));
  snew[icon_sizes.size()] = pnew;
  for (Point icon_size : snew) {
    std::cout << "snew Icon size: " << icon_size << "\n";
  }
}

int main(int c, char **v) {
  JavaVirtualMachine jvm(c, v);

    auto shells =
        Display::scall<Display>("getDefault").call<ShellArray>("getShells");
    std::cout << "Shells: " << shells << "\n";
    auto num_shells = shells.size();
    std::cout << "Shells size: " << num_shells << "\n";
    for (auto i = 0; i < num_shells; ++i)
      std::cout << "Shell " << i << ": " << shells[i] << "\n";

  Shell shell{Display::getDefault()};
  shell.setText(String{"NoJNI shell test!"});
  shell.open();

  while (!shell.call<jboolean>("isDisposed"))
    if (!Display::getDefault().readAndDispatch())
      Display::getDefault().sleep();

  Display::getDefault().call<jvoid>("dispose");

  return 0;
}
