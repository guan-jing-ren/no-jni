/*
 time g++ -O3 -std=c++14 -Wall -Wextra -Werror -pedantic-errors -o genwrap  wrapper_generator.cpp
 time clang++ -O3 -std=c++14 -Wall -Wextra -Werror -pedantic-errors -o genwrap  wrapper_generator.cpp
 */

#include <iomanip>
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <cstdlib>
#include <cassert>

struct Usage
{
  bool enable = true;

  ~Usage()
  {
    if(!enable) return;
    std::clog << "Usage: genwrap -h or --help for this message\n"
                 "       genwrap [jar file [exclude class regex]]\n"
                 "                jar file default $JAVA_HOME/jre/lib/rt.jar\n"
                 "Environment variables: JAVA_HOME, TMPDIR (default /tmp)\n";
  }
};

auto remove_class = std::regex{R"(\.class)", std::regex::optimize};
auto to_macro = std::regex{R"(/|\$)", std::regex::optimize};
auto to_qual_name = std::regex{R"(\$)", std::regex::optimize};
auto& to_signature = to_macro;
auto to_qualifier = std::regex{R"((\w+)/)", std::regex::optimize};
auto& to_inner = to_qual_name;
auto to_namespace = std::regex{R"((\w+)::)", std::regex::optimize};
auto escape_namespace = std::regex{R"(namespace (namespace|asm) \{)", std::regex::optimize};
auto line_rx = std::regex{R"( +([[:graph:]]+?) +([[:graph:]]+?)\((.*?)\);| +([[:graph:]]+?)\((.*?)\);| +([[:graph:]]+?) +\w+?(.*?=.*?)?;)", std::regex::optimize};
auto arg_rx = std::regex{R"(jhandle.+?(?=,|\))|\w+(\[\])*)", std::regex::optimize};
auto change_array = std::regex{R"(\[\])", std::regex::optimize};
// auto change_array = std::regex{};
auto native_rx = std::regex{R"(j(void|boolean|byte|char|short|int|long|float|double|handle<(j(object|class|string|thread))>))", std::regex::optimize};
auto remove_space = std::regex{" ", std::regex::optimize};
auto remove_keywords = std::regex{R"(\b(public|private|protected|static|abstract|final|synchronized|transient|native)\b)", std::regex::optimize};
auto to_jni = std::regex{R"(\b(void|boolean|byte|char|short|int|long|float|double)\b)", std::regex::optimize};
auto reduce_spaces = std::regex{R"( +)", std::regex::optimize};
auto replace_elipsis = std::regex{R"( ?\.\.\. ?)", std::regex::optimize};
auto replace_jobject = std::regex{R"(java.lang.Object(?!\w|\$))", std::regex::optimize};
auto replace_jclass = std::regex{R"(java.lang.Class(?!\w|\$))", std::regex::optimize};
auto replace_jstring = std::regex{R"(java.lang.String(?!\w|\$))", std::regex::optimize};
auto replace_jthread = std::regex{R"(java.lang.Thread(?!\w|\$))", std::regex::optimize};
auto replace_jthrowable = std::regex{R"(java.lang.Throwable(?!\w|\$))", std::regex::optimize};
auto remove_packages = std::regex{R"(\w+\.)", std::regex::optimize};
auto remove_throws = std::regex{R"(\) +throws.+?;)", std::regex::optimize};

static std::unordered_map<std::string, bool> is_native
{
  {"jvoid", true},
  {"jboolean", true},
  {"jbyte", true},
  {"jchar", true},
  {"jshort", true},
  {"jint", true},
  {"jlong", true},
  {"jfloat", true},
  {"jdouble", true},
  {"jhandle<jobject>", true},
  {"jhandle<jclass>", true},
  {"jhandle<jstring>", true},
  {"jhandle<jthread>", true},
  {"jhandle<jthrowable>", true}
};

auto pattern_iter(const std::string& string, std::regex& pattern)
{
  return std::sregex_iterator{std::begin(string), std::end(string), pattern};
}

std::string process_members(std::string& members)
{
  std::vector<std::string> lines;
  std::transform(pattern_iter(members, line_rx), {}, std::back_inserter(lines), [](auto& i) {return i.str();});

  std::string class_name;
  // Return type/constructor, method name, arguments
  std::vector<std::tuple<std::string, std::string, std::vector<std::string>>> methods;
  // Type, name
  std::vector<std::tuple<std::string, std::string>> fields;
  std::for_each(std::begin(lines), std::end(lines),
                [&methods, &fields, &class_name](const auto& i)
                {
                  auto paren_pos = i.find('(');
                  auto name_pos = i.find_last_of(' ', paren_pos) + 1;
                  auto type_pos = i.find_first_not_of(' ');
                  auto type_len = i.find_last_not_of(' ', (type_pos == name_pos /* is constructor */ ? paren_pos : name_pos) - 1) - type_pos + 1;
		  if(type_pos == name_pos) class_name = i.substr(type_pos, type_len);
                  if(paren_pos != std::string::npos && i.find('=') == std::string::npos)
                  {
                    methods.emplace_back(i.substr(type_pos, type_len), type_pos == name_pos ? "" : i.substr(name_pos, paren_pos - name_pos), std::vector<std::string>{});
                    std::transform(pattern_iter(i.substr(paren_pos+1, i.find_last_of(')') - paren_pos+1), arg_rx), {}, std::back_inserter(std::get<2>(methods.back())),
                                   [](auto& a)
                                   {
                                     return a.str();
                                   });
                  }
                  else fields.emplace_back(i.substr(type_pos, type_len), i.substr(name_pos, paren_pos - name_pos));
                });
  lines.clear();

  for(auto& f : fields)
    if(std::get<0>(f) == class_name) std::get<0>(f) += "_impl";
  for(auto& m : methods)
  {
    if(std::get<0>(m) == class_name) std::get<0>(m) += "_impl";
    for(auto& a : std::get<2>(m))
      if(a == class_name) a += "_impl";
  }
  class_name += "_impl";

  std::ostringstream out;
  for(auto&& l : fields)
  {
    if(std::get<0>(l).find('=') != std::string::npos)
    {
      std::get<1>(l) = std::get<0>(l).substr(std::get<0>(l).find(' ')+1);
      std::get<1>(l) = std::get<1>(l).substr(0, std::get<1>(l).find('='));
      std::get<0>(l) = std::get<0>(l).substr(0, std::get<0>(l).find(' '));
    }
    std::get<1>(l).pop_back();
    if(std::get<1>(l) == "signed")             std::get<1>(l) = "signed_";
    else if(std::get<1>(l) == "NULL")          std::get<1>(l) = "NULL_";
    else if(std::get<1>(l) == "DOMAIN")        std::get<1>(l) = "DOMAIN_";
    else if(std::get<1>(l) == "OVERFLOW")      std::get<1>(l) = "OVERFLOW_";
    else if(std::get<1>(l) == "UNDERFLOW")     std::get<1>(l) = "UNDERFLOW_";
    else if(std::get<1>(l) == "LITTLE_ENDIAN") std::get<1>(l) = "LITTLE_ENDIAN_";
    else if(std::get<1>(l) == "BIG_ENDIAN")    std::get<1>(l) = "BIG_ENDIAN_";
    else if(std::get<1>(l) == "EOF")           std::get<1>(l) = "EOF_";
    else if(std::get<1>(l) == "LOG")           std::get<1>(l) = "LOG_";

    auto array_pos = std::get<0>(l).find('[');
    auto type = std::get<0>(l).substr(0, array_pos);
    if(!is_native[type] && type != class_name) out << "template<typename " << type << ">\\" << std::endl;

    out << "auto " << std::get<1>(l) << "() const";
    if(array_pos != std::string::npos && !is_native[type] && type != class_name) out << " -> std::enable_if_t<is_jhandle<" << type << ">, " << type << '>';

    auto subscripts = std::count(std::begin(std::get<0>(l)),  std::end(std::get<0>(l)), '[');
    type += std::string(subscripts, '>');
    for(auto i = 0; i < subscripts; ++i) type = "jhandle<" + type;

    out << '\\' << std::endl << "{return jhandle<Raw>::template field<" << type << ">(\"" << std::get<1>(l) <<"\");}\\" << std::endl;
  }

  // Sort by arity, then
  // Sort by argument genericity, then
  // Sort by argument array rank, then
  // Sort by return type genericity, then
  // Sort by method name
  std::sort(std::begin(methods), std::end(methods), [](const auto& l, const auto& r){return std::get<2>(l).size() < std::get<2>(r).size();});
  std::stable_sort(std::begin(methods), std::end(methods),
                   [](const auto& l, const auto& r)
                   {
                     return is_native[std::get<0>(l).substr(0, std::get<0>(l).find('['))] > is_native[std::get<0>(r).substr(0, std::get<0>(r).find('['))];
                   });
  std::stable_sort(std::begin(methods), std::end(methods),
                   [](const auto& l, const auto& r)
                   {
                     return std::lexicographical_compare(std::begin(std::get<2>(l)), std::end(std::get<2>(l)),
                                                         std::begin(std::get<2>(r)), std::end(std::get<2>(r)));
                   });
  std::stable_sort(std::begin(methods), std::end(methods),
                   [](const auto& l, const auto& r)
                   {
                     return std::lexicographical_compare(std::begin(std::get<2>(l)), std::end(std::get<2>(l)),
                                                         std::begin(std::get<2>(r)), std::end(std::get<2>(r)),
                                                         [](const auto& a, const auto& b)
                                                         {
                                                           auto l_nat = is_native[a.substr(0, a.find('['))];
                                                           auto r_nat = is_native[b.substr(0, b.find('['))];
                                                           if(l_nat && r_nat) return a < b;
                                                           return l_nat > r_nat;
                                                         });
                   });
  std::stable_sort(std::begin(methods), std::end(methods),
                   [](const auto& l, const auto& r)
                   {
                     return std::lexicographical_compare(std::begin(std::get<2>(l)), std::end(std::get<2>(l)),
                                                         std::begin(std::get<2>(r)), std::end(std::get<2>(r)),
                                                         [](const auto& a, const auto& b)
                                                         {
                                                           return std::count(std::begin(a), std::end(a), '[') < std::count(std::begin(b), std::end(b), '[');
                                                         });
                   });
  std::stable_sort(std::begin(methods), std::end(methods), [](const auto& l, const auto& r){return std::get<1>(l) < std::get<1>(r);});

//   if(!methods.empty() && std::get<1>(methods.front()).empty() /* is constructor */) class_name = std::get<0>(methods.front());

  auto method_and_arity = [](const auto& l, const auto& r)
  {
    return std::get<1>(l) != std::get<1>(r) || std::get<2>(l).size() != std::get<2>(r).size();
  };

  decltype(methods) all_collapsed;
  std::size_t count = 0;
  for(auto lower = std::begin(methods), end = std::end(methods), upper = std::adjacent_find(lower, end, method_and_arity);
      lower != end; upper = std::adjacent_find((lower = std::min(end, upper + 1)), end, method_and_arity))
      {
        auto collapsed =
        std::accumulate(lower, std::min(end, upper + 1), decltype(methods){},
                        [&count](auto& v, const auto& i)
                        {
                          if(v.empty()) v.push_back(i);
                          else
                          {
                            if(!std::equal(std::begin(std::get<2>(v.back())), std::end(std::get<2>(v.back())),
                              std::begin(std::get<2>(i)), std::end(std::get<2>(i)),
                                          [](const auto& l, const auto& r)
                                          {
                                            if(std::count(std::begin(l), std::end(l), '[') != std::count(std::begin(r), std::end(r), '[')) return false;
                                            if(!is_native[l.substr(0, l.find('['))] && !is_native[r.substr(0, r.find('['))]) return true;
                                            return l ==  r;
                                          }))
                            {
                              v.emplace_back(std::get<0>(v.back()), std::get<1>(v.back()), std::get<2>(i));
                            }
                            else
                            {
                              auto r = std::begin(std::get<2>(i));
                              for(auto& l : std::get<2>(v.back()))
                              {
                                if(l == *r);
                                else if(l.find('[') != std::string::npos) l.insert(l.find('['), r->substr(0, r->find('[')));
                                else l +=  "__or__" + *r;
                                ++r;
                              }
                            }
                          }
                          ++count;
                          return v;
                        });
        std::copy(std::begin(collapsed), std::end(collapsed), std::back_inserter(all_collapsed));
      }
  assert(count == methods.size());

  for(auto& m : all_collapsed)
  {
    auto deduced_types =
    std::accumulate(std::begin(std::get<2>(m)), std::end(std::get<2>(m)), std::vector<std::string>{},
		    [&class_name](auto& v,  const auto& i)
		    {
		      auto type = i.substr(0, i.find('['));
		      if(!is_native[type] && type != class_name && std::find(std::begin(v), std::end(v), type) == std::end(v)) v.push_back(type);
		      return v;
		    });
    auto return_type = std::get<0>(m).substr(0, std::get<0>(m).find('['));
    if(!is_native[return_type] && return_type != class_name && std::find(std::begin(deduced_types), std::end(deduced_types), return_type) == std::end(deduced_types)) deduced_types.insert(std::begin(deduced_types),  return_type);

    auto arg_types =
    std::accumulate(std::begin(std::get<2>(m)), std::end(std::get<2>(m)), std::vector<std::string>{},
		    [](auto& v,  const auto& i)
		    {
		      auto type = i.substr(0, i.find('['));
		      auto subscripts = std::count(std::begin(i),  std::end(i), '[');
		      type += std::string(subscripts, '>');
		      for(auto i = 0; i < subscripts; ++i) type = "jhandle<" + type;
                      if(type.find("jhandle") != std::string::npos) type = "const " + type + '&';
                      else if(!is_native[type]) type += "&&";
                      v.push_back(type);
                      return v;
		    });

    auto subscripts = std::count(std::begin(std::get<0>(m)),  std::end(std::get<0>(m)), '[');
    std::get<0>(m) = std::get<0>(m).substr(0, std::get<0>(m).find('['));
    std::get<0>(m) += std::string(subscripts, '>');
    for(auto i = 0; i < subscripts; ++i) std::get<0>(m) = "jhandle<" + std::get<0>(m);

    if(!deduced_types.empty())
    {
      out << "template<";
      std::transform(std::begin(deduced_types), std::end(deduced_types), std::ostream_iterator<std::string>(out, ", "),
		      [](const auto& i)
		      {
			return "typename " + i;
		      });
      out << ">\\" << std::endl;
    }

    if(std::get<1>(m) == "union") std::get<1>(m) = "union_";
    else if(std::get<1>(m) == "register") std::get<1>(m) = "register_";
    else if(std::get<1>(m) == "delete") std::get<1>(m) = "delete_";
    else if(std::get<1>(m) == "and") std::get<1>(m) = "and_";
    else if(std::get<1>(m) == "or") std::get<1>(m) = "or_";
    else if(std::get<1>(m) == "not") std::get<1>(m) = "not_";
    else if(std::get<1>(m) == "xor") std::get<1>(m) = "xor_";
    else if(std::get<1>(m) == "namespace") std::get<1>(m) = "namespace_";
    else if(std::get<1>(m) == "export") std::get<1>(m) = "export_";
    else if(std::get<1>(m) == class_name) std::get<1>(m) += '_';

    out << (std::get<1>(m).empty() ? std::get<0>(m) : "auto") << ' ' << (std::get<1>(m).empty() ? "" : std::get<1>(m) + ' ') << '(';
    std::transform(std::begin(arg_types), std::end(arg_types), std::ostream_iterator<std::string>(out, ", "),
		    [a = 0](const auto& i) mutable
		    {
		      return i + " arg" + std::to_string(a++);
		    });
    out << ')';
    if(!std::get<1>(m).empty())
    {
      out << " const ";
      if(deduced_types.empty()) out << "-> " << std::get<0>(m);
      else
      {
	out << "-> std::enable_if_t<";
	std::transform(std::begin(deduced_types), std::end(deduced_types), std::ostream_iterator<std::string>(out, " && "),
			[](const auto& i)
			{
			  return "is_jhandle<" + i + '>';
			});
	out << ", " << std::get<0>(m) << '>';
      }
    }
    if(std::get<1>(m).empty())
    {
      out << " : jhandle<Raw>(" << std::get<0>(m) << "::get_signature(), ";
      std::transform(std::begin(arg_types), std::end(arg_types), std::ostream_iterator<std::string>(out, ", "),
                     [a = 0](const auto& i) mutable
                     {
                       if(i.find("&&") != std::string::npos)
                       {
                         return "std::forward<" + i.substr(0, i.find("&&")) + ">(arg" + std::to_string(a++) + ")";
                       }
                       return "arg" + std::to_string(a++);
                     });
      out << ") {}\\" << std::endl;
    }
    else
    {
      out << "\\\n{return jhandle<Raw>::template method<" << std::get<0>(m) << ">(\"" << std::get<1>(m) << "\", ";
      std::transform(std::begin(arg_types), std::end(arg_types), std::ostream_iterator<std::string>(out, ", "),
		      [a = 0](const auto& i) mutable
		      {
                        if(i.find("&&") != std::string::npos)
                        {
                          return "std::forward<" + i.substr(0, i.find("&&")) + ">(arg" + std::to_string(a++) + ")";
                        }
			return "arg" + std::to_string(a++);
		      });
      out << ");}\\" << std::endl;
    }
  }

  members = out.str();
  members = std::regex_replace(members, std::regex{", \\)"}, ")");
  members = std::regex_replace(members, std::regex{", >"}, ">");
  members = std::regex_replace(members, std::regex{", \\}"}, "}");
  members = std::regex_replace(members, std::regex{" && ,"}, ",");

  return members;
}

int main(int c, char** v)
{
  int rc = 0;
  Usage usage;
  if(std::regex_match(c > 1 ? v[1] : "", std::regex{"-h|--help"}) || !std::getenv("JAVA_HOME")) return 0;

  std::string javhom = std::getenv("JAVA_HOME");
  std::string jarfil = c > 1 ? v[1] : javhom + "/jre/lib/rt.jar";
  if(!std::ifstream{jarfil})
  {
    rc = -1;
    std::cerr << "File \"" << jarfil << "\" cannot be opened" << std::endl;
    return rc;
  }

  usage.enable = false;

  std::string tmpdir = std::getenv("TMPDIR") ? std::getenv("TMPDIR") : "/tmp";
  auto class_list = tmpdir + "/class_list.txt";

  auto jarcmd = javhom + R"(/bin/jar tf %1 | grep .class | grep -v /internal/ | grep -v /impl/ | grep -v package-info | grep -v java.lang.Object | grep -v java.lang.Class | grep -v java.lang.String | grep -v java.lang.Thread | grep -v java.lang.Throwable >)" + class_list;

  jarcmd = std::regex_replace(jarcmd, std::regex{"%1"}, jarfil);
  if((rc = std::system(jarcmd.c_str()))) return rc;

  std::vector<std::string> classes;
  std::ifstream class_list_file{class_list};
  std::copy_if(std::istream_iterator<std::string>(class_list_file), std::istream_iterator<std::string>(), std::inserter(classes, std::begin(classes)),
               [c, v](const auto& class_name)
               {
                 return c < 3 || !std::regex_search(class_name, std::regex{v[2]});
               });

  std::sort(std::begin(classes), std::end(classes));
  for(auto& cls : classes) cls = std::regex_replace(cls, remove_class, "");
  std::vector<std::string> escaped;
  for(auto& cls : classes) escaped.emplace_back(std::regex_replace(cls, to_inner, R"(\$$)"));

  {
    std::ofstream class_stream{tmpdir + "/class_defs.sh"};
    class_stream << "javap -classpath " << jarfil << " -public -constants ";
    std::copy(std::begin(escaped), std::end(escaped), std::ostream_iterator<std::string>(class_stream, " "));
    class_stream << " > " << tmpdir << "/class_defs.txt";
  }

  if((rc = std::system(("chmod 744 " + tmpdir + "/class_defs.sh").c_str()))) return rc;
  if((rc = std::system((tmpdir + "/class_defs.sh").c_str()))) return rc;

  std::vector<std::string> members_list;
  std::string line;
  for(std::ifstream class_stream{tmpdir + "/class_defs.txt"}; std::getline(class_stream, line, '}');) members_list.push_back(line.substr(line.find('{') + 1));
  line.clear();

  std::vector<std::string> results;
  std::generate_n(std::back_inserter(results), classes.size(), []()
  {
    return
R"(#define import_%%MACRO_NAME%% \
%%NAMESPACE%%\
template<typename Raw = jobject>\
struct %%NAME%%_impl : jhandle<Raw>\
{\
using jhandle<Raw>::jhandle;\
static std::string get_signature()\
{\
return "%%SIGNATURE%%";\
}\
%%MEMBERS%%\
};\
using %%NAME%% = %%NAME%%_impl<jobject>;\
using %%NAME%%_cls = %%NAME%%_impl<jclass>;\
%%NAME%% %%NAME%%_arg{nullptr};\
%%NAMESPACE_END%%\
)";
  });


  std::vector<std::string> macros;
  for(auto& cls : classes) macros.emplace_back(std::regex_replace(cls, to_macro, "_"));
  std::vector<std::string> qual_names;
  for(auto& cls : classes) qual_names.emplace_back(std::regex_replace(std::regex_replace(cls, to_qual_name, "_"), to_qualifier, "$1::"));
  for(auto& ns : qual_names) ns = std::regex_replace(ns, to_inner, "_");
  std::vector<std::string> names;
  for(auto& ns : qual_names) names.emplace_back(ns.substr(ns.find_last_of(':')+1));
  std::vector<std::string> signatures;
  for(auto& cls : classes) signatures.emplace_back("L" + std::regex_replace(cls, to_signature, "/") + ";");
  std::vector<std::string> namespaces;
  for(auto& ns : qual_names) namespaces.emplace_back(std::regex_replace(ns, to_namespace, "namespace $1 { "));
  for(auto& ns : namespaces) ns = ns.substr(0, ns.find_last_of(' ')+1);
  for(auto& ns : namespaces) ns = std::regex_replace(ns, escape_namespace, "namespace $1_ {");
  std::vector<std::string> endbraces;
  for(auto& cls : classes) endbraces.emplace_back(std::count(std::begin(cls), std::end(cls), '/'), '}');
  classes.clear();

  auto accumulator = [](auto&& v, auto&& r)
  {
    v.emplace_back(std::forward<decltype(r)>(r));
    return std::move(v);
  };

  std::string variable;
  auto product = [&](const auto& l, const auto& r)
  {
    return std::move(std::regex_replace(l, std::regex{variable}, r));
  };

  auto process = [&](auto& v)
  {
    results = std::inner_product(std::begin(results), std::end(results), std::begin(v),
                                 std::vector<std::string>{}, accumulator, product);
  };

  variable = "%%MACRO_NAME%%";
  process(macros);
  macros.clear();

  variable = "%%QUAL_NAME%%";
  process(qual_names);
  qual_names.clear();

  variable = "%%NAME%%";
  process(names);
  names.clear();

  variable = "%%NAMESPACE%%";
  process(namespaces);
  namespaces.clear();

  variable = "%%NAMESPACE_END%%";
  process(endbraces);
  endbraces.clear();

  for(auto& members : members_list)
  {
    members = std::regex_replace(members, remove_keywords, "");
    members = std::regex_replace(members, to_jni, "j$1");

    if(members.find('<') != std::string::npos)
    {
      std::size_t bracket_count = 0;
      for(auto& c : members)
      {
        bracket_count += c == '<';
        bracket_count -= c == '>';
        if(bracket_count > 0 || c == '>') c = '~';
      }
    }

    members = std::regex_replace(members, std::regex{"~"}, "");
    members = std::regex_replace(members, reduce_spaces, " ");
    members = std::regex_replace(members, replace_elipsis, "[]");
    members = std::regex_replace(members, replace_jobject, "jhandle<jobject>");
    members = std::regex_replace(members, replace_jclass, "jhandle<jclass>");
    members = std::regex_replace(members, replace_jstring, "jhandle<jstring>");
    members = std::regex_replace(members, replace_jthread, "jhandle<jthread>");
    members = std::regex_replace(members, replace_jthrowable, "jhandle<jthrowable>");
    members = std::regex_replace(members, remove_packages, "");
    members = std::regex_replace(members, to_inner, "_");
    members = std::regex_replace(members, remove_throws, ");");
    members = process_members(members);
  }

  variable = "%%MEMBERS%%";
  process(members_list);
  members_list.clear();

  variable = "%%SIGNATURE%%";
  process(signatures);
  signatures.clear();

  std::move(std::begin(results), std::end(results), std::ostream_iterator<std::string>(std::cout, "\n\n"));

  return 0;
}
