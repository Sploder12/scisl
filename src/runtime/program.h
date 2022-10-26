#ifndef _SCISL_PROGRAM_H
#define _SCISL_PROGRAM_H

#include <vector>
#include <stack>
#include <string>

#include <algorithm>
#include <execution>

#include "../compiler/type.h"

namespace scisl {
	struct Program;

	struct Val {
		ScislType data;

		constexpr Val() = default;

		constexpr Val(const Val& other) {
			data = other.data;
		}

		constexpr Val(SCISL_INT* ptr):
			data() {
				data = ptr;
		}

		constexpr Val(SCISL_FLOAT* ptr) :
			data() {
			data = ptr;
		}

		constexpr Val(SCISL_STR* ptr) :
			data() {
			data = ptr;
		}

		SCISL_INT asInt() const {
			return std::visit([](auto&& val) {
				using T = std::decay_t<decltype(val)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return *val;
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return (SCISL_INT)*val;
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					return (SCISL_INT)std::stoi(*val);
				else 
					static_assert(false, "Cast to ScislInt from Err not possible.");
			}, data);
		}

		SCISL_FLOAT asFloat() const {
			return std::visit([](auto&& val) {
				using T = std::decay_t<decltype(val)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return (SCISL_FLOAT)*val;
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return *val;
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					return (SCISL_FLOAT)std::stod(*val);
				else
					static_assert(false, "Cast to ScislFloat from Err not possible.");
			}, data);
		}

		SCISL_STR asStr() const {
			return std::visit([](auto&& val) {
				using T = std::decay_t<decltype(val)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return std::to_string(*val);
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return std::to_string(*val);
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					return *val;
				else
					static_assert(false, "Cast to ScislStr from Err not possible.");
			}, data);
		}

		// the backbone of all instructions
		constexpr Val& operator=(const ScislType& data) {
			this->data = data;
			return *this;
		}

		Val& operator=(SCISL_INT val) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& v = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*v = val;
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*v = (SCISL_FLOAT)val;
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					*v = std::to_string(val);
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator=(SCISL_FLOAT val) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& v = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*v = (SCISL_INT)val;
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*v = val;
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					*v = std::to_string(val);
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator=(const SCISL_STR& val) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& v = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*v = (SCISL_INT)std::stoi(val);
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*v = (SCISL_FLOAT)std::stod(val);
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					*v = val;
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*val = other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*val = other.asFloat();
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					*val = other.asStr();
				else
					static_assert(false);
			}, data);

			return *this;
		}

		constexpr Val& operator=(Val&& moved) noexcept {
			std::swap(data, moved.data);
			return *this;
		}

		Val& operator+=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*val += other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*val += other.asFloat();
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					*val += other.asStr();
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator-=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*val -= other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*val -= other.asFloat();
				else if constexpr (std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator*=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*val *= other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*val *= other.asFloat();
				else if constexpr (std::is_same_v<T, SCISL_STR*>) {
					SCISL_STR tmp = "";
					SCISL_INT times = other.asInt();
					tmp.reserve(times * val->size());

					for (size_t i = 0; i < times; ++i) {
						tmp += *val;
					}
					*val = std::move(tmp);
				}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator/=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					*val /= other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					*val /= other.asFloat();
				else if constexpr (std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator%=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = std::get<T>(this->data);
					*val %= other.asInt();
				}
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*> || std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator|=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = std::get<T>(this->data);
					*val |= other.asInt();
				}
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*> || std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator&=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = std::get<T>(this->data);
					*val &= other.asInt();
				}
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*> || std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator^=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = std::get<T>(this->data);
					*val ^= other.asInt();
				}
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*> || std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator>>=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = std::get<T>(this->data);
					*val >>= other.asInt();
				}
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*> || std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}

		Val& operator<<=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = std::get<T>(this->data);
					*val <<= other.asInt();
				}
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*> || std::is_same_v<T, SCISL_STR*>) {}
				else
					static_assert(false);
			}, data);

			return *this;
		}
	

		bool operator<(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = *std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return val < other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return val < other.asFloat();
				else
					return false;
			}, data);
		}

		bool operator>(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = *std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return val > other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return val > other.asFloat();
				else
					return false;
			}, data);
		}

		bool operator==(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = *std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return val == other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return val == other.asFloat();
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					return val == other.asStr();
				else
					return false;
			}, data);
		}

		bool operator&&(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = *std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return val && other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return val && other.asFloat();
				else
					return false;
			}, data);
		}

		bool operator||(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& val = *std::get<T>(this->data);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					return val || other.asInt();
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					return val || other.asFloat();
				else
					return false;
			}, data);
		}
	};

	// default constructed Val
	template <class T>
	constexpr inline Val createTemporary(void* buf) {
	
		Val out{ (T*)buf };

		auto& dat = std::get<T*>(out.data);

		if constexpr (std::is_same_v<T, SCISL_INT*>)
			*dat = (SCISL_INT)0;
		else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
			*dat = (SCISL_FLOAT)0.0;
		else if constexpr (std::is_same_v<T, SCISL_STR*>)
			*dat = (SCISL_STR)"";

		return out;

	}

	template <typename T>
	constexpr ValType toValType() {
		return ValType::err;
	}
	
	template <>
	constexpr ValType toValType<SCISL_INT>() {
		return ValType::integer;
	}

	template <>
	constexpr ValType toValType<SCISL_FLOAT>() {
		return ValType::floating;
	}

	template <>
	constexpr ValType toValType<SCISL_STR>() {
		return ValType::string;
	}

	template <typename T>
	inline Val createTemporary(const T& val, void* buf) {
		constexpr ValType type = toValType<T>();
		if constexpr (type == ValType::err) {
			return {};
		}
		else {
			Val out{ createTemporary(type, buf) };
			out = val;
			return out;
		}
	}

	static Val createTemporary(ValType type, void* buf) {
		switch (type) {
		case ValType::integer:
			return createTemporary<SCISL_INT>(buf);
		case ValType::floating:
			return createTemporary<SCISL_FLOAT>(buf);
		case ValType::string:
			return createTemporary<SCISL_STR>(buf);
		default:
			return {};
		}
	}

	template <>
	inline Val createTemporary(const Val& val, void* buf) {
		return std::visit([&](auto&& v) {
			using T = std::remove_pointer_t<std::decay_t<decltype(v)>>;

			Val out{ createTemporary(toValType<T>(), buf) };
			out = val;

			return out;
		}, val.data);
	}

	static void deleteTemporary(Val& temp) {
		std::visit([&](auto&& d) {
			using T = std::decay_t<decltype(d)>;

			if constexpr (std::is_same_v<T, SCISL_STR*>)
				d->~basic_string();
		}, temp.data);
	}


	
	struct Instruction {
		void (*func)(Program&, std::vector<Val>&) = nullptr;
		std::vector<Val> args{};

		inline void run(Program& prog) {
			func(prog, args);
		}
	};

	struct Program {
		std::stack<size_t, std::vector<size_t>> callstack{};

		std::vector<SCISL_STR*> strs{}; // stored to cleanup can be done correctly
		uint8_t* data; // all variables and constants are stored here
		size_t dataSize;

		std::vector<Instruction> instructions;
		size_t rip = 0;

		int returnVal = 0;
		bool broke = false;
		
		Program() :
			data(nullptr), dataSize(0), instructions({}) {}

		Program(const Program&) = delete;
		Program(Program&& other) noexcept :
			callstack(std::move(other.callstack)),
			strs(std::move(other.strs)),
			data(other.data), dataSize(other.dataSize),
			instructions(std::move(other.instructions)),
			rip(other.rip), returnVal(other.returnVal), broke(other.broke) {

			other.data = nullptr;
			other.strs = {};
		}

		Program& operator=(const Program&) = delete;

		Program& operator=(Program&& o) noexcept {
			std::swap(callstack, o.callstack);
			std::swap(strs, o.strs);
			std::swap(data, o.data);
			std::swap(dataSize, o.dataSize);
			std::swap(instructions, o.instructions);
			std::swap(rip, o.rip);
			std::swap(returnVal, o.returnVal);
			std::swap(broke, o.broke);
		}

		~Program() {
			if (data == nullptr) return;

			for (auto& str : strs) {
				str->~basic_string();
			}

			delete[] data;
		}

		SCISL_INT run() {
			size_t isize = instructions.size();
			if (rip >= isize) rip = 0;
			broke = false;

			while (rip < isize && !broke) {
				instructions[rip++].run(*this);
			}

			return returnVal;
		}
	};
}

#endif