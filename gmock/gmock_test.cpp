#include <memory>

#include "thirdparty/gmock/gmock.h"

class FancyObject {
 public:
	virtual std::unique_ptr<int> f() { return std::make_unique<int>(); }
	virtual ~FancyObject() = default;
};

class MockFancyObject : public FancyObject {
 public:
	MOCK_METHOD0(f, std::unique_ptr<int>());
};

TEST(GMock, GMockTest) {
	MockFancyObject mfo;

	EXPECT_CALL(mfo, f).WillRepeatedly(testing::Invoke(&std::make_unique<int>));
}