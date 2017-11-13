// clang-format off
/*
clang++ -std=c++17 -Wall -I /usr/lib/jvm/default-java/include -I /usr/lib/jvm/default-java/include/linux/ -L /usr/lib/jvm/default-java/jre/lib/amd64/server/ -o nojni no-jvm.cpp no-jni.cpp -ljvm
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

  JavaVirtualMachine jvm(c, v);
  cout << jvm.vm << ' ' << jvm.env << '\n';

  return 0;
}