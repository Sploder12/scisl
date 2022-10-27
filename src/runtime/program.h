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
		constexpr T as() const {
			return std::visit([](auto&& val) {
				return ScislCast<T>(*val);
			}, data);
		}

		// the backbone of all instructions
		constexpr Val& operator=(const ScislType& data) {
			this->data = data;
			return *this;
		}


		template <typename T>
		constexpr Val& operator=(const T& val) {
			std::visit([&val](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				*dat = ScislCast<std::remove_pointer_t<ScislPtr>>(val);
			}, data);

			return *this;
		}

		constexpr Val& operator=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				*dat = other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);

			return *this;
		}

		constexpr Val& operator=(Val&& moved) noexcept {
			std::swap(data, moved.data);
			return *this;
		}

		constexpr Val& operator+=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				*dat += other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);

			return *this;
		}

		constexpr Val& operator-=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>) {
					*dat -= other.as<std::remove_pointer_t<ScislPtr>>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator*=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<ScislPtr, SCISL_STR*>) {
					SCISL_STR tmp = "";
					SCISL_INT times = other.as<SCISL_INT>();
					tmp.reserve(times * dat->size());

					for (size_t i = 0; i < times; ++i) {
						tmp += *dat;
					}
					*dat = std::move(tmp);
				}
				else *dat *= other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);

			return *this;
		}

		constexpr Val& operator/=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>) {
					*dat /= other.as<std::remove_pointer_t<ScislPtr>>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator%=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					*dat %= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator|=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					*dat |= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator&=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					*dat &= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator^=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					*dat ^= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator>>=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					*dat >>= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}

		constexpr Val& operator<<=(const Val& other) {
			std::visit([&other](auto&& dat) {
				using T = std::decay_t<decltype(dat)>;
				if constexpr (std::is_same_v<T, SCISL_INT*>) {
					*dat <<= other.as<SCISL_INT>();
				}
			}, data);

			return *this;
		}
	

		constexpr bool operator<(const Val& other) const {
			return std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return *dat < other.as<std::remove_pointer_t<ScislPtr>>();
				else
					return false;
			}, data);
		}

		constexpr bool operator>(const Val& other) const {
			return std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return *dat > other.as<std::remove_pointer_t<ScislPtr>>();
				else
					return false;
			}, data);
		}

		constexpr bool operator==(const Val& other) const {
			return std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				return *dat == other.as<std::remove_pointer_t<ScislPtr>>();
			}, data);
		}

		constexpr bool operator&&(const Val& other) const {
			return std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return *dat && other.as<std::remove_pointer_t<ScislPtr>>();
				else
					return false;
			}, data);
		}

		constexpr bool operator||(const Val& other) const {
			return std::visit([&other](auto&& dat) {
				using ScislPtr = std::decay_t<decltype(dat)>;
				if constexpr (!std::is_same_v<ScislPtr, SCISL_STR*>)
					return *dat || other.as<std::remove_pointer_t<ScislPtr>>();
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