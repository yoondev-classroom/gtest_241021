// 11_파라미터화테스트3.cpp

bool IsPrime(int value)
{
    return false;

    for (int i = 2; i < value; ++i) {
        if (value % i == 0) {
            return false;
        }
    }

    return true;
}

#include <gtest/gtest.h>

// 입력 데이터가 여러개인 경우
// 1) tuple
#if 0
using InputType = std::tuple<int, int>;

class PrimeTest : public testing::TestWithParam<InputType> { };

INSTANTIATE_TEST_SUITE_P(PrimeValues, PrimeTest,
    testing::Values(
        InputType { 2, 4 },
        InputType { 3, 6 },
        InputType { 5, 8 },
        InputType { 7, 10 },
        InputType { 11, 12 },
        InputType { 13, 14 },
        InputType { 17, 18 }));

TEST_P(PrimeTest, IsPrime_Good)
{
    const InputType& data = GetParam();
    int good = std::get<0>(data);

    EXPECT_TRUE(IsPrime(good));
}

TEST_P(PrimeTest, IsPrime_Bad)
{
    const InputType& data = GetParam();
    int bad = std::get<1>(data);

    EXPECT_FALSE(IsPrime(bad));
}
#endif

// 2) 사용자 정의 타입 - struct

#if 1
struct InputType {
    int good;
    int bad;
};

// => 구글 테스트에서 사용자 정의 타입이 표현될 때,
//    원하는 형태로 표현될 수 있도록 연산자 재정의 함수가 필요합니다.
std::ostream& operator<<(std::ostream& os, const InputType& data)
{
    return os << "{" << data.good << ", " << data.bad << "}";
}

class PrimeTest : public testing::TestWithParam<InputType> { };

INSTANTIATE_TEST_SUITE_P(PrimeValues, PrimeTest,
    testing::Values(
        InputType { 2, 4 },
        InputType { 3, 6 },
        InputType { 5, 8 },
        InputType { 7, 10 },
        InputType { 11, 12 },
        InputType { 13, 14 },
        InputType { 17, 18 }));

TEST_P(PrimeTest, IsPrime_Good)
{
    const InputType& data = GetParam();

    EXPECT_TRUE(IsPrime(data.good));
}

TEST_P(PrimeTest, IsPrime_Bad)
{
    const InputType& data = GetParam();

    EXPECT_FALSE(IsPrime(data.bad));
}
#endif
