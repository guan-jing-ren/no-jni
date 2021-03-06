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
#include "org/eclipse/swt/widgets/*"

using org::eclipse::swt::SWT;
using namespace org::eclipse::swt::widgets;
using org::eclipse::swt::graphics::Point;

using namespace std;

[[maybe_unused]] constexpr jPackage com{"com"};

void basic_test(bool b) {
  if (!b)
    return;
  std::cout << "Display getCurrent: " << Display::getCurrent() << "\n";
  std::cout << "Display getDefault: " << Display::getDefault() << "\n";
  std::cout << "Display getDefault hashCode: "
            << Display::getDefault().super<java::lang::Object>().hashCode()
            << "\n";
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

  auto icon_sizes = Display::getDefault().getIconSizes();
  std::cout << "Icon sizes: " << icon_sizes << "\n";
  auto num_icon_sizes = icon_sizes.size();
  icon_sizes[0] = point;
  std::cout << "Icon sizes size: " << num_icon_sizes << "\n";
  for (Point icon_size : icon_sizes)
    std::cout << "Icon size: " << icon_size.x() << "," << icon_size.y() << "\n";

  std::vector<Point> icon_size_vector(begin(icon_sizes), end(icon_sizes));
  for (Point icon_size : icon_size_vector)
    std::cout << "Icon size: " << icon_size.x() << "," << icon_size.y() << "\n";

  Point pnew(84, point2.y());
  std::cout << "pnew: " << pnew.x() << "," << pnew.y() << "\n";

  jObject<Point *> snew{icon_sizes.size() + 1};
  std::copy(make_reverse_iterator(end(icon_sizes)),
            make_reverse_iterator(begin(icon_sizes)), begin(snew));
  snew[icon_sizes.size()] = pnew;
  for (Point icon_size : snew)
    std::cout << "snew Icon size: " << icon_size << "\n";
}

int main(int c, char **v) {
  JavaVirtualMachine jvm(c, v);

  basic_test(false);

  Shell shell{Display::getDefault()};
  shell.setText(String{"NoJNI shell test!"});
  shell.open();

  while (!shell.super<org::eclipse::swt::widgets::Widget>().isDisposed())
    if (!Display::getDefault().readAndDispatch())
      Display::getDefault().sleep();

  Display::getDefault().super<org::eclipse::swt::graphics::Device>().dispose();

  return 0;
}
