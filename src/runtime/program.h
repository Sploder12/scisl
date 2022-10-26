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

		operator ScislType() const { return data; }

		template <typename T>
		explicit operator T () const {
			return std::get<T>(data);
		}

		constexpr Val() = default;

		constexpr Val(const Val& other) :
			data(other.data) {}

		template <typename T>
		constexpr Val(T ptr) :
			data(ptr) {}

		constexpr ValType type() const {
			return (ValType)data.index();
		}

		template <typename T>
		constexpr T as() const;

		template <>
		constexpr SCISL_INT as() const {
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

		template <>
		constexpr SCISL_FLOAT as() const {
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

		template <>
		constexpr SCISL_STR as() const {
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

		constexpr Val& operator=(SCISL_INT val) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& v = *(T)(*this);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					v = val;
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					v = (SCISL_FLOAT)val;
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					v = std::to_string(val);
				else
					static_assert(false);
			}, data);

			return *this;
		}

		constexpr Val& operator=(SCISL_FLOAT val) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& v = *(T)(*this);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					v = (SCISL_INT)val;
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					v = val;
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					v = std::to_string(val);
				else
					static_assert(false);
			}, data);

			return *this;
		}

		constexpr Val& operator=(const SCISL_STR& val) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				auto& v = *(T)(*this);
				if constexpr (std::is_same_v<T, SCISL_INT*>)
					v = (SCISL_INT)std::stoi(val);
				else if constexpr (std::is_same_v<T, SCISL_FLOAT*>)
					v = (SCISL_FLOAT)std::stod(val);
				else if constexpr (std::is_same_v<T, SCISL_STR*>)
					v = val;
				else
					static_assert(false);
			}, data);

			return *this;
		}

		constexpr Val& operator=(const Val& other) {
			std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				val = other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);

			return *this;
		}

		constexpr Val& operator=(Val&& moved) noexcept {
			std::swap(data, moved.data);
			return *this;
		}

		constexpr Val& operator+=(const Val& other) {
			std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				val += other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);

			return *this;
		}

		constexpr Val& operator-=(const Val& other) {
			std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>) {
					auto& val = *(ScislPtr)(*this);
					val -= other.as<std::remove_pointer_t<ScislPtr>>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator*=(const Val& other) {
			std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				if constexpr (std::is_same_v<ScislPtr, SCISL_STR*>) {
					SCISL_STR tmp = "";
					SCISL_INT times = other.as<SCISL_INT>();
					tmp.reserve(times * val.size());

					for (size_t i = 0; i < times; ++i) {
						tmp += val;
					}
					val = std::move(tmp);
				}
				else val *= other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);

			return *this;
		}

		constexpr Val& operator/=(const Val& other) {
			std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>) {
					auto& val = *(ScislPtr)(*this);
					val /= other.as<std::remove_pointer_t<ScislPtr>>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator%=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = *(T)(*this);
					val %= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator|=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = *(T)(*this);
					val |= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator&=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = *(T)(*this);
					val &= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator^=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = *(T)(*this);
					val ^= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator>>=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = *(T)(*this);
					val >>= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator<<=(const Val& other) {
			std::visit([&](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					auto& val = *(T)(*this);
					val <<= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}
	

		constexpr bool operator<(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return val < other.as<std::remove_pointer_t<ScislPtr>>();
				else
					return false;
			}, data);
		}

		constexpr bool operator>(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return val > other.as<std::remove_pointer_t<ScislPtr>>();
				else
					return false;
			}, data);
		}

		constexpr bool operator==(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				return val == other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);
		}

		constexpr bool operator&&(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return val && other.as<std::remove_pointer_t<ScislPtr>>();
				else
					return false;
			}, data);
		}

		constexpr bool operator||(const Val& other) const {
			return std::visit([&](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				auto& val = *(ScislPtr)(*this);
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return val || other.as<std::remove_pointer_t<ScislPtr>>();
				else
					return false;
			}, data);
		}
	};

	// default constructed Val
	template <class T>
	constexpr Val createTemporary(void* buf) {
		return { std::construct_at((T*)buf) };

	}

	constexpr Val createTemporary(ValType type, void* buf) {
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

	constexpr Val createTemporary(const Val& val, void* buf) {
		Val out{ createTemporary(val.type(), buf) };
		out = val;

		return out;
	}

	constexpr void deleteTemporary(Val& temp) {
		std::visit([](auto&& d) {
			std::destroy_at(d);
		}, temp.data);
	}


	struct Instruction {
		void (*func)(Program&, std::vector<Val>&) = nullptr;
		std::vector<Val> args{};

		constexpr void run(Program& prog) {
			func(prog, args);
		}
	};

	struct Program {
		std::stack<size_t, std::vector<size_t>> callstack{};

		std::vector<SCISL_STR*> strs{}; // stored so cleanup can be done correctly
		uint8_t* data; // all variables and constants are stored here
		size_t dataSize;

		std::vector<Instruction> instructions;
		size_t rip = 0;

		int returnVal = 0;
		bool broke = false;
		
		constexpr Program() :
			data(nullptr), dataSize(0), instructions({}) {}

		Program(const Program&) = delete;
		constexpr Program(Program&& other) noexcept :
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

		constexpr ~Program() {
			if (data == nullptr) return;

			std::destroy(strs.begin(), strs.end());

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