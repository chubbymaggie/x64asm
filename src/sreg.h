/*
Copyright 2013 eric schkufza

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef X64ASM_SRC_SREG_H
#define X64ASM_SRC_SREG_H

#include <cassert>
#include <iostream>

#include "src/operand.h"

namespace x64asm {

/** A segment register. The segment register bit assignments are ES = 0,
    CS = 1, SS = 2, DS = 3, FS = 4, and GS = 5.
*/
class Sreg : public Operand {
    // Needs access to constructor.
    friend class Constants;
    // Needs access to constructor.
    template <class T>
    friend class M;
    // Needs access to constructor.
    friend class Moffs;

  public:
    /** Returns true if this segment register is well-formed. */
    constexpr bool check() {
			return val_ < 6;
		}

    /** Comparison based on on val_. */
    constexpr bool operator<(const Sreg& rhs) {
			return val_ < rhs.val_;
		}
    /** Comparison based on on val_. */
    constexpr bool operator==(const Sreg& rhs) {
			return val_ == rhs.val_;
		}
    /** Comparison based on on val_. */
    constexpr bool operator!=(const Sreg& rhs) {
			return !(*this == rhs);
		}

    /** Conversion based on underlying value. */
    constexpr operator uint64_t() {
			return val_;
		}

    /** STL-compliant hash. */
    constexpr size_t hash() {
			return val_;
		}

		/** @todo This method is undefined. */
		std::istream& read_att(std::istream& is) {
			is.setstate(std::ios::failbit);
			return is;
		}
    /** Writes this segment register to an ostream using at&t syntax. */
    std::ostream& write_att(std::ostream& os) const {
			assert(check());
			const char* sregs[6] = {"es","cs","ss","ds","fs","gs"};
			return (os << "%" << sregs[val_]);
		}

  protected:
    /** Direct access to this constructor is disallowed. */
    constexpr Sreg(uint64_t val) : Operand(Type::SREG, val) {}
    constexpr Sreg(Type t, uint64_t val) : Operand(t, val) {}
};

/** The segment register FS. */
class Fs : public Sreg {
    // Needs access to constructor.
    friend class Constants;

  public:
    /** Checks whether this segment register is %fs. */
    constexpr bool check() {
			return val_ == 4;
		}

  private:
    /** Direct access to this constructor is disallowed. */
    constexpr Fs() : Sreg(Type::FS, 4) {}
};

/** The segment register GS. */
class Gs : public Sreg {
    // Needs access to constructor.
    friend class Constants;

  public:
    /** Checks whether this segment register is %gs. */
    constexpr bool check() {
			return val_ == 5;
		}

  private:
    /** Direct access to this constructor is disallowed. */
    constexpr Gs() : Sreg(Type::GS, 5) {}
};

} // namespace x64asm

namespace std {

/** STL hash specialization. */
template <>
struct hash<x64asm::Sreg> {
  size_t operator()(const x64asm::Sreg& s) const {
		return s.hash();
	}
};

/** iostream overload. */
inline istream& operator>>(istream& is, x64asm::Sreg& s) {
	return s.read_att(is);
}
/** iostream overload. */
inline ostream& operator<<(ostream& os, const x64asm::Sreg& s) {
	return s.write_att(os);
}

} // namespace std

#endif
