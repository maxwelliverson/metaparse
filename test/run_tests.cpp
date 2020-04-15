//
// Created by maxwell on 2020-04-09.
//

#include "meta/concepts.ii"

constexpr auto get_env_str = [](auto&& var)->std::string{
  constexpr struct{
    std::string operator()(const llvm::StringRef& string) const{
      return string.str();
    }
    std::string operator()(int i) const{
      return std::to_string(i);
    }
    std::string operator()(bool b) const{
      return b ? "True" : "False";
    }
    std::string operator()(const Version& ver) const{
      return ver.toString();
    }
  }visitor;
  if(var.hasValue())
    return std::visit(visitor, var.getValue());
  return "N/A";
};

class AtLeast : public TestPrecondition{
  llvm::StringRef str;
  size_t val;
  mutable const Environment* last_env = nullptr;

public:
  template <typename Char, size_t N>
  AtLeast(const Char (& str)[N], size_t val) : str(str), val(val){}

  [[nodiscard]] bool isSatisfied(const Environment & env) const override {
    last_env = &env;

    if(auto&& value = env.get(str)){
      auto&& variant = value.getValue();
      switch(variant.index()){
        case 0: {
          size_t integral_val;
          std::get<0>(variant).getAsInteger(10, integral_val);
          return val < integral_val;
        }
        case 1:{
          return std::get<1>(variant) && !val;
        }
        case 2:{
          return val < std::get<2>(variant);
        }
        case 3:{
          auto&& ver = std::get<3>(variant);
          size_t ver_int = 0;
          ver_int += ver.getBuild();
          ver_int += ver.getMinor() * 100;
          ver_int += ver.getMajor() * 1000;
          return val < ver_int;
        }
        default:
          assert(variant.index() < 4 && "You shouldn't be here!");
      }
    }
    return false;
  }
  [[nodiscard]] std::string message() const override {
    return (llvm::StringRef("Environment Variable ") + str +
            + " [Value = " + get_env_str(last_env->get(str)) + "] must be greater than " + std::to_string(val)).str();
  }
};

using GreaterThan = std::shared_ptr<TestPrecondition>;

Environment env;

auto expect_to_pass(){
  using Ret = std::unique_ptr<TestResult>;

  //Effectively a runtime "#ifdef"
  if(env.get("TEST_NUM"))
    return Ret{new TestPass()};
  else
    return Ret{new TestFailure("TEST_NUM is not defined")};
};
auto expect_to_fail(){
  using Ret = std::unique_ptr<TestResult>;

  //Effectively a runtime "#ifdef"
  if(env.get("TEST_NUM_2"))
    return Ret{new TestPass()};
  else
    return Ret{new TestFailure("TEST_NUM_2 is not defined")};
};

template <typename ...Args> requires ((pram::meta::derived_from<Args, TestPrecondition> || std::is_same_v<Args, TestPrecondition>) && ...)
auto precondition_list(std::shared_ptr<Args>& ...args){
  return std::vector<std::shared_ptr<TestPrecondition>>{args->shared_from_this()...};
}

constexpr auto passed_str = [](bool B){return B ? "Passed" : "Failed";};

#include <iostream>
#include <thread>
template <typename T>
void run_and_print(T& tests)
{
  static size_t count = 0;

  size_t passed_expected = 0, passed_unexpected = 0, failed_expected = 0, failed_unexpected = 0, total = 0;

  std::cout << "Test Run #" << ++count << "\n" << std::endl;
  std::cout << "-------------------------------------------------------------------\n\n";
  for(auto&& result : tests.run_all()){
    ++total;
    if(bool pass = result->pass(), expected = result->expectedToPass(); pass == expected){
      std::cout << result->to_string() << std::endl;
      std::cout << "Expected to Pass: " << std::boolalpha << expected;
      if(!expected){
        ++failed_expected;
        std::cout << "\nPrecondition List: \n";
        for(auto&& [cond, passed] : result->getPreconditions())
          std::cout << "\t-> " << cond->message() << ": " << passed_str(passed) << std::endl;
      } else
        ++passed_expected;
      std::cout << "\n" << std::endl;
    } else {
      std::cerr << result->to_string() << std::endl;
      std::cerr << "Expected to Pass: " << std::boolalpha << expected;
      if(!expected){
        ++passed_unexpected;
        std::cerr << "\nPrecondition List: \n";
        for(auto&& [cond, passed] : result->getPreconditions())
          std::cerr << "\t-> " << cond->message() << ": " << passed_str(passed) << std::endl;
      } else
        ++failed_unexpected;
      std::cerr << "\n" << std::endl;
    }
    using namespace std::chrono_literals;
    std::cout << std::flush;
    std::cerr << std::flush;
    std::this_thread::sleep_for(50ms);
  }

  std::cout << "\n\nTest Results:\n";
  std::cout << "\tPassed: " << (passed_expected + passed_unexpected) << " out of " << total << "\n";
  std::cout << "\t\tExpected: " << passed_expected << " out of " << (passed_expected + passed_unexpected) << "\n";
  std::cout << "\t\tUnexpected: " << passed_unexpected << " out of " << (passed_expected + passed_unexpected) << "\n";
  std::cout << "\tFailed: " << (failed_expected + failed_unexpected) << " out of " << total << "\n";
  std::cout << "\t\tExpected: " << failed_expected << " out of " << (failed_expected + failed_unexpected) << "\n";
  std::cout << "\t\tUnexpected: " << failed_unexpected << " out of " << (failed_expected + failed_unexpected) << "\n\n";
  std::cout << "-------------------------------------------------------------------\n\n\n";
}







int main()
{

  env.set("TEST_NUM", 3);
  env.set("TEST_STRING", "10");


  std::shared_ptr<TestPrecondition> test_num_gt_2(new AtLeast("TEST_NUM", 2));
  std::shared_ptr<TestPrecondition> test_str_gt_3(new AtLeast("TEST_STRING", 3));
  std::shared_ptr<TestPrecondition> test_num_gt_6(new AtLeast("TEST_NUM", 6));
  std::shared_ptr<TestPrecondition> test_str_gt_15(new AtLeast("TEST_STRING", 15));



  std::shared_ptr<Test> A{new Test(env, "Is TEST_NUM Defined (no preconditions)", expect_to_pass)};



  std::shared_ptr<Test> B{new Test(env, "Is TEST_NUM_2 Defined (no preconditions)", expect_to_fail)};



  std::shared_ptr<Test> C{
    new Test(env, "Is TEST_NUM Defined (precondition: TEST_NUM is greater than 2)", expect_to_pass,
    precondition_list(test_num_gt_2))
  };


  std::shared_ptr<Test> D{
    new Test(env,"Is TEST_NUM_2 Defined (precondition: TEST_STRING is greater than 3)", expect_to_fail,
    precondition_list(test_str_gt_3))
  };

  std::shared_ptr<Test> E{
    new Test(env, "Is TEST_NUM Defined (precondition: TEST_NUM is greater than 2 and 6)", expect_to_pass,
    precondition_list(test_num_gt_2, test_num_gt_6))
  };

  std::shared_ptr<Test> F{
    new Test(env, "Is TEST_NUM_2 Defined (precondition: TEST_STRING is greater than 3 and 15)", expect_to_fail,
    precondition_list(test_str_gt_3, test_str_gt_15))
  };


  TestList tests{A, B, C};
  tests.enqueue(D, E, F);

  run_and_print(tests);

  env.set("TEST_NUM", 100);
  env.set("TEST_NUM_2", "Hello");
  env.set("TEST_STRING", "30");


  //std::cout << "Value of TEST_NUM = " << get_env_str(env.get("TEST_NUM")) << std::endl;

  run_and_print(tests);



  return 0;
}

