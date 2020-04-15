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
#include <llvm/ADT/StringMap.h>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>




class index_iterator{
    size_t pos;
  public:
      index_iterator(size_t pos) : pos(pos){}

      bool operator ==(const index_iterator& other) const{
        return pos == other.pos;
      }
      bool operator !=(const index_iterator& other) const{
        return pos != other.pos;
      }

      index_iterator& operator++(){++pos; return *this;}
      size_t operator*() const{
        return pos;
      }
  };
class iota_range{
    const size_t first = 0;
    const size_t last;
  public:
    iota_range(size_t last) : last(last){}
    iota_range(size_t first, size_t last) : first(first), last(last){}
    index_iterator begin() const{
      return first;
    }
    index_iterator end() const{
      return last;
    }
  };

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
  template <typename T>
  void set(const llvm::StringRef& name, T&& value){
    if(!variables.insert({name, EnvVarValue(value)}).second) {
      if (auto &&opt = variables[name]; opt.hasValue())
        opt.getValue() = value;
      else
        opt.emplace(value);
    }
  }
  template <typename T, size_t N>
  void set(const llvm::StringRef& name, const T (& value)[N]){
    set(name, llvm::StringRef(value));
  }
};
struct TestPrecondition : std::enable_shared_from_this<TestPrecondition>
{
  virtual std::string message() const {return "";};
  virtual bool isSatisfied(const Environment&) const = 0;
};


namespace {
  class Test;
  class TestResult
  {
    using ConditionVector = std::vector<std::shared_ptr<TestPrecondition>>;
    using MarkedConditionVector = std::vector<std::pair<std::shared_ptr<TestPrecondition>, bool>>;

    std::shared_ptr<Test> test_ptr;
    const static std::string newline;
    const bool passed;
    const std::string passedStr = passed ? " Passed: " : " Failed: ";

    friend class Test;

    [[nodiscard]] virtual std::string message() const noexcept = 0;

    class MarkedConditionRange {
      const ConditionVector& ref;
      const Environment& env;
      class Iterator
      {
        const ConditionVector& ref;
        const Environment& env;
        size_t pos;
      public:
          Iterator(const Environment& env, const ConditionVector& ref) : ref(ref), pos(0), env(env){}

          bool operator!=(size_t endpos) const noexcept{
            return pos != endpos;
          }
          auto operator*() const{
            const auto& cond = ref[pos];
            return std::pair{cond, cond->isSatisfied(env)};
          }
          Iterator& operator++(){
            ++pos;
            return *this;
          }
      };
    public:
        MarkedConditionRange(const Environment& env, const ConditionVector& ref) : ref(ref), env(env){}

        Iterator begin() const{
          return Iterator(env, ref);
        }
        size_t end() const{
          return ref.size();
        }
    };

  public:
    TestResult(bool passed) : passed(passed){}

    [[nodiscard]] std::string to_string() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept
    {
      return passed;
    }
    [[nodiscard]] bool pass() const noexcept{
      return passed;
    }

    [[nodiscard]] bool expectedToPass() const;
    [[nodiscard]] bool expectedToFail() const;

    [[nodiscard]] ConditionVector getUnsatisfiedPreconditions() const;
    [[nodiscard]] ConditionVector getSatisfiedPreconditions() const;

    [[nodiscard]] MarkedConditionRange getPreconditions() const;
  };
  const std::string TestResult::newline = "\n";

  using TestFunc = std::unique_ptr<TestResult>();

  class Test : public std::enable_shared_from_this<Test>
  {
    using ConditionVector = typename TestResult::ConditionVector;
    using MarkedConditionVector = typename TestResult::MarkedConditionVector;

    const Environment& env;
    std::string name;
    ConditionVector preconditions;
    TestFunc* const test_func;
    std::unique_ptr<TestResult> test_result;

    friend class TestResult;

  public:
      Test(const Environment& env, std::string name, TestFunc* func, const ConditionVector& precons = {}) :
         env(env),
         name(std::move(name)),
         test_func(func),
         test_result(nullptr),
         preconditions(precons){
        /*llvm::for_each(precons, [this, &env](const std::shared_ptr<TestPrecondition>& cond){preconditions.push_back({cond, cond->isSatisfied(env)});});
        expected = llvm::all_of(preconditions, [&env](auto&& cond){return cond.second;});*/
      }

      std::unique_ptr<TestResult>&& runTest()
      {
        test_result = std::move(test_func());
        test_result->test_ptr = shared_from_this();
        return std::move(test_result);
      }
  };


  [[nodiscard]] std::string TestResult::to_string() const noexcept {
    return test_ptr->name + ": " + this->message();
  }
  [[nodiscard]] bool TestResult::expectedToPass() const {
    return llvm::all_of(test_ptr->preconditions, [&env = test_ptr->env](auto&& cond){return cond->isSatisfied(env);});
  }
  [[nodiscard]] bool TestResult::expectedToFail() const{
    return !expectedToPass();
  }


  [[nodiscard]] TestResult::ConditionVector TestResult::getUnsatisfiedPreconditions() const
  {
    namespace views = ranges::views;
    ConditionVector vec;
    const auto predicate = [&env = test_ptr->env](auto&& cond){return !cond->isSatisfied(env);};
    for(auto&& cond : test_ptr->preconditions | views::filter(predicate))
      vec.push_back(cond);
    return vec;
  }
  [[nodiscard]] TestResult::ConditionVector TestResult::getSatisfiedPreconditions() const
  {
    namespace views = ranges::views;
    ConditionVector vec;
    const auto predicate = [&env = test_ptr->env](auto&& cond){return cond->isSatisfied(env);};
    for(auto&& cond : test_ptr->preconditions | views::filter(predicate))
      vec.push_back(cond);
    return vec;
  }
  [[nodiscard]] TestResult::MarkedConditionRange TestResult::getPreconditions() const{
    return {test_ptr->env, test_ptr->preconditions};
  }


  class TestPass : public TestResult
  {
  public:
    TestPass() : TestResult(true){}

    [[nodiscard]] std::string message() const noexcept override {
      return "Passed";
    }
  };
  class TestFailure : public TestResult
  {
    std::string str;
  public:
    TestFailure(const std::string& str) : str(str), TestResult(false){}
    TestFailure() : TestFailure("Unknown Error"){}

    [[nodiscard]] std::string message() const noexcept override {
      return str;
    }
  };



  template <typename T>
  auto zip_with_indices(const T& container){
    return llvm::zip(container, llvm::make_range(index_iterator(0), index_iterator(container.size())));
  }

  class TestList
  {
    std::vector<std::shared_ptr<Test>> test_list;
public:
    TestList() = default;
    template <typename ...T> requires (std::is_same_v<T, Test> && ...)
    TestList(std::shared_ptr<T>& ...t) : test_list{t->shared_from_this()...}{}

    template <typename ...T> requires (std::is_same_v<T, Test> && ...)
    void enqueue(std::shared_ptr<T>& ...t){
      (test_list.push_back(t->shared_from_this()), ...);
    }

    std::vector<std::unique_ptr<TestResult>> run_all(){
      std::vector<std::unique_ptr<TestResult>> results;
      results.reserve(test_list.size());

      for(auto&& test : test_list){
        results.push_back(std::move(test->runTest()));
      }
      return results;
    }
  };
}

#define ADD_TEST(name, test_function) \
  struct \
  {\
    \
  } name;

#endif//METAPARSE_TEST_DEFS_INL
