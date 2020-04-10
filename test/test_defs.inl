//
// Created by maxwell on 2020-04-09.
//

#ifndef METAPARSE_TEST_DEFS_INL
#define METAPARSE_TEST_DEFS_INL

#include <string>
#include <memory>
#include <vector>

namespace {
  class TestResult
  {
    const static std::string newline;

    const bool passed;
    const std::string passedStr = passed ? "Test Passed: " : "Test Failed: ";
  public:
    TestResult(bool passed) : passed(passed){}

    [[nodiscard]] virtual std::string message() const noexcept = 0;
    [[nodiscard]] operator std::string() const noexcept
    {
      return passedStr + message() + newline;
    }
    [[nodiscard]] explicit operator bool() const noexcept
    {
      return passed;
    }
  };
  const std::string TestResult::newline = "\n";

  using TestFunc = std::unique_ptr<TestResult>();

  class Test
  {
    std::string name;
    std::unique_ptr<TestFunc> test_func;
  public:
      Test(std::string name, TestFunc* func) : name(std::move(name)), test_func(func){}
      [[nodiscard]] std::unique_ptr<TestResult> run_test() const
      {
        return std::move(test_func.get()());
      }
  };

  class TestPass : TestResult
  {
  public:
    TestPass() : TestResult(true){}

    [[nodiscard]] std::string message() const noexcept override {
      return "no errors";
    }
  };
  class TestFailure : TestResult
  {
  public:
    TestFailure() : TestResult(false){}

    [[nodiscard]] std::string message() const noexcept override {
      return "unknown error";
    }
  };
}

struct TestRunner
{
  std::vector<std::unique_ptr<Test>> test_list;
};

#define ADD_TEST(name, test_function) \
  struct \
  {\
    \
  } name;

#endif//METAPARSE_TEST_DEFS_INL
