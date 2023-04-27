#include <string>

template <typename Tag>
struct NewType {
    explicit NewType(typename Tag::Type value): value(value) {}
    typename Tag::Type value;
};

struct TimestampTag {
    using Type = int64_t;
};
using Timestamp = NewType<TimestampTag>;

struct UserIdTag {
    using Type = std::string;
};
using UserId = NewType<UserIdTag>;

struct SKUTag {
    using Type = std::string;
};
using SKU = NewType<SKUTag>;

struct Sale {
    UserId customer;
    SKU item;
    Timestamp date;
};

using TimestampAlias = int64_t;

void ExpectAlias(TimestampAlias) {}

void ExpectNewType(Timestamp) {}

int main() {

    TimestampAlias ts1 = 1000;
    Timestamp ts2 = Timestamp(1000);

    ExpectAlias(ts1); // <- OK
    ExpectAlias(1000); // <- OK
    ExpectNewType(ts2); // <- OK
    ExpectNewType(1000); // <- Ошибка компиляции

    return 0;
}