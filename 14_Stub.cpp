/// 14_Stub.cpp
#include <iostream>
#include <string>

class Time {
public:
    virtual ~Time() { }

    virtual std::string GetCurrentTime() const = 0;
};

class Clock : public Time {
public:
    std::string GetCurrentTime() const override
    {
        time_t rawTime;
        tm* timeInfo;
        char buffer[128];

        time(&rawTime);
        timeInfo = localtime(&rawTime);

        strftime(buffer, sizeof(buffer), "%H:%M", timeInfo);

        return std::string { buffer };
    }
};

class Scheduler {
    Time* time;

public:
    Scheduler(Time* p)
        : time { p }
    {
    }

    int Alarm()
    {
        std::string current = time->GetCurrentTime();
        if (current == "00:00") {
            return 42;
        } else if (current == "10:00") {
            return 100;
        }

        return 0;
    }
};

#include <gtest/gtest.h>

// Test Stub Pattern
// 의도: 다른 컴포넌트로부터의 간접 입력에 의존하는 로직을 독립적으로 검증하고 싶습니다.
// 방법: 실제 의존하는 객체를 테스트 대역으로 교체해서,
//      SUT가 테스트하는데 필요한 결과를 보내도록 '제어' 합니다.
//  "특수한 상황을 시뮬레이션 하는 목적으로 사용됩니다."

class StubTime : public Time {
    std::string result;

public:
    StubTime(const std::string& r)
        : result { r }
    {
    }

    std::string GetCurrentTime() const override
    {
        return result;
    }
};

TEST(SchedulerTest, Alarm_00_00)
{
    StubTime clock { "00:00" };
    Scheduler sc { &clock };

    EXPECT_EQ(sc.Alarm(), 42) << "00:00 일 때";
}

TEST(SchedulerTest, Alarm_10_00)
{
    StubTime clock { "10:00" };
    Scheduler sc { &clock };

    EXPECT_EQ(sc.Alarm(), 100) << "10:00 일 때";
}

// * 의존성 주입
//  : 제품 코드를 사용하듯이, 테스트 대역을 적용할 수 있습니다.
#if 0
int main()
{
    // std::cout << clock.GetCurrentTime() << std::endl;

    Clock clock;
    Scheduler sc { &clock };

    std::cout << sc.Alarm() << std::endl;
}
#endif

// 위의 예제를 Google Mock을 통해 구현해보세요.
// - ON_CALL
// - NiceMock

#include <gmock/gmock.h>

using testing::NiceMock;
using testing::Return;

class MockTime : public Time {
public:
    // std::string GetCurrentTime() const override
    MOCK_METHOD(std::string, GetCurrentTime, (), (const, override));
};

TEST(SchedulerTestGoogleMock, Alarm_00_00)
{
    NiceMock<MockTime> clock;
    ON_CALL(clock, GetCurrentTime).WillByDefault(Return("00:00"));
    Scheduler sc { &clock };

    EXPECT_EQ(sc.Alarm(), 42) << "00:00 일 때";
}

TEST(SchedulerTestGoogleMock, Alarm_10_00)
{
    NiceMock<MockTime> clock;
    ON_CALL(clock, GetCurrentTime).WillByDefault(Return("10:00"));
    Scheduler sc { &clock };

    EXPECT_EQ(sc.Alarm(), 100) << "10:00 일 때";
}
