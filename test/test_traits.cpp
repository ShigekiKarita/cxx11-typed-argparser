#include <vector>
#include <deque>
#include <list>
#include <forward_list>
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

    static_assert(type_tag_of<std::deque<bool>>::value == TypeTag::BoolVector, "");
    static_assert(type_tag_of<std::deque<int>>::value == TypeTag::IntVector, "");
    static_assert(type_tag_of<std::deque<float>>::value == TypeTag::FloatVector, "");
    static_assert(type_tag_of<std::deque<std::string>>::value == TypeTag::StringVector, "");

    static_assert(type_tag_of<std::list<bool>>::value == TypeTag::BoolVector, "");
    static_assert(type_tag_of<std::list<int>>::value == TypeTag::IntVector, "");
    static_assert(type_tag_of<std::list<float>>::value == TypeTag::FloatVector, "");
    static_assert(type_tag_of<std::list<std::string>>::value == TypeTag::StringVector, "");

    static_assert(type_tag_of<std::array<bool, 1>>::value == TypeTag::BoolVector, "");
    static_assert(type_tag_of<std::array<int, 1>>::value == TypeTag::IntVector, "");
    static_assert(type_tag_of<std::array<float, 1>>::value == TypeTag::FloatVector, "");
    static_assert(type_tag_of<std::array<std::string, 1>>::value == TypeTag::StringVector, "");

    static_assert(type_tag_of<std::unordered_set<bool>>::value == TypeTag::BoolVector, "");
    static_assert(type_tag_of<std::unordered_set<int>>::value == TypeTag::IntVector, "");
    static_assert(type_tag_of<std::unordered_set<float>>::value == TypeTag::FloatVector, "");
    static_assert(type_tag_of<std::unordered_set<std::string>>::value == TypeTag::StringVector, "");

    // not supported now
    // static_assert(type_tag_of<std::forward_list<bool>>::value == TypeTag::BoolVector, "");
    // static_assert(type_tag_of<std::forward_list<int>>::value == TypeTag::IntVector, "");
    // static_assert(type_tag_of<std::forward_list<float>>::value == TypeTag::FloatVector, "");
    // static_assert(type_tag_of<std::forward_list<std::string>>::value == TypeTag::StringVector, "");
}

TEST_CASE( "is_container", "[traits]" ) {
    static_assert(is_container<std::vector<int>>::value, "");
    static_assert(is_container<std::deque<int>>::value, "");
    static_assert(is_container<std::list<int>>::value, "");
    static_assert(is_container<std::array<int, 3>>::value, "");
    static_assert(is_container<std::unordered_set<int>>::value, "");
    // TODO support this?
    // static_assert(is_container<std::unordered_set<int, int>>::value, "");

    static_assert(!is_container<int>::value, "");
    static_assert(!is_container<std::string>::value, "");
}
