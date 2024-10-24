// 15_Fake.cpp
#include <string>

class User {
    std::string name;
    int age;

public:
    User(const std::string& s, int n)
        : name { s }
        , age { n }
    {
    }

    std::string GetName() const { return name; }
    int GetAge() const { return age; }
};

class IDatabase {
public:
    virtual ~IDatabase() { }

    virtual void SaveUser(const std::string& name, User* user) = 0;
    virtual User* LoadUser(const std::string& name) = 0;
};

class Repository {
    IDatabase* database;

public:
    Repository(IDatabase* p)
        : database { p }
    {
    }

    void Save(User* user)
    {
        // ...
        database->SaveUser(user->GetName(), user);
        // ...
    }

    User* Load(const std::string& name)
    {
        // ...
        return database->LoadUser(name);
    }
};

#include <gtest/gtest.h>
#include <map>

// Fake Object Pattern
// 의도: 아직 준비되지 않은 협력 객체로 인해서, 테스트 되지 않은 요구사항이 존재합니다.
// 방법: 동일한 기능을 제공하는 가벼운 테스트 대역을 만들어서, 테스트 되지 않은 요구사항을 검증합니다.
//   1) 협력 객체가 준비되지 않았을 때
//   2) 협력 객체가 사용하기 어려울 때
//   3) 협력 객체가 너무 느려서, 느린 테스트의 문제가 발생할 때
//  * Fake Object를 만드는 비용을 고려해야 합니다.

// * 사용자 정의 타입에 대해서, 단언문을 사용할 경우, 단언문이 사용하는 연산자에 대한 재정의가 반드시 필요합니다.
bool operator==(const User& lhs, const User& rhs)
{
    // return false;
    return lhs.GetName() == rhs.GetName() && lhs.GetAge() == rhs.GetAge();
}

// * 사용자 정의 타입에 대해서, 구글 테스트에서 제대로 출력되기 위해서는
//   연산자 재정의 함수가 필요합니다.
std::ostream& operator<<(std::ostream& os, const User& user)
{
    return os << "{ " << user.GetName() << ", " << user.GetAge() << "}";
}

class FakeDatabase : public IDatabase {
    std::map<std::string, User*> data;

public:
    void SaveUser(const std::string& name, User* user) override
    {
        data[name] = user;
    }

    User* LoadUser(const std::string& name) override
    {
        return data[name];
        // return nullptr;
    }
};

TEST(RepositoryTest, Save)
{
    FakeDatabase database;
    Repository repo { &database };
    std::string test_name = "test_name";
    int test_age = 42;
    User expected { test_name, test_age };

    repo.Save(&expected);
    User* actual = repo.Load(test_name);

    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(expected, *actual); // ==
}

#include <gmock/gmock.h>

#if 0
using testing::NiceMock;

class MockDatabase : public IDatabase {
public:
    // void SaveUser(const std::string& name, User* user) override
    MOCK_METHOD(void, SaveUser, (const std::string& name, User* user), (override));

    // User* LoadUser(const std::string& name) override
    MOCK_METHOD(User*, LoadUser, (const std::string& name), (override));
};

TEST(RepositoryTestGoogleMock, Save)
{
    NiceMock<MockDatabase> database;
    std::map<std::string, User*> data;
    ON_CALL(database, SaveUser).WillByDefault([&data](const std::string& name, User* user) {
        data[name] = user;
    });
    ON_CALL(database, LoadUser).WillByDefault([&data](const std::string& name) {
        return data[name];
    });

    Repository repo { &database };
    std::string test_name = "test_name";
    int test_age = 42;
    User expected { test_name, test_age };

    repo.Save(&expected);
    User* actual = repo.Load(test_name);

    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(expected, *actual); // ==
}
#endif

using testing::NiceMock;

class MockDatabase : public IDatabase {
    std::map<std::string, User*> data;

public:
    MockDatabase()
    {
        ON_CALL(*this, SaveUser).WillByDefault([this](const std::string& name, User* user) {
            data[name] = user;
        });
        ON_CALL(*this, LoadUser).WillByDefault([this](const std::string& name) {
            return data[name];
        });
    }

    MOCK_METHOD(void, SaveUser, (const std::string& name, User* user), (override));

    MOCK_METHOD(User*, LoadUser, (const std::string& name), (override));
};

TEST(RepositoryTestGoogleMock, Save)
{
    NiceMock<MockDatabase> database;
    std::map<std::string, User*> data;
    Repository repo { &database };
    std::string test_name = "test_name";
    int test_age = 42;
    User expected { test_name, test_age };

    repo.Save(&expected);
    User* actual = repo.Load(test_name);

    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(expected, *actual); // ==
}
