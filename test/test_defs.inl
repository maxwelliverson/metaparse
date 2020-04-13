//
// Created by maxwell on 2020-04-09.
//

#ifndef METAPARSE_TEST_DEFS_INL
#define METAPARSE_TEST_DEFS_INL

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <llvm/Support/Error.h>
#include <llvm/Support/Regex.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/StringExtras.h>






namespace string_constants
{
  const std::string period = ".";
  const std::string comma = ",";
  const std::string space = " ";
  const std::string colon = ":";
}
class Version
{
  const unsigned major;
  const unsigned minor;
  const unsigned build;

public:
  explicit Version(unsigned major = 0, unsigned minor = 0, unsigned build = 0) :
    major(major), minor(minor), build(build){}

  [[nodiscard]] std::string toString() const
  {
    using namespace string_constants;
    return std::to_string(major) + period + std::to_string(minor) + period + std::to_string(build);
  }
  [[nodiscard]] unsigned getMajor() const {
    return major;
  }
  [[nodiscard]] unsigned getMinor() const {
    return minor;
  }
  [[nodiscard]] unsigned getBuild() const {
    return build;
  }

  friend bool operator <(const Version& a, const Version& b) {
    if(a.major != b.major)
      return a.major < b.major;
    if(a.minor != b.minor)
      return a.minor < b.minor;
    return a.build < b.build;
  }
  friend bool operator ==(const Version& a, const Version& b){
    return a.major == b.major && a.minor == b.minor && a.build == b.build;
  }
  friend bool operator <=(const Version& a, const Version& b)
  {
    return a < b || a == b;
  }
  friend bool operator >(const Version& a, const Version& b){
    return !(a <= b);
  }
  friend bool operator >=(const Version& a, const Version& b) {
    return !(a < b);
  }
  friend bool operator !=(const Version& a, const Version& b) {
    return !(a == b);
  }
};

llvm::Expected<Version> parseVersionString(const llvm::StringRef& str)
{
    using namespace std::string_literals;
    static llvm::Regex version_regex{R"reg(([\d]*).([\d]).?([\d]?))reg"s};
    llvm::SmallVector<llvm::StringRef, 4> matches;

    struct MatchError : llvm::StringError
    {
      explicit MatchError(const llvm::Twine& ref) :
         llvm::StringError("Could not match version "s + ref, std::make_error_code(std::errc::invalid_argument)){}
    };

    if(!version_regex.match(str, &matches))
      return llvm::make_error<MatchError>(str);

    unsigned version_nums[3]{};
    for(auto&& [num, sub] : llvm::zip(version_nums, llvm::make_range(matches.data() + 1, matches.data() + matches.size()))){
      if(!sub.empty()) {
        if (unsigned proxy = 0; sub.getAsInteger(10, proxy))
          num = proxy;
        else
          return llvm::make_error<MatchError>(sub + llvm::StringLiteral(" from string ") + str);
      }
      num = 0;
    }

    constexpr auto helper = []<auto...I>(const unsigned (&nums)[3], const std::index_sequence<I...>&){return Version(nums[I]...);};
    constexpr auto seq = std::make_index_sequence<3>();
    return helper(version_nums, seq);
}




class Environment
{
  using EnvVarValue = llvm::Optional<std::variant<llvm::StringRef, bool, int, Version>>;
  using EnvVarMap = llvm::StringMap<EnvVarValue>;

  EnvVarMap variables;
public:
  using VariableType = std::variant<llvm::StringRef, bool, int, Version>;

  [[nodiscard]] EnvVarValue get(const llvm::StringRef& name) const{
    return variables.lookup(name);
  }
  void set(const llvm::StringRef& name, const EnvVarValue& value){
    variables.insert({name, value});
  }
};
struct TestPrecondition
{
  virtual llvm::StringRef message() const = 0;
  virtual bool isSatisfied(const Environment&) const = 0;
};


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
    using ConditionVector = std::vector<std::shared_ptr<TestPrecondition>>;
    using MarkedConditionVector = std::vector<std::pair<std::shared_ptr<TestPrecondition>, bool>>;

    std::string name;
    MarkedConditionVector preconditions;
    std::unique_ptr<TestFunc> test_func;
    std::unique_ptr<TestResult> test_result;
    bool expected;

  public:
      Test(const Environment& env, std::string name, TestFunc* func, const ConditionVector& precons = {}) :
         name(std::move(name)),
         test_func(func),
         test_result(nullptr),
         preconditions(){
        llvm::for_each(precons, [this, &env](std::shared_ptr<TestPrecondition>& cond){preconditions.push_back({cond, cond->isSatisfied(env)});});
        expected = llvm::all_of(preconditions, [&env](auto&& cond){return cond.second;});
      }

      const std::unique_ptr<TestResult>& runTest()
      {
        test_result = std::move(test_func.get()());
        return test_result;
      }
      [[nodiscard]] bool expectedToPass() const{
        return expected;
      }
      [[nodiscard]] bool expectedToFail() const{
        return !expected;
      }

      [[nodiscard]] ConditionVector getUnsatisfiedPreconditions() const
      {
        ConditionVector vec;
        for(auto&& cond : preconditions){
          if(!cond.second)
            vec.push_back(cond.first);
        }
        return vec;
      }
      [[nodiscard]] ConditionVector getSatisfiedPreconditions() const
      {
        ConditionVector vec;
        for(auto&& cond : preconditions){
          if(cond.second)
            vec.push_back(cond.first);
        }
        return vec;
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
  std::vector<Test> test_list;
};

#define ADD_TEST(name, test_function) \
  struct \
  {\
    \
  } name;

#endif//METAPARSE_TEST_DEFS_INL
