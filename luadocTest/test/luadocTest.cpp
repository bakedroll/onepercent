#include <gtest/gtest.h>

#include <luadoc/Namespace.h>
#include <QDir>

class A
{
public:
  A() {}

  void        function_test() {}
  const char* function_const_char() const { return ""; }
};

class Z
{
public:
  A  a_val_function() const { return m_a; }
  A& a_ref_function() { return m_a; }
  A* a_ptr_function() { return &m_a; }

private:
  A m_a;
};

class B
{
};

class C : public B
{
public:
  void        function_void() {}
  int         function_int() { return 0; }
  std::string function_string() { return ""; }

  int  property_rw_getter() const { return 0; }
  void property_rw_setter(int value) {}

  std::string property_r_getter() const { return ""; }
};

TEST(LuaDocTest, Test)
{
  auto L = luaL_newstate();
  luaL_openlibs(L);

  luadoc::getGlobalNamespace(L)
    .beginClass<Z>("Z")
    .addFunction("a_val_function", &Z::a_val_function)
    .addFunction("a_ref_function", &Z::a_ref_function)
    .addFunction("a_ptr_function", &Z::a_ptr_function)
    .endClass()
    .beginClass<A>("A")
    .addConstructor<void (*) ()>()
    .addFunction("function_test", &A::function_test)
    .addFunction("function_const_char", &A::function_const_char)
    .endClass()
    .beginNamespace("ns_a")
      .beginNamespace("ns_a_a")
        .beginClass<B>("B")
        .endClass()
      .endNamespace()
    .endNamespace()
    .beginNamespace("ns_b")
      .deriveClass<C, B>("C")
      .addFunction("function_void", &C::function_void)
      .addFunction("function_int", &C::function_int)
      .addFunction("function_string", &C::function_string)
      .addProperty("property_rw", &C::property_rw_getter, &C::property_rw_setter)
      .addProperty("property_r", &C::property_r_getter)
      .endClass()
    .endNamespace();

  auto dir = QDir::current();
  dir.mkdir("doc");
  dir.cd("doc");

  luadoc::DocsGenerator::instance().generate("luadocTest", dir.path(), QDir::current().filePath("descriptions.csv"));

  dir.removeRecursively();

  lua_close(L);
}
