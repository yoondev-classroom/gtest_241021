// 17_Mock.cpp
#include <string>
#include <vector>

enum Level {
    INFO,
    WARN,
    ERROR,
};

class DLoggerTarget {
public:
    virtual ~DLoggerTarget() { }

    virtual void Write(Level level, const std::string& message) = 0;
};

// 파일에 로그를 기록합니다.
class FileTarget : public DLoggerTarget { };

// 로그를 네트워크로 전송합니다.
class NetworkTarget : public DLoggerTarget { };

class DLogger {
    std::vector<DLoggerTarget*> targets;

public:
    void AddTarget(DLoggerTarget* p) { targets.push_back(p); }

    void Write(Level level, const std::string& message)
    {
        for (auto e : targets) {
            e->Write(level, message);
        }
    }
};

// * 문제점
// : DLogger의 Write를 수행하였을 때,
//   검증할 수 있는 상태가 존재하지 않습니다.

// * Mock Object Pattern
// 의도: SUT의 함수를 호출하였을 때, 발생하는 부수 효과를 관찰할 수 없어서
//     테스트되지 않은 요구사항이 존재합니다.
// 방법: "행위 기반 검증"을 수행합니다.

// - 상태 검증
//  : SUT에 작용을 가한후, 내부 상태 변화를 단언문을 통해 확인합니다.
// - 동작 검증
//  : SUT에 작용을 가한후, 협력 객체를 대상으로 내부적으로 발생하는 함수의 호출 여부, 호출 인자, 호출 횟수,
//    호출 순서 등의 정보를 통해 정상 동작 여부를 판단합니다.
//  => Mock Framework를 통해서, 모의 객체를 쉽게 만들 수 있습니다.

// C++ Mock Framework
//  => Google Mock Framework 입니다.
//  => Google Test 1.8 이후로, Google Mock 프로젝트가 흡수되었습니다.

#include <gmock/gmock.h>
// 위의 헤더를 통해 gtest/gtest.h도 포함되기 때문에, 별도로 포함할 필요가 없습니다.

class MockDLoggerTarget : public DLoggerTarget {
public:
    // virtual void Write(Level level, const std::string& message)
    // Mocking => MOCK_METHOD 매크로를 이용하면 됩니다.

    // * 1.10 이전
    // MOCK_METHOD{인자개수}(메소드 이름, 메소드 타입)
    // MOCK_CONST_METHOD{인자개수}
    // MOCK_METHOD2(Write, void(Level level, const std::string& message));

    // * 1.10 이후
    // => Mocking 방법이 변경되었습니다.
    //  : MOCK_METHOD 하나로 통일되었습니다.
    // MOCK_METHOD(반환타입, 메소드이름, (인자정보), (한정자정보));

    // void Write(Level level, const std::string& message) override
    MOCK_METHOD(void, Write, (Level level, const std::string& message), (override));
};

TEST(DLoggerTest, Write)
{
    // Arrange
    DLogger logger;
    MockDLoggerTarget t1, t2;
    logger.AddTarget(&t1);
    logger.AddTarget(&t2);

    // 주의사항: 동작을 수행하기 전에, 먼저 EXPECT_CALL을 해야 합니다.
    // Assert
    EXPECT_CALL(t1, Write(INFO, "test_message"));
    EXPECT_CALL(t2, Write(INFO, "test_message"));

    // Act
    logger.Write(INFO, "test_message");
}

// * 테스트 대역(Test Double)
// => xUnit Test Pattern
// => xUnit Test Pattern 이전에는 테스트 대역을 Mock 이라고 불렀습니다.
//  "Google Mock은 테스트 대역 프레임워크입니다."
//  - 행위 기반 검증 뿐 아니라, 다양한 기능을 추가적으로 제공하고 있습니다.
//    Stub / Fake

// 1) Test Stub
// => SUT가 테스트하는데 필요한 결과를 테스트 대역을 통해 제어합니다.
//   "시간/파일시스템" - 특수한 상황을 시뮬레이션하는 목적

// 2) Fake Object
// => 협력 객체가 준비되지 않았거나, 사용하기 어렵거나, 너무 느려서 테스트되지 않은 요구사항이 존재합니다.
//   같은 역활을 수행하는 가벼운 테스트 대역을 통해서 SUT를 검증합니다.
//  "데이터베이스"

// 3) Test Spy
// => 관찰할 수 있는 SUT의 상태가 존재하지 않습니다.
//    목격한 일을 기록해두었다가, 테스트에서 확인할 수 있는 테스트 대역을 만듭니다.

// 4) Mock Object
// => 관찰할 수 있는 SUT의 상태가 존재하지 않습니다.
//    협력 객체를 대상으로 호출되는 메소드의 호출 여부/횟수/순서/인자 등을 통해 정상 동작 여부를 검증합니다.
//   "행위 기반 검증 / 동작 검증"

// ------
// TDD(Test Driven Development) - Kent Back
// BDD(Behavior Driven Development, 행위 주도 개발)
//  * 가독성(자연어)
//    Given/When/Then
//  * 행위 기반 검증
