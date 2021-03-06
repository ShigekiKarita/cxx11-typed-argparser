#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <exception>
#include <unordered_set>
#include <unordered_map>
#include <type_traits>

// TODO has_include
#include <cxxabi.h>

#if __has_include(<rapidjson/writer.h>)
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <fstream>
#if __cplusplus > 201703L
// C++ 17 deduction guides
namespace rapidjson {
    template<typename OutputStream>
    PrettyWriter(OutputStream&) -> PrettyWriter<OutputStream, UTF8<>, UTF8<>, CrtAllocator, kWriteDefaultFlags>;
    template<typename OutputStream, typename StackAllocator>
    PrettyWriter(OutputStream&, StackAllocator*) -> PrettyWriter<OutputStream, UTF8<>, UTF8<>, StackAllocator, kWriteDefaultFlags>;
    template<typename OutputStream, typename StackAllocator>
    PrettyWriter(OutputStream&, StackAllocator*, size_t) -> PrettyWriter<OutputStream, UTF8<>, UTF8<>, StackAllocator, kWriteDefaultFlags>;

    template<typename OutputStream>
    Writer(OutputStream&) -> Writer<OutputStream, UTF8<>, UTF8<>, CrtAllocator, kWriteDefaultFlags>;
    template<typename OutputStream, typename StackAllocator>
    Writer(OutputStream&, StackAllocator*) -> Writer<OutputStream, UTF8<>, UTF8<>, StackAllocator, kWriteDefaultFlags>;
    template<typename OutputStream, typename StackAllocator>
    Writer(OutputStream&, StackAllocator*, size_t) -> Writer<OutputStream, UTF8<>, UTF8<>, StackAllocator, kWriteDefaultFlags>;
}  // namespace rapidjson
#endif
#endif

namespace typed_argparser {

    template <typename T>
    struct Demangle {
        char* realname ;

        Demangle() {
            int status = 0 ;
            realname = abi::__cxa_demangle( typeid(T).name(), 0, 0, &status );
        }

        Demangle( Demangle const & ) = delete;
        Demangle & operator = ( Demangle const & ) = delete;

        ~Demangle() {
            std::free( realname );
        }

        operator char const*() const {
            return realname;
        }
    };

    template <>
    struct Demangle<std::string> {
        operator char const*() const {
            return "std::string";
        }
    };

    template <typename T, typename A>
    struct Demangle<std::vector<T, A>> {
        std::string ret;

        operator char const*() {
            ret = "std::vector<" + std::string(Demangle<T>()) + ">";
            return ret.c_str();
        }
    };

    class ArgParserError : public std::invalid_argument {
    public:
        using std::invalid_argument::invalid_argument;
    };


    enum class TypeTag {
        Unknown = 0,
        Bool,
        Int,
        Float,
        String,
        BoolVector,
        IntVector,
        FloatVector,
        StringVector
    };

    template<typename T, typename _ = void>
    struct is_container : std::false_type {};

    template<typename... Ts>
    struct is_container_helper {};

    template<typename T>
    struct is_container<
        T,
        typename std::conditional<
            false,
            is_container_helper<
                typename T::value_type,
                typename T::size_type,
                // typename T::allocator_type, // not defined in std::array
                typename T::iterator,
                typename T::const_iterator,
                decltype(std::declval<T>().size()),
                decltype(std::declval<T>().begin()),
                decltype(std::declval<T>().end()),
                decltype(std::declval<T>().cbegin()),
                decltype(std::declval<T>().cend()),
                typename std::enable_if<
                    !std::is_same<std::string, typename std::decay<T>::type>::value
                    >::type
                >,
            void
            >::type
        > : public std::true_type {};


    template <typename T>
    typename std::enable_if<is_container<T>::value, std::ostream&>::type
    operator << (std::ostream& ostr, const T& v) {
        if (v.empty()) {
            ostr << "{}";
            return ostr;
        }
        ostr << "{" << v.front();
        if (v.size() > 1) {
            for (auto iter = std::next(v.begin()); iter != v.end(); ++iter) {
                ostr << ", " << *iter;
            }
        }
        ostr << "}";
        return ostr;
    }

    template <typename T>
    struct vector_tag_of {
        static constexpr TypeTag value =
            std::is_same<typename std::decay<T>::type, bool>::value ? TypeTag::BoolVector :
            std::is_integral<T>::value ? TypeTag::IntVector :
            std::is_floating_point<T>::value ? TypeTag::FloatVector :
            std::is_same<typename std::decay<T>::type, std::string>::value ? TypeTag::StringVector :
            TypeTag::Unknown;
    };

    template <typename T>
    typename std::enable_if<is_container<T>::value, typename T::value_type>::type value_t();

    template <typename T>
    typename std::enable_if<!is_container<T>::value>::type value_t();

    template <typename T>
    using value_type = decltype(value_t<T>());

    template <typename T>
    struct type_tag_of  {
        static constexpr TypeTag value =
            std::is_same<typename std::decay<T>::type, bool>::value ? TypeTag::Bool :
            std::is_integral<T>::value ? TypeTag::Int :
            std::is_floating_point<T>::value ? TypeTag::Float :
            std::is_same<typename std::decay<T>::type, std::string>::value ? TypeTag::String :
            is_container<T>::value ?  vector_tag_of<value_type<T>>::value :
            TypeTag::Unknown;
    };

    struct ArgValue {
        std::string key;
        std::vector<std::string> value;
        TypeTag type = TypeTag::Unknown;
    };

    struct ArgParser {
        using Value = std::vector<std::string>;

        std::string help_start;
        std::string help_end;
        std::string help = "--help";
        bool help_wanted = false;
        std::unordered_set<std::string> added_keys;
        std::unordered_map<std::string, ArgValue> parsed_map;

        void clear() {
            this->help_wanted = false;
            this->added_keys.clear();
            this->parsed_map.clear();
        }

        ArgParser() {}

        ArgParser(int argc, const char* const argv[])
            : ArgParser(argc, argv, argv[0]) {}

        ArgParser(int argc, const char* const argv[],
                  const std::string& help_start, const std::string& help_end="")
            : help_start(help_start), help_end(help_end) {
            std::string key;
            std::vector<std::string> value;
            for (int i = 1; i < argc; ++i) {
                std::string s = argv[i];
                if (s.substr(0, 2) == "--") {
                    key = s;
                    value.clear();
                    value.shrink_to_fit();
                } else {
                    value.push_back(s);
                }
                ArgValue v;
                v.key = key;
                v.value = value;
                parsed_map[key] = v;
            }
            this->help_wanted = this->parsed_map.find(this->help) != this->parsed_map.end();
            this->_help_stream << this->help_start << std::endl;
        }

        static bool to_bool(const Value& src) {
            if (src.size() == 0) {
                return true;
            }
            const std::string& v = src[0];
            if (v != "true" && v != "false" && src.size() != 1) {
                std::stringstream ret;
                ret << "bool value should be \"true\" or \"false\" but passed: \"" << src[0] << "\"";
                throw ArgParserError(ret.str());
            }
            return v == "true";
        }

        TypeTag assign(const Value& src, bool& dst) const {
            dst = to_bool(src);
            return TypeTag::Bool;
        }

        TypeTag assign(const Value& src, std::vector<bool>::reference dst) const {
            dst = to_bool(src);
            return TypeTag::Bool;
        }

        TypeTag assign(const Value& src, std::string& dst) const {
            dst = src[0];
            return TypeTag::String;
        }

        template <typename T>
        typename std::enable_if<std::is_integral<T>::value, TypeTag>::type
        assign(const Value& src, T& dst) const {
            dst = std::stoi(src[0]);
            return TypeTag::Int;
        }

        template <typename T>
        typename std::enable_if<std::is_floating_point<T>::value, TypeTag>::type
        assign(const Value& src, T& dst) const {
            dst = std::stod(src[0]);
            return TypeTag::Float;
        }

        template <typename T>
        typename std::enable_if<is_container<T>::value, TypeTag>::type
        assign(const Value& src, T& dst) const {
            dst.clear();
            // TODO reserve if possible
            // val.reserve(v.size());
            for (size_t i = 0; i < src.size(); ++i) {
                value_type<T> d;
                assign({src[i]}, d);
                dst.push_back(d);
            }
            return type_tag_of<T>::value;
        }

        template <typename T, size_t N>
        TypeTag assign(const Value& src, std::array<T, N>& dst) const {
            if (src.size() != N) {
                std::stringstream ss;
                ss << "defined array length " << N << " != " << src.size();
                throw ArgParserError(ss.str());
            }
            for (size_t i = 0; i < src.size(); ++i) {
                assign({src[i]}, dst[i]);
            }
            return vector_tag_of<T>::value;
        }

        void set_default_value(const std::string& key, const bool& v) {
            Value val = {v ? "true" : "false"};
            ArgValue a;
            a.key = key;
            a.value = val;
            a.type = TypeTag::Bool;
            this->parsed_map[key] = a;
        }

        void set_default_value(const std::string& key, const std::string& v) {
            Value val = {v};
            ArgValue a;
            a.key = key;
            a.value = val;
            a.type = TypeTag::String;
            this->parsed_map[key] = a;
        }

        template <typename T>
        typename std::enable_if<!is_container<T>::value>::type
        set_default_value(const std::string& key, const T& v) {
            Value val = {std::to_string(v)};
            ArgValue a;
            a.key = key;
            a.value = val;
            a.type = type_tag_of<T>::value;
            this->parsed_map[key] = a;
        }

        template <typename T>
        typename std::enable_if<is_container<T>::value>::type
        set_default_value(const std::string& key, const T& v) {
            Value val;
            // TODO reserve if possible
            // val.reserve(v.size());
            for (const auto& u: v) {
                val.push_back(std::to_string(u));
            }
            ArgValue a;
            a.key = key;
            a.value = val;
            a.type = type_tag_of<T>::value;
            this->parsed_map[key] = a;
        }

        std::stringstream _help_stream;
        std::string _help_message;

        std::string help_message() {
            if (this->_help_message.empty()) {
                this->_help_stream << this->help_end;
                this->_help_message = this->_help_stream.str();
            }
            return this->_help_message;
        }

        template <typename T>
        void add(const std::string& key, T& value, const std::string& doc="", bool require=false) {
            if (this->help_wanted) {
                this->_help_stream << std::boolalpha
                                   << "  " << key << (require ? " (require)" : "") << std::endl;
                this->_help_stream << "    type: " << Demangle<T>()
                                   << ", default:" << value << std::endl;
                if (!doc.empty()) {
                    this->_help_stream << "    " << doc << std::endl;
                }
                this->_help_stream << std::endl;
                return;
            }
            if (key.substr(0, 2) != "--") {
                throw ArgParserError("key should start with \"--\" but \"" + key + "\".");
            }
            this->added_keys.insert(key);
            if (this->parsed_map.find(key) != this->parsed_map.end()) {
                try {
                    parsed_map[key].type = assign(parsed_map[key].value, value);
                    return;
                } catch (std::invalid_argument& e) {
                    throw ArgParserError(std::string(e.what()) + "\nthrown from key: " + key);
                }
            }
            else if (require) {
                std::string msg = "cmd arg: \"" + key + "\" is require but not found.";
                throw ArgParserError(msg);
            }
            else {
                this->set_default_value(key, value);
            }
        }

        template <typename T>
        void require(const std::string& key, T& value, const std::string& doc="") {
            add(key, value, doc, true);
        }

        /// throw if an invalid argument (key) are passed
        void check() {
            std::string invalid;
            for (const auto& kv : this->parsed_map) {
                if (this->added_keys.find(kv.first) == this->added_keys.end()) {
                    invalid += kv.first +  ", ";
                }
            }
            if (invalid.empty()) return;

            auto msg = "cmd args: [" + invalid.substr(0, invalid.size() - 2) + "] are found but not defined.";
            throw ArgParserError(msg);
        }

#if __has_include(<rapidjson/writer.h>)
        std::string value_to_string(const rapidjson::Value& v) {
            using namespace rapidjson;
            switch (v.GetType()) {
                case kNumberType:
                    return v.IsInt64()
                        ? std::to_string(v.GetInt64())
                        : std::to_string(v.GetDouble());
                case kStringType:
                    return v.GetString();
                case kTrueType:
                    return "true";
                case kFalseType:
                    return "false";
                default:
                    throw ArgParserError("array, null, and object are not expected here: " + std::to_string(v.GetType()));
            }
        }

        void from_json(std::string json, bool clear=true) {
            auto n = json.size();
            rapidjson::Document doc;
            // read from file
            if (json.substr(n - 5, n) == ".json") {
                std::ifstream file(json);
                if (!file.is_open()) {
                    throw ArgParserError("cannot open the file: " + json);
                }
                rapidjson::IStreamWrapper isw(file);
                doc.ParseStream(isw);
            }
            // read from string
            else {
                doc.Parse(json.c_str());
            }

            // set values
            if (clear) {
                this->clear();
            }
            for (const auto& m : doc.GetObject()) {
                auto key = m.name.GetString();
                ArgValue av;
                av.key = key;
                const auto& v = m.value;
                switch (v.GetType()) {
                case rapidjson::kArrayType:
                    for (const auto& m : v.GetArray()) {
                        av.value.push_back(value_to_string(m));
                    }
                    break;
                default:
                    av.value.push_back(value_to_string(v));
                }
                this->parsed_map[key] = av;
            }
        }

        std::string to_json(bool pretty=true, bool single_line_array=true) const {
            if (pretty) {
                rapidjson::PrettyWriter<rapidjson::StringBuffer> writer;
                writer.SetFormatOptions(single_line_array
                                        ? rapidjson::kFormatSingleLineArray
                                        : rapidjson::kFormatDefault);
                return to_json_impl(writer);
            } else {
                rapidjson::Writer<rapidjson::StringBuffer> writer;
                return to_json_impl(writer);
            }
        }

        template <typename Writer>
        std::string to_json_impl(Writer& writer) const {
            rapidjson::StringBuffer s;
            writer.Reset(s);
            writer.StartObject();
            for (const auto& kv : this->parsed_map) {
                writer.Key(kv.first.c_str());
                switch (kv.second.type) {
                case TypeTag::Int:
                {
                    std::int64_t i;
                    assign(kv.second.value, i);
                    writer.Int(i);
                    break;
                }
                case TypeTag::Float:
                {
                    double d;
                    assign(kv.second.value, d);
                    writer.Double(d);
                    break;
                }
                case TypeTag::Bool:
                {
                    bool b;
                    assign(kv.second.value, b);
                    writer.Bool(b);
                    break;
                }
                case TypeTag::IntVector:
                {
                    std::vector<std::int64_t> iv;
                    assign(kv.second.value, iv);
                    writer.StartArray();
                    for (auto i : iv) {
                        writer.Int(i);
                    }
                    writer.EndArray();
                    break;
                }
                case TypeTag::FloatVector:
                {
                    std::vector<double> iv;
                    assign(kv.second.value, iv);
                    writer.StartArray();
                    for (auto i : iv) {
                        writer.Double(i);
                    }
                    writer.EndArray();
                    break;
                }
                case TypeTag::StringVector:
                {
                    std::vector<std::string> iv;
                    assign(kv.second.value, iv);
                    writer.StartArray();
                    for (auto i : iv) {
                        writer.String(i.c_str());
                    }
                    writer.EndArray();
                    break;
                }
                default:
                    writer.String(kv.second.value[0].c_str());
                }
            }
            writer.EndObject();
            return s.GetString();
        }
#endif // __has_include(<rapidjson/writer.h>)
    };

    template <typename T, size_t N>
    constexpr size_t asizeof(T(&)[N]) { return N; }
}
