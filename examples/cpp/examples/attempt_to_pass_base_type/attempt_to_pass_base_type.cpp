#include <string>

template <typename Tag>
struct NewType {
    using Raw = typename Tag::Type;
    explicit NewType(Raw const& value): value_(value) {}
    explicit NewType(Raw && value): value_(std::move(value)) {}
    const Raw& value() {
        return value_;
    }
private:
    Raw value_;
};

struct TimestampTag {
    using Type = int64_t;
};
using Timestamp = NewType<TimestampTag>;

using TimestampAlias = int64_t;

void ExpectAlias(TimestampAlias) {}

void ExpectNewType(Timestamp) {}

int main() {

    TimestampAlias ts1 = 1000;
    Timestamp ts2 = Timestamp(1000);

    ExpectAlias(ts1); // <- OK
    ExpectAlias(1000); // <- OK
    ExpectNewType(ts2); // <- OK
    //ExpectNewType(1000); // <- Ошибка компиляции

    return 0;
}