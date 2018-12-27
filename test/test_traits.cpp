#include <vector>
#include <deque>
#include <list>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

#include <catch.hpp>
#include <typed_argparser.hpp>

using namespace typed_argparser;

TEST_CASE( "type_tag_of", "[traits]" ) {
    static_assert(type_tag_of<bool>::value == TypeTag::Bool, "");
    static_assert(type_tag_of<int>::value == TypeTag::Int, "");
    static_assert(type_tag_of<float>::value == TypeTag::Float, "");
    static_assert(type_tag_of<std::string>::value == TypeTag::String, "");

    static_assert(type_tag_of<std::vector<bool>>::value == TypeTag::BoolVector, "");
    static_assert(type_tag_of<std::vector<int>>::value == TypeTag::IntVector, "");
    static_assert(type_tag_of<std::vector<float>>::value == TypeTag::FloatVector, "");
    static_assert(type_tag_of<std::vector<std::string>>::value == TypeTag::StringVector, "");
}

TEST_CASE( "is_container", "[traits]" ) {
    static_assert(is_container<std::vector<int>>::value, "");
    static_assert(is_container<std::deque<int>>::value, "");
    static_assert(is_container<std::list<int>>::value, "");
    static_assert(is_container<std::array<int, 3>>::value, "");
    static_assert(is_container<std::unordered_set<int>>::value, "");
    // static_assert(is_container<std::unordered_set<int, int>>::value, "");

    static_assert(!is_container<int>::value, "");
    static_assert(!is_container<std::string>::value, "");
}
