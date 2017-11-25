#ifndef NO_SWT_HPP
#define NO_SWT_HPP

#include "no-jni.hpp"

constexpr jPackage org{"org"};
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

#endif