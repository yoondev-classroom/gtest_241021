// 19_Mock종류.cpp
#include <string>

class Car {
public:
    virtual ~Car() { }

    virtual void Go() { }
    virtual std::string GetName() const { return "Avante"; }
};

void Process(Car* p)
{
    // ...
    std::string name = p->GetName();
    // ...
    p->Go();
}

#include <gmock/gmock.h>

/*
GMOCK WARNING:
Uninteresting mock function call - returning default value.
    Function call: GetName()
          Returns: ""
*/
// 원인: MOCK_METHOD => 행위 기반 검증을 수행하겠다는 의도가 있습니다.
//     MOCK_METHOD 한 메소드에 대해서, EXPECT_CALL을 작성하지 않았는데,
//     호출이 테스트에서 수행되었을 때, 경고가 발생합니다.

// 1) NaggyMock(기본)
// => EXPECT_CALL 되지 않은 메소드가 호출되면, 경고가 발생합니다.
//    테스트의 결과는 변하지 않습니다.

// 2) NiceMock
// => 경고도 발생하지 않습니다.

// 3) StrictMock
// => EXPECT_CALL 되지 않은 메소드가 호출되면, 테스트는 실패합니다.
// => 주의해야 합니다.
//    : 테스트의 통과 기준이 높아져서, 테스트를 작성하는 비용이 증가할 수 있습니다.

class MockCar : public Car {
public:
    MOCK_METHOD(void, Go, (), (override));
    MOCK_METHOD(std::string, GetName, (), (const, override));
};

using testing::NiceMock;
using testing::StrictMock;

// Process에 Car 객체를 전달하였을 때, Go가 호출되는지 여부를 검증하고 싶습니다.
TEST(CarTest, Process)
{
    // MockCar mock;
    // NiceMock<MockCar> mock;
    StrictMock<MockCar> mock;

    EXPECT_CALL(mock, Go());
    // EXPECT_CALL(mock, GetName());

    Process(&mock);
}
