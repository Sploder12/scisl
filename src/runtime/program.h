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

	template <typename T>
	inline constexpr T* cast(void* data) {
		return static_cast<T*>(data);
	}

	struct Val {
		void* data;
		ValType valtype;

		Val() noexcept :
			data(nullptr), valtype(ValType::err) {}

		Val(void* data, ValType valtype) noexcept :
			data(data), valtype(valtype) {}

		Val(const Val& o) :
			data(o.data), valtype(o.valtype) {}

		Val(Val&& other) noexcept:
			data(other.data), valtype(other.valtype) {
			other.data = nullptr;
			other.valtype = ValType::err;
		}


		inline SCISL_INT asInt() const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return *cast<SCISL_INT>(data);
			case scisl::ValType::floating:
				return (SCISL_INT)*cast<SCISL_FLOAT>(data); // let C++ handle float->int
			case scisl::ValType::string:
				return (SCISL_INT)std::stoi(*cast<SCISL_STR>(data));
			default:
				return 0; // really dangerous fallback
			}
		}

		inline SCISL_FLOAT asFloat() const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return (SCISL_FLOAT)*cast<SCISL_INT>(data); // let C++ handle int->float
			case scisl::ValType::floating:
				return *cast<SCISL_FLOAT>(data);
			case scisl::ValType::string:
				return (SCISL_FLOAT)std::stod(*cast<SCISL_STR>(data));
			default:
				return 0.0; // really dangerous fallback
			}
		}

		inline SCISL_STR asStr() const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return std::to_string(*cast<SCISL_INT>(data));
			case scisl::ValType::floating:
				return std::to_string(*cast<SCISL_FLOAT>(data));
			case scisl::ValType::string:
				return *cast<SCISL_STR>(data);
			default:
				return "";
			}
		}

		// the backbone of all instructions
		Val& operator=(void* data) {
			this->data = data;
			return *this;
		}

		Val& operator=(SCISL_INT val) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) = val;
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) = (SCISL_FLOAT)val;
				break;
			case scisl::ValType::string:
				*cast<SCISL_STR>(data) = std::to_string(val);
				break;
			default:
				break;
			}
			return *this;
		}

		Val& operator=(SCISL_FLOAT val) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) = (SCISL_INT)val;
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) = val;
				break;
			case scisl::ValType::string:
				*cast<SCISL_STR>(data) = std::to_string(val);
				break;
			default:
				break;
			}
			return *this;
		}

		Val& operator=(const SCISL_STR& val) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) = (SCISL_INT)std::stoi(val);
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) = (SCISL_FLOAT)std::stod(val);
				break;
			case scisl::ValType::string:
				*cast<SCISL_STR>(data) = val;
				break;
			default:
				break;
			}
			return *this;
		}

		Val& operator=(const Val& other) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) = other.asInt();
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) = other.asFloat();
				break;
			case scisl::ValType::string:
				*cast<SCISL_STR>(data) = other.asStr();
				break;
			default:
				break;
			}
			return *this;
		}

		Val& operator=(Val&& moved) noexcept {
			std::swap(data, moved.data);
			std::swap(valtype, moved.valtype);
			return *this;
		}

		Val& operator+=(const Val& other) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) += other.asInt();
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) += other.asFloat();
				break;
			case scisl::ValType::string:
				*cast<SCISL_STR>(data) += other.asStr();
				break;
			default:
				break;
			}
			return *this;
		}

		Val& operator-=(const Val& other) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) -= other.asInt();
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) -= other.asFloat();
				break;
			default:
				break;
			}
			return *this;
		}

		Val& operator*=(const Val& other) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) *= other.asInt();
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) *= other.asFloat();
				break;
			case scisl::ValType::string: {
				std::string tmp = "";
				SCISL_INT times = other.asInt();
				for (size_t i = 0; i < times; ++i) {
					tmp += *cast<SCISL_STR>(data);
				}
				*cast<SCISL_STR>(data) = tmp;
				break;
			}
			default:
				break;
			}
			return *this;
		}

		Val& operator/=(const Val& other) {
			switch (valtype)
			{
			case scisl::ValType::integer:
				*cast<SCISL_INT>(data) /= other.asInt();
				break;
			case scisl::ValType::floating:
				*cast<SCISL_FLOAT>(data) /= other.asFloat();
				break;
			default:
				break;
			}
			return *this;
		}

		Val& operator%=(const Val& other) {
			if(valtype == scisl::ValType::integer)
				*cast<SCISL_INT>(data) %= other.asInt();

			return *this;
		}

		Val& operator|=(const Val& other) {
			if (valtype == scisl::ValType::integer)
				*cast<SCISL_INT>(data) |= other.asInt();

			return *this;
		}

		Val& operator&=(const Val& other) {
			if (valtype == scisl::ValType::integer)
				*cast<SCISL_INT>(data) &= other.asInt();

			return *this;
		}

		Val& operator^=(const Val& other) {
			if (valtype == scisl::ValType::integer)
				*cast<SCISL_INT>(data) ^= other.asInt();

			return *this;
		}

		Val& operator>>=(const Val& other) {
			if (valtype == scisl::ValType::integer)
				*cast<SCISL_INT>(data) >>= other.asInt();

			return *this;
		}

		Val& operator<<=(const Val& other) {
			if (valtype == scisl::ValType::integer)
				*cast<SCISL_INT>(data) <<= other.asInt();

			return *this;
		}
	
		
		bool operator<(const Val& other) const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return *cast<SCISL_INT>(data) < other.asInt();
			case scisl::ValType::floating:
				return *cast<SCISL_FLOAT>(data) < other.asFloat();
			case scisl::ValType::string:
				return false;
			default:
				return false;
			}
		}

		bool operator>(const Val& other) const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return *cast<SCISL_INT>(data) > other.asInt();
			case scisl::ValType::floating:
				return *cast<SCISL_FLOAT>(data) > other.asFloat();
			case scisl::ValType::string:
				return false;
			default:
				return false;
			}
		}

		bool operator==(const Val& other) const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return *cast<SCISL_INT>(data) == other.asInt();
			case scisl::ValType::floating:
				return *cast<SCISL_FLOAT>(data) == other.asFloat();
			case scisl::ValType::string:
				return *cast<SCISL_STR>(data) == other.asStr();
			default:
				return false;
			}
		}

		bool operator&&(const Val& other) const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return *cast<SCISL_INT>(data) && other.asInt();
			case scisl::ValType::floating:
				return *cast<SCISL_FLOAT>(data) && other.asFloat();
			case scisl::ValType::string:
				return false;
			default:
				return false;
			}
		}

		bool operator||(const Val& other) const {
			switch (valtype)
			{
			case scisl::ValType::integer:
				return *cast<SCISL_INT>(data) || other.asInt();
			case scisl::ValType::floating:
				return *cast<SCISL_FLOAT>(data) || other.asFloat();
			case scisl::ValType::string:
				return false;
			default:
				return false;
			}
		}
	};

	template <typename T>
	inline constexpr T* cast(const Val& val) {
		return static_cast<T*>(val.data);
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
	inline Val createTemporary(const T& val, T* buffer) {
		constexpr ValType type = toValType<T>();
		if constexpr (type == ValType::err) {
			return {};
		}
		else {
			Val out{ buffer, toValType<T>() };
			out = val;
			return out;
		}
	}

	// default constructed Val
	inline Val createTemporary(ValType type, void* buffer) {
		Val out{ buffer, type };
		switch (type)
		{
		case ValType::integer:
			out = 0;
			break;
		case ValType::floating:
			out = 0.0f;
			break;
		case ValType::string:
			out = "";
			break;
		default:
			break;
		}
		return out;
	}

	inline void deleteTemporary(Val& val) {
		switch (val.valtype) {
		case ValType::string:
			((SCISL_STR*)(val.data))->~basic_string();
			break;
		default:
			break;
		}
		val.data = nullptr;
		val.valtype = ValType::err;
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

		void decompile(const char* filename);
	};
}

#endif