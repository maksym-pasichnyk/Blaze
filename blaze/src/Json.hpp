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
#include <tl/optional.hpp>
#include <spdlog/spdlog.h>
#include <range/v3/view.hpp>

struct Json {
    using Self = Json;

	struct Read;
	struct Dump;

	struct Null {};
	using Bool = bool;
	using Number = std::variant<int64_t, double>;
	using String = std::string;
	using Array = std::vector<Json>;
	using Object = std::unordered_map<std::string, Json>;

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
        using Value = T;
        static auto from(const Value& value) -> Self;
    };

	template <typename T>
	struct Into {
        using Value = T;
        using Result = tl::optional<T>;
		static auto into(const Self& self) -> Result;
	};

	Json() /*noexcept*/ : m_storage(Null{}) {}
	Json(std::initializer_list<std::pair<const std::string, Json>> v) /*noexcept*/ : m_storage(Object{v}) {}

    template <typename T> requires (std::is_convertible_v<T, Value> && !std::is_same_v<std::decay_t<T>, Json>)
    Json(T&& obj) : m_storage(std::forward<T>(obj)) {}

	template <typename T> requires (!std::is_convertible_v<T, Value> && !std::is_same_v<std::decay_t<T>, Json>)
	Json(T&& obj) : Json(From<std::decay_t<T>>::from(std::forward<T>(obj))) {}

    Json(Json&&) /*noexcept*/ = default;
    Json(const Json&) /*noexcept*/ = default;

	Json& operator=(Json&&) = default;
	Json& operator=(const Json&) = default;

	template<typename T>
	operator T() const {
		return into<T>();
	}

	auto is_null() const /*noexcept*/ -> bool {
		return m_storage.index() == 0;
	}

	auto is_bool() const /*noexcept*/ -> bool {
		return m_storage.index() == 1;
	}

	auto is_number() const /*noexcept*/ -> bool {
		return m_storage.index() == 2;
	}

	auto is_string() const /*noexcept*/ -> bool {
		return m_storage.index() == 3;
	}

	auto is_array() const /*noexcept*/ -> bool {
		return m_storage.index() == 4;
	}

	auto is_object() const /*noexcept*/ -> bool {
		return m_storage.index() == 5;
	}

	auto as_string() const noexcept -> tl::optional<const std::string&> {
		if (is_string()) {
            return std::get<std::string>(m_storage);
        }
        return tl::nullopt;
	}

	auto as_bool() const /*noexcept*/ -> tl::optional<bool> {
        if (is_bool()) {
            return std::get<Bool>(m_storage);
        }
        return tl::nullopt;
	}

	auto as_i64() const /*noexcept*/ -> tl::optional<int64_t> {
        if (is_number()) {
            return match(std::get<Number>(m_storage),
                [](int64_t v) { return v; },
                [](double v) { return static_cast<int64_t>(v); }
            );
        }
        return tl::nullopt;
	}

	auto as_f64() const /*noexcept*/ -> tl::optional<double> {
        if (is_number()) {
            return match(std::get<Number>(m_storage),
                [](int64_t v) { return static_cast<double>(v); },
                [](double v) { return v; }
            );
        }
        return tl::nullopt;
	}

	auto as_array() const /*noexcept*/ -> tl::optional<const Array&> {
        if (is_array()) {
            return std::get<Array>(m_storage);
        }
        return tl::nullopt;
	}

	auto as_object() const /*noexcept*/ -> tl::optional<const Object&> {
        if (is_object()) {
            return std::get<Object>(m_storage);
        }
        return tl::nullopt;
	}

    auto at(const std::string& key) const /*noexcept*/ -> const Json& {
        return as_object().value().at(key);
    }

    auto contains(const std::string& key) const /*noexcept*/ -> bool {
        auto&& o = as_object().value();
        return o.find(key) != o.end();
    }

    template <typename T>
    auto into() const -> T {
        return try_into<T>().value();
    }

    template <typename T>
    auto try_into() const -> tl::optional<T> {
        return Into<std::decay_t<T>>::into(*this);
    }

    auto find(const std::string& key) const /*noexcept*/ -> tl::optional<const Json&> {
        auto& o = std::get<Object>(m_storage);
        if (auto it = o.find(key); it != o.end()) {
            return {it->second};
        }
        return tl::nullopt;
    }

    template <typename U>
    auto value_or(const std::string& key, U&& value) const -> U {
        auto&& o = as_object().value();
        if (auto it = o.find(key); it != o.end()) {
            return it->second.into<U>();
        }
        return std::forward<U>(value);
    }

    template <typename... Ts, typename... Fs>
    friend auto match(Json& v, Fs&&... fs) -> decltype(auto) {
        return ::match(v.m_storage, std::forward<Fs>(fs)...);
    }

    template <typename... Ts, typename... Fs>
    friend auto match(Json&& v, Fs&&... fs) -> decltype(auto) {
        return ::match(std::move(v.m_storage), std::forward<Fs>(fs)...);
    }

    template <typename... Ts, typename... Fs>
    friend auto match(const Json& v, Fs&&... fs) -> decltype(auto) {
        return ::match(v.m_storage, std::forward<Fs>(fs)...);
    }

private:
	Value m_storage;
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
		auto obj = Json::Object{};
		for (auto&& [k, v] : elements) {
			obj.emplace(Json(k).as_string().value(), Json(v));
		}
		return obj;
	}
};

template<typename K, typename V, typename P, typename A>
struct Json::Into<std::map<K, V, P, A>> {
    static auto into(const Json& obj) -> tl::optional<std::map<K, V, P, A>> {
        return obj.as_object().map([](const Json::Object& o) {
            auto ret = std::map<K, V, P, A>{};
            for (auto&& [k, v] : o) {
                ret.emplace(Json(k), v);
            }
            return ret;
        });
    }
};

template<typename T, size_t N>
struct Json::Into<std::array<T, N>> {
    template <size_t... I>
    static auto to_array(const Json::Array& o, std::index_sequence<I...>) -> std::array<T, N> {
        return std::array<T, N>{ o.at(I)... };
    }

	static auto into(const Json& obj) -> tl::optional<std::array<T, N>> {
        return obj.as_array().map([](auto&& o) {
            return to_array(o, std::make_index_sequence<N>{});
        });
	}
};

template<typename T>
struct Json::Into<std::vector<T>> {
	static auto into(const Json& obj) -> tl::optional<std::vector<T>> {
        if (obj.is_object()) {
            return std::vector<T>{obj};
        }
		return obj.as_array().map([](auto&& o) {
            return o | ranges::views::transform([](const auto& element) -> T { return element; }) | ranges::to<std::vector<T>>();
        });
	}
};

template<typename K, typename V>
struct Json::Into<std::vector<std::pair<K, V>>> {
    static auto into(const Self& self) -> tl::optional<std::vector<std::pair<K, V>>> {
        return self.as_object().map([](auto&& o) {
            auto out = std::vector<std::pair<K, V>>{};
            for (auto&& [k, v] : o) {
                out.emplace_back(Json(k), v);
            }
            return out;
        });
    }
};

struct Json::Read {
    struct Internal;

    static auto read(std::istream& stream) -> tl::optional<Json>;
    static auto read(std::istream&& stream) -> tl::optional<Json> {
        return read(stream);
    }
};

struct Json::Dump {
	static void dump(std::ostream &out, const Json &obj, int ident = 0);
};