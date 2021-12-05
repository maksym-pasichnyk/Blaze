#pragma once

#include <map>
#include <span>
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <variant>
#include <numeric>
#include <optional>
#include <streambuf>
#include <type_traits>
#include <unordered_map>
#include <util/match.hpp>
#include <range/v3/view.hpp>

#include <spdlog/spdlog.h>

struct Json {
	struct Read;
	struct Dump;

	struct Null {};
	using Bool = bool;
	using Number = std::variant<int64_t, double>;
	using String = std::string;
	using Array = std::vector<Json>;
	using Object = std::map<std::string, Json>;

	using Value = std::variant<
		Null,
		Bool,
		Number,
		String,
		Array,
		Object
	>;

    template <typename T>
    struct From {
        static auto from(const T &val) -> Json = delete;
    };

	template <typename T>
	struct Into {
		static auto into(const Json&) -> std::optional<T>;
	};

	Json() noexcept : m_storage(Null{}) {}
	Json(std::initializer_list<std::pair<const std::string, Json>> v) noexcept : m_storage(Object{v}) {}

    template <typename T> requires (std::is_convertible_v<T, Value> && !std::is_same_v<std::decay_t<T>, Json>)
    Json(T&& obj) : m_storage(std::forward<T>(obj)) {}

	template <typename T> requires (!std::is_convertible_v<T, Value> && !std::is_same_v<std::decay_t<T>, Json>)
	Json(T&& obj) : Json(From<std::decay_t<T>>::from(std::forward<T>(obj))) {}

    Json(Json&&) noexcept = default;
    Json(const Json&) noexcept = default;

	Json& operator=(Json&&) = default;
	Json& operator=(const Json&) = default;

	template<typename T>
	operator T() const {
		return Into<std::decay_t<T>>::into(*this).value();
	}

	auto is_null() const noexcept -> bool {
		return m_storage.index() == 0;
	}

	auto is_bool() const noexcept -> bool {
		return m_storage.index() == 1;
	}

	auto is_number() const noexcept -> bool {
		return m_storage.index() == 2;
	}

	auto is_string() const noexcept -> bool {
		return m_storage.index() == 3;
	}

	auto is_array() const noexcept -> bool {
		return m_storage.index() == 4;
	}

	auto is_object() const noexcept -> bool {
		return m_storage.index() == 5;
	}

	auto to_string() const noexcept -> const std::string& {
		return std::get<String>(m_storage);
	}

	auto to_bool() const noexcept -> bool {
		return std::get<Bool>(m_storage);
	}

	auto to_i64() const noexcept -> int64_t {
		return match(std::get<Number>(m_storage),
			[](int64_t v) { return v; },
			[](double v) { return static_cast<int64_t>(v); }
		);
	}

	auto to_f64() const noexcept -> double {
		return match(std::get<Number>(m_storage),
			[](int64_t v) { return static_cast<double>(v); },
			[](double v) { return v; }
		);
	}

	auto to_array() const noexcept -> const Array& {
		return std::get<Array>(m_storage);
	}

	auto to_object() const noexcept -> const Object& {
		return std::get<Object>(m_storage);
	}

    auto at(const std::string& key) const noexcept -> const Json& {
        return to_object().at(key);
    }

    auto contains(const std::string& key) const noexcept -> bool {
        auto&& o = to_object();
        return o.find(key) != o.end();
    }

    template <typename T>
    auto into() const -> T {
        return Into<std::decay_t<T>>::into(*this).value();
    }

//    auto find(const std::string& key) const noexcept -> std::optional<std::reference_wrapper<const Json>> {
//        auto&& o = to_object();
//        if (auto it = o.find(key); it != o.end()) {
//            return std::cref(it->second);
//        }
//        return std::nullopt;
//    }

    template <typename U>
    auto value_or(const std::string& key, U&& value) const -> U {
        auto&& o = to_object();
        if (auto it = o.find(key); it != o.end()) {
            return it->second.into<U>();
        }
        return std::forward<U>(value);
    }

private:
	Value m_storage;
};

template<>
struct Json::From<bool> {
    static auto from(const bool& val) -> Json {
        return Json::Bool{val};
    }
};

template <typename T> requires (
	std::is_same_v<T, int8_t> ||
	std::is_same_v<T, int16_t> ||
	std::is_same_v<T, int32_t> ||
	std::is_same_v<T, int64_t> ||
	std::is_same_v<T, uint8_t> ||
	std::is_same_v<T, uint16_t> ||
	std::is_same_v<T, uint32_t> ||
	std::is_same_v<T, uint64_t>
)
struct Json::From<T> {
	static auto from(const T& val) -> Json {
		return Json::Number{static_cast<int64_t>(val)};
	}
};

template <typename T> requires (
    std::is_same_v<T, float> ||
    std::is_same_v<T, double>
)
struct Json::From<T> {
    auto from(const T &val) -> Json {
        return Json::Number{static_cast<double>(val)};
    }
};

template<typename T, size_t N>
struct Json::From<std::array<T, N>> {
	static auto from(const std::array<T, N>& elements) -> Json {
		return elements | ranges::views::transform([](auto element) -> Json { return std::move(element); }) | ranges::to_vector;
	}
};

template<typename T>
struct Json::From<std::vector<T>> {
	static auto from(const std::vector<T> &elements) -> Json {
		return elements | ranges::views::transform([](auto element) -> Json { return std::move(element); }) | ranges::to_vector;
	}
};

template<typename K, typename V, typename P, typename A>
struct Json::From<std::map<K, V, P, A>> {
	static auto from(const std::map<K, V, P, A> &elements) -> Json {
		Json::Object obj{};
		for (auto&& [k, v] : elements) {
			obj.emplace(Json(k).to_string(), Json(v));
		}
		return obj;
	}
};

template<typename K, typename V, typename P, typename A>
struct Json::Into<std::map<K, V, P, A>> {
    static auto into(const Json& obj) -> std::optional<std::map<K, V, P, A>> {
        std::map<K, V, P, A> ret{};
        for (auto&& [k, v] : obj.to_object()) {
            ret.emplace(Json(k), v);
        }
        return ret;
    }
};

template <>
struct Json::Into<bool> {
    static auto into(const Json& obj) -> std::optional<bool> {
        return obj.to_bool();
    }
};

template <typename T> requires (
	std::is_same_v<T, float> ||
	std::is_same_v<T, double>
)
struct Json::Into<T> {
	static auto into(const Json& obj) -> std::optional<T> {
		return obj.to_f64();
	}
};

template <typename T> requires (
	std::is_same_v<T, int8_t> ||
	std::is_same_v<T, int16_t> ||
	std::is_same_v<T, int32_t> ||
	std::is_same_v<T, int64_t> ||
	std::is_same_v<T, uint8_t> ||
	std::is_same_v<T, uint16_t> ||
	std::is_same_v<T, uint32_t> ||
	std::is_same_v<T, uint64_t>
)
struct Json::Into<T> {
	static auto into(const Json& obj) -> std::optional<T> {
		return obj.to_i64();
	}
};

template<typename T, size_t N>
struct Json::Into<std::array<T, N>> {
	template <size_t... I>
	static auto to_array(const Json::Array& elements, std::index_sequence<I...>) -> std::array<T, N> {
		return { elements.at(I)... };
	}

	static auto into(const Json& obj) -> std::optional<std::array<T, N>> {
		return to_array(obj.to_array(), std::make_index_sequence<N>{});
	}
};

template<typename T>
struct Json::Into<std::vector<T>> {
	static auto into(const Json& obj) -> std::optional<std::vector<T>> {
		return obj.to_array() | ranges::views::transform([](const auto& element) -> T { return element; }) | ranges::to<std::vector<T>>();
	}
};

struct Json::Read {
	struct End {};
	struct Comma {};
	struct Column {};
	struct BeginArray {};
	struct EndArray {};
	struct BeginObject {};
	struct EndObject {};

	using String = Json::String;
	using Number = Json::Number;
	using Bool = Json::Bool;
	using Null = Json::Null;

	using Token = std::variant<
		End,
		Comma,
		Column,
		BeginArray,
		EndArray,
		BeginObject,
		EndObject,

		String,
		Number,
		Bool,
		Null
	>;

	template <typename Fn, typename... Args>
	static auto take_while(std::istream& stream, Fn&& fn, Args&&... args) {
		while (!stream.eof() && fn(stream.peek(), std::forward<Args>(args)...)) {
			stream.get();
		}
	}

	static auto read_string(std::istream& stream, const char ch) noexcept -> std::optional<Token> {
		std::string out{};

		stream.get();

		take_while(stream, [&stream, ch, &out](char c) -> bool {
			if (c == ch || c == '\n') {
				return false;
			}
			if (c == '\\') {
				stream.get();
				if (stream.eof()) {
					return false;
				}
				switch (const auto s = static_cast<char>(stream.peek())) {
					case 'n': out.push_back('\n'); break;
					case 'r': out.push_back('\r'); break;
					case 't': out.push_back('\t'); break;
					case 'v': out.push_back('\v'); break;
					default: out.push_back(s); break;
				}
				return true;
			}
			out.push_back(c);
			return true;
		});

		if (stream.peek() != ch) {
//				fmt::print("unterminated string literal");
			return std::nullopt;
		}

		stream.get();

		return std::move(out);
	}

	static auto read_number(std::istream& stream) -> std::optional<Token> {
		const auto sign = stream.peek() == '-';
		if (sign) {
			stream.get();
		}

		std::string out{};

		bool flag = true;
		take_while(stream, [&flag, &out](char c) -> bool {
			if (isdigit(c) || (c == '.' && std::exchange(flag, false))) {
				out.push_back(c);
				return true;
			}
			return false;
		});

        if (stream.peek() == 'e' || stream.peek() == 'E') {
            flag = true;

            out.push_back(stream.peek());
            stream.get();

            if (stream.peek() == '-' || stream.peek() == '+') {
                out.push_back(stream.peek());
                stream.get();
            }
            take_while(stream, [&out](char c) -> bool {
                if (isdigit(c)) {
                    out.push_back(c);
                    return true;
                }
                return false;
            });
        }

		if (flag) {
			const auto num = std::stoi(out, nullptr);
			return Number{static_cast<int64_t>(sign ? -num : num)};
		} else {
			const auto num = std::stod(out, nullptr);
			return Number{static_cast<double>(sign ? -num : num)};
		}
	}

	static auto read_ident(std::istream& stream) -> std::optional<Token> {
		std::string out{};

		using namespace std::string_view_literals;

		take_while(stream, [&out](char c) -> bool {
			if (isalnum(c) || (c == '_')) {
				out.push_back(c);
				return true;
			}
			return false;
		});

		if (out == "true"sv) {
			return Bool{ true };
		} else if (out == "false"sv) {
			return Bool{ false };
		} else if (out == "null"sv) {
			return Null{};
		} else {
			return std::nullopt;
		}
	}

	static void nextline(std::istream& stream, char c) noexcept {
		stream.get();
		if (!stream.eof() && (c != stream.peek()) && isnewline(stream.peek())) {
			stream.get();
		}
	}

	static auto next_token(std::istream& stream) noexcept -> std::optional<Token> {
		while (!stream.eof()) {
			switch (char c = stream.peek()) {
				case '\n': case '\r':
					nextline(stream, c);
					continue;
				case ' ': case '\t': case '\v': case '\f':
					stream.get();
					continue;
                case '/':
                    stream.get();
                    if (stream.peek() == '/') {
                        stream.get();
                        while (!stream.eof()) {
                            if (isnewline(stream.peek())) {
                                nextline(stream, stream.peek());
                                break;
                            } else {
                                stream.get();
                            }
                        }
                        continue;
                    }
                    return std::nullopt;
				case '{':
					stream.get();
					return BeginObject{};
				case '}':
					stream.get();
					return EndObject{};
				case '[':
					stream.get();
					return BeginArray{};
				case ']':
					stream.get();
					return EndArray{};
				case ':':
					stream.get();
					return Column{};
				case ',':
					stream.get();
					return Comma{};
				case '"': case '\'':
					return read_string(stream, c);
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9': case '-':
					return read_number(stream);
				default:
					if (isalpha(c) || (c == '_')) {
						return read_ident(stream);
					}
					return std::nullopt;
			}
		}
		return End{};
	}

	static auto read(std::istream& stream) noexcept -> std::optional<Json> {
		return parse(stream, next_token(stream).value());
	}

    static auto read(std::istream&& stream) noexcept -> std::optional<Json> {
        return read(stream);
    }

	static auto parse(std::istream& stream, const Token& in) noexcept -> std::optional<Json> {
		return match(in,
			[&stream](BeginArray) -> std::optional<Json> {
				auto tk = next_token(stream).value();
				if (is<EndArray>(tk)) {
					return Json::Array{};
				}

				Json::Array out{};
				while (true) {
					out.emplace_back(parse(stream, tk).value());
					tk = next_token(stream).value();
					if (is<EndArray>(tk)) {
						return std::move(out);
					}
					if (!is<Comma>(tk)) {
						fmt::print("Json: syntax error");
						return std::nullopt;
					}
					tk = next_token(stream).value();
				}
			},
			[&stream](BeginObject) -> std::optional<Json> {
				auto tk = next_token(stream).value();
				if (is<EndObject>(tk)) {
					return Json::Object{};
				}

				Json::Object out{};
				while (true) {
					auto name = std::move(std::get<std::string>(tk));
					tk = next_token(stream).value();
					if (!is<Column>(tk)) {
						fmt::print("Json: syntax error");
						return std::nullopt;
					}
					tk = next_token(stream).value();
					out.emplace(std::move(name), parse(stream, tk).value());
					tk = next_token(stream).value();
					if (is<EndObject>(tk)) {
						return std::move(out);
					}
					if (!is<Comma>(tk)) {
						fmt::print("Json: syntax error");
						return std::nullopt;
					}
					tk = next_token(stream).value();
				}
			},
			[](const String& val) -> std::optional<Json> {
				return val;
			},
			[](const Number& val) -> std::optional<Json> {
				return val;
			},
			[](const Bool& val) -> std::optional<Json> {
				return val;
			},
			[](const Null& val) -> std::optional<Json> {
				return val;
			},
			[](const auto& val) -> std::optional<Json> {
				return std::nullopt;
			}
		);
	}

	template <typename T>
	static auto is(const Token& tk) -> bool {
		return std::get_if<T>(&tk) != nullptr;
	}

	static auto isnewline(const char c) noexcept -> bool {
		return (c == '\n') || (c == '\r');
	}
};

struct Json::Dump {
	static void dump(std::ostream &out, const Json &obj, int ident = 0) {
		match(obj.m_storage,
			[&out](const Json::Null&) {
				out << "null";
			},
			[&out](const Json::Bool& v) {
				out << (v ? "true" : "false");
			},
			[&out](const Json::Number& v) {
				return match(v, [&out](auto v) { out << v; });
			},
			[&out](const Json::String& v) {
				out << '"' << v << '"';
			},
			[&out, ident](const Json::Array &arr) {
				out << '[';
				if (!arr.empty()) {
					out << std::endl << fmt::format(R"({:{}})", "", ident + 4);
					dump(out, *arr.begin(), ident + 4);

					if (arr.size() > 1) {
						const auto _begin = std::next(arr.begin());
						const auto _end = arr.end();

						for (auto v = _begin; v != _end; v++) {
							out << ',' << std::endl << fmt::format(R"({:{}})", "", ident + 4);
							dump(out, *v, ident + 4);
						}
					}
					out << std::endl << fmt::format(R"({:{}})", "", ident);
				}
				out << ']';
			},
			[&out, ident](const Json::Object &obj) {
				out << '{';
				if (!obj.empty()) {
					out << std::endl;
					auto first = obj.begin();
					out << fmt::format(R"({:{}}"{}": )", "", ident + 4, first->first);
					dump(out, first->second, ident + 4);

					if (obj.size() > 1) {
						const auto _begin = std::next(obj.begin());
						const auto _end = obj.end();

						for (auto v = _begin; v != _end; v++) {
							out << ',' << std::endl;
							out << fmt::format(R"({:{}}"{}": )", "", ident + 4, v->first);
							dump(out, v->second, ident + 4);
						}
					}
					out << std::endl << fmt::format(R"({:{}})", "", ident);
				}
				out << '}';
			},
			[&out](const auto &) {}
		);
	}
};

static auto operator<<(std::ostream& out, const Json& obj) noexcept -> std::ostream& {
	Json::Dump::dump(out, obj, 0);
	return out;
}

static auto operator<<(std::ostream&& out, const Json& obj) noexcept -> std::ostream&& {
	Json::Dump::dump(out, obj, 0);
	return std::move(out);
}

static auto operator>>(std::istream& in, Json& obj) noexcept -> std::istream& {
	obj = Json::Read::read(in).value();
	return in;
}

static auto operator>>(std::istream&& in, Json& obj) noexcept -> std::istream&& {
	obj = Json::Read::read(in).value();
	return std::move(in);
}