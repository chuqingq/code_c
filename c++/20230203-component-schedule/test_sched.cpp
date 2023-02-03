#include "common.h"
#include "component.h"

using std::abs;
using std::accumulate;
using std::back_inserter;
using std::condition_variable;
using std::count_if;
using std::cout;
using std::endl;
using std::for_each;
using std::getline;
using std::ifstream;
using std::istringstream;
using std::make_shared;
using std::max_element;
using std::min_element;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::transform;
using std::unique_lock;
using std::vector;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::this_thread::sleep_for;
using std::this_thread::sleep_until;
using namespace std::literals;

// #define TEST_CASE_FILE "/tests/scripts/task_sched.testcase"
#define TEST_CASE_FILE "/task_sched.testcase"
#define ONE_MS_IN_MICROSENCONDS 1000
#define MIN_JITTER 0.5

#define BUILD_MAX_MIN_AVG(max, min, avg, max_it, min_it, rawdata) \
  auto max_it = max_element(rawdata.begin(), rawdata.end());      \
  if (max_it != rawdata.end()) {                                  \
    max = *max_it;                                                \
  }                                                               \
  auto min_it = min_element(rawdata.begin(), rawdata.end());      \
  if (min_it != rawdata.end()) {                                  \
    min = *min_it;                                                \
  }                                                               \
  avg = accumulate(rawdata.begin(), rawdata.end(), 0.0) / rawdata.size()

namespace {
auto const kTestPeriodMicroSec = 1000;  // unit of microseconds
auto const KTestCounts = 1000;
}  // namespace

class TaskSchedTest : public vs::rte::Component {
 public:
  struct TestCase {
    TestCase(uint32_t duration_microsec, uint32_t times)
        : duration_microsec(duration_microsec), times(times), test_results({}) {
      id = make_shared<int>(-1);
    };
    shared_ptr<int> id;
    uint32_t duration_microsec;
    uint32_t times;
    vector<float> test_results;
  };

  struct TestSummary {
    TestSummary(const TestCase& test_case) : test_case(test_case){};
    void print() {
      cout << "============== TestSummary Of Task Scheduler=============="
           << endl;
      cout << "  " << endl;
      cout << "任务调度目标时延:  " << test_case.duration_microsec / 1000
           << "毫秒" << endl;
      cout << "测试次数:   " << test_case.times << "次" << endl;
      cout << "最大时延:                    " << max_duration << "毫秒" << endl;
      cout << "时延误差大于0.5毫秒百分比:     " << jitter_500mcrosec_percent
           << "%" << endl;
    };

    TestCase test_case;
    float max_duration;
    float min_duration;
    float avg_duration;
    float max_jitter;
    float min_jitter;
    float avg_jitter;
    float jitter_500mcrosec_percent;
  };

 public:
  TaskSchedTest(const std::string& name);
  int OnCreate();
  int OnDestroy();

 private:
  vector<TestCase> BuildCase();
  void SerializeExcuteCases(vector<TestCase>& cases);
  int ExecuteCase(TestCase& test_case);
  TestSummary BuildSummary(const TestCase& test_case);

 private:
  mutex mx_task_execute_;
  condition_variable cv_task_execute_;
  bool task_done_{false};
};

TaskSchedTest::TaskSchedTest(const std::string& name) : Component(name) {}

int TaskSchedTest::OnCreate() {
  cout << "======= TaskSchedTest Begins ==========" << endl;
  sleep_for(1000ms);

  auto test_case_group = BuildCase();
  SerializeExcuteCases(test_case_group);

  return 0;
}

int TaskSchedTest::OnDestroy() {
  cout << "======= TaskShedTest Ends    ==========" << endl;
  sleep_for(1000ms);
  return 0;
}

vector<TaskSchedTest::TestCase> TaskSchedTest::BuildCase() {
  vector<TaskSchedTest::TestCase> result{};
  char* buffer;
  if ((buffer = getcwd(nullptr, 0)) == nullptr) {
    cout << " get cwd error " << endl;
  } else {
    string cur_dir(buffer);
    free(buffer);
    cout << " cur dir " << cur_dir << endl;
    string case_file = cur_dir + TEST_CASE_FILE;
    auto input = ifstream(case_file);
    if (input) {
      string line;
      uint32_t duration_microsec;
      uint32_t times;
      getline(input, line);  // read title
      while (getline(input, line)) {
        istringstream record(line);
        record >> duration_microsec >> times;
        result.emplace_back(duration_microsec, times);
      }
    } else {
      cout << "====" << case_file << " does not exists " << endl;
    }
  }

#if 0
  for_each(result.cbegin(), result.cend(), [](const auto& test_case) {
    cout << " duration_microsec: " << test_case.duration_microsec << endl;
    cout << " times: " << test_case.times << endl;
  });
#endif

  cout << "=========== endof TaskSchedTest::BuildCase" << endl;
  return result;
}

void TaskSchedTest::SerializeExcuteCases(vector<TestCase>& cases) {
  decltype(cases.size()) count = 1;
  for_each(cases.begin(), cases.end(),
           [task_ched_test = this, count,
            size = cases.size()](auto& test_case) mutable {
             cout << "+++++++ executing test_case " << count << " ....... "
                  << endl;

             task_ched_test->task_done_ = false;
             task_ched_test->ExecuteCase(test_case);
             //  unique_lock<mutex> lk(task_ched_test->mx_task_execute_);
             //  task_ched_test->cv_task_execute_.wait(
             //      lk, [task_ched_test]() { return task_ched_test->task_done_;
             //      });

             //  cout << "+++++++ test_case " << count << " done +++++++ " <<
             //  endl;

             ++count;
           });
}

int TaskSchedTest::ExecuteCase(TestCase& test_case) {
  decltype(test_case.times) count = 0;
  auto ret = AddTimer(
      test_case.duration_microsec, false,
      [test_case, count, task_ched_test = this, cur_time = steady_clock::now()](
          int timer_id, void* priv) mutable {
        auto end_time = steady_clock::now();

        auto dur_ms =
            (static_cast<float>(
                (duration_cast<microseconds>(end_time - cur_time).count()))) /
            ONE_MS_IN_MICROSENCONDS;
        test_case.test_results.push_back(dur_ms);
        cur_time = end_time;

        ++count;
        if (count == test_case.times) {
          task_ched_test->BuildSummary(test_case);
          task_ched_test->DelTimer(timer_id);
          //   {
          //     unique_lock<mutex> lk(task_ched_test->mx_task_execute_);
          //     task_ched_test->task_done_ = true;
          //   }
          //   task_ched_test->cv_task_execute_.notify_one();
          return;
        }
      },
      nullptr, test_case.id.get());

  if (ret != 0) {
    cout << " ======== AddTimer failed ======== " << endl;
  }

  return ret;
}

TaskSchedTest::TestSummary TaskSchedTest::BuildSummary(
    const TestCase& test_case) {
  const auto& raw_durations = test_case.test_results;

  vector<decltype(TestSummary::min_jitter)> raw_jitters;
  transform(raw_durations.begin(), raw_durations.end(),
            back_inserter(raw_jitters),
            [targ_dur = (static_cast<float>(test_case.duration_microsec)) /
                        ONE_MS_IN_MICROSENCONDS](auto dur) {
              return abs(dur - targ_dur);
            });

  TestSummary test_summary(test_case);

  BUILD_MAX_MIN_AVG(test_summary.max_duration, test_summary.min_duration,
                    test_summary.avg_duration, max_dur, min_dur, raw_durations);
  BUILD_MAX_MIN_AVG(test_summary.max_jitter, test_summary.min_jitter,
                    test_summary.avg_jitter, max_jitter, min_jitter,
                    raw_jitters);
  auto unexpected_jitter_counts =
      count_if(raw_jitters.begin(), raw_jitters.end(),
               [](const auto jitter) { return jitter > MIN_JITTER; });
  cout << "  unexpected_jitter_counts: " << unexpected_jitter_counts << endl;
  test_summary.jitter_500mcrosec_percent =
      (static_cast<float>(unexpected_jitter_counts)) / test_case.times * 100;
  test_summary.print();

#if 0
  for_each(raw_durations.begin(), raw_durations.end(), [](auto&& duration) {
    cout << " duration -> " << duration << " milliseconds " << endl;
    sleep_for(500ms);
  });

  cout << "===============================" << endl;
  for_each(raw_jitters.begin(), raw_jitters.end(), [](auto&& jitter) {
    cout << " jitter -> " << jitter << " milliseconds " << endl;
    sleep_for(500ms);
  });
#endif

  return test_summary;
}

// REGISTER_COMPONENT(TaskSchedTest, component_task_sched_test)

int main() {
  auto loop = uv_default_loop();
  TaskSchedTest component_task_sched_test("component_task_sched_test");
  component_task_sched_test.OnCreate();
  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}

// duration_microsec test_counts
// 1000              100
// 1000              1000
// 1000              5000
// 1000              10000
