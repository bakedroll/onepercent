#include <gtest/gtest.h>

#include <luadoc/Namespace.h>
#include <QDir>

class A
{
  
};

class Z
{
  
};

class B
{
public:
  void function_test1() {}
};

class C : public B
{
public:
  void        function_test1() {}
  int         function_test2() { return 0; }
  std::string function_test3() { return ""; }

  int  property_test1_getter() const { return 0; }
  void property_test1_setter(int value) {}

  std::string property_test2_getter() const { return ""; }
};

TEST(LuaDocTest, Test)
{
  auto L = luaL_newstate();
  luaL_openlibs(L);

  luadoc::getGlobalNamespace(L)
    .beginClass<Z>("Z")
    .endClass()
    .beginClass<A>("A")
    .endClass()
    .beginNamespace("ns_a")
      .beginNamespace("ns_a_a")
        .beginClass<B>("B")
        .addFunction("function_test1", &B::function_test1)
        .endClass()
      .endNamespace()
    .endNamespace()
    .beginNamespace("ns_b")
      .deriveClass<C, B>("C")
      .addFunction("function_test1", &C::function_test1)
      .addFunction("function_test2", &C::function_test2)
      .addFunction("function_test3", &C::function_test3)
      .addProperty("property_test1", &C::property_test1_getter, &C::property_test1_setter)
      .addProperty("property_test2", &C::property_test2_getter)
      .endClass()
    .endNamespace();

  auto dir = QDir::current();
  dir.mkdir("doc");
  dir.cd("doc");

  luadoc::DocsGenerator::instance().generate("luadocTest", dir.path(), QDir::current().filePath("descriptions.csv"));

  dir.removeRecursively();

  lua_close(L);
}
