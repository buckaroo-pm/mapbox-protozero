
#include <test.hpp>

TEST_CASE("read repeated packed float field") {

    // Run these tests twice, the second time we basically move the data
    // one byte down in the buffer. It doesn't matter how the data or buffer
    // is aligned before that, in at least one of these cases the floats will
    // not be aligned properly. So we test that even in that case the floats
    // will be extracted properly.

    for (std::string::size_type n = 0; n < 2; ++n) {

        std::string abuffer;
        abuffer.reserve(1000);
        abuffer.append(n, '\0');

        SECTION("empty") {
            std::string buffer = load_data("repeated_packed_float/data-empty");

            abuffer.append(buffer);
            protozero::pbf_reader item(abuffer.data() + n, abuffer.size() - n);

            REQUIRE(!item.next());
        }

        SECTION("one") {
            std::string buffer = load_data("repeated_packed_float/data-one");

            abuffer.append(buffer);
            protozero::pbf_reader item(abuffer.data() + n, abuffer.size() - n);

            REQUIRE(item.next());
            auto it_pair = item.get_packed_float();
            REQUIRE(!item.next());

            REQUIRE(*it_pair.first == 17.34f);
            REQUIRE(++it_pair.first == it_pair.second);
        }

        SECTION("many") {
            std::string buffer = load_data("repeated_packed_float/data-many");

            abuffer.append(buffer);
            protozero::pbf_reader item(abuffer.data() + n, abuffer.size() - n);

            REQUIRE(item.next());
            auto it_pair = item.get_packed_float();
            REQUIRE(!item.next());

            auto it = it_pair.first;
            REQUIRE(*it++ == 17.34f);
            REQUIRE(*it++ ==   0.0f);
            REQUIRE(*it++ ==   1.0f);
            REQUIRE(*it++ == std::numeric_limits<float>::min());
            REQUIRE(*it++ == std::numeric_limits<float>::max());
            REQUIRE(it == it_pair.second);
        }

        SECTION("end_of_buffer") {
            std::string buffer = load_data("repeated_packed_float/data-many");

            abuffer.append(buffer);

            for (std::string::size_type i = 1; i < buffer.size() - n; ++i) {
                protozero::pbf_reader item(buffer.data() + n, i);
                REQUIRE(item.next());
                REQUIRE_THROWS_AS(item.get_packed_float(), protozero::end_of_buffer_exception);
            }
        }

    }

}

TEST_CASE("write repeated packed float field") {

    std::string buffer;
    protozero::pbf_writer pw(buffer);

    SECTION("empty") {
        float data[] = { 17.34f };
        pw.add_packed_float(1, std::begin(data), std::begin(data) /* !!!! */);

        REQUIRE(buffer == load_data("repeated_packed_float/data-empty"));
    }

    SECTION("one") {
        float data[] = { 17.34f };
        pw.add_packed_float(1, std::begin(data), std::end(data));

        REQUIRE(buffer == load_data("repeated_packed_float/data-one"));
    }

    SECTION("many") {
        float data[] = { 17.34f, 0.0f, 1.0f, std::numeric_limits<float>::min(), std::numeric_limits<float>::max() };
        pw.add_packed_float(1, std::begin(data), std::end(data));

        REQUIRE(buffer == load_data("repeated_packed_float/data-many"));
    }

}

