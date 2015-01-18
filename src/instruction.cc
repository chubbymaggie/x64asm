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

#include "src/instruction.h"

#include "src/constants.h"
#include "src/label.h"
#include "src/mm.h"
#include "src/moffs.h"
#include "src/rel.h"
#include "src/st.h"
#include "src/xmm.h"
#include "src/ymm.h"

using namespace std;

namespace {

array<const char*, 3803> att_ {{
    // Internal mnemonics
    "<label definition>"
    // Auto-generated mnemonics
    #include "src/opcode.att"
}};

} // namespace

namespace x64asm {

bool Instruction::is_xor_reg_reg() const {
  // we special case for xor of two identical registers.  in that case,
  // the instruction sets the value of the register to zero, and this is a
  // safe operation even if the register was undefined before.
  switch (get_opcode()) {
    case PXOR_MM_MM:
    if (get_operand<Mm>(0) == get_operand<Mm>(1)) {
      return true;
    }
    break;

    case PXOR_XMM_XMM:
    if (get_operand<Xmm>(0) == get_operand<Xmm>(1)) {
      return true;
    }
    break;

    case VPXOR_XMM_XMM_XMM:
    if (get_operand<Xmm>(1) == get_operand<Xmm>(2)) {
      return true;
    }
    break;

    case VPXOR_YMM_YMM_YMM:
    if (get_operand<Ymm>(1) == get_operand<Ymm>(2)) {
      return true;
    }
    break;

    case XOR_RB_RB:
    if (get_operand<Rb>(0) == get_operand<Rb>(1)) {
      return true;
    }
    break;

    case XOR_RL_RL:
    if (get_operand<Rl>(0) == get_operand<Rl>(1)) {
      return true;
    }
    break;

    case XOR_RH_RH:
    if (get_operand<Rl>(0) == get_operand<Rl>(1)) {
      return true;
    }
    break;

    case XOR_R16_R16:
    if (get_operand<R16>(0) == get_operand<R16>(1)) {
      return true;
    }
    break;

    case XOR_R32_R32:
    if (get_operand<R32>(0) == get_operand<R32>(1)) {
      return true;
    }
    break;

    case XOR_R64_R64:
    if (get_operand<R64>(0) == get_operand<R64>(1)) {
      return true;
    }
    break;

    default:
    return false;
  }
}

RegSet& Instruction::explicit_must_read_set(RegSet& ret) const {
  if (is_xor_reg_reg()) {
    return ret;
  }
  for (size_t i = 0, ie = arity(); i < ie; ++i) {
    switch (type(i)) {
      case Type::M_8:
      case Type::M_16:
      case Type::M_32:
      case Type::M_64:
      case Type::M_128:
      case Type::M_256:
      case Type::M_16_INT:
      case Type::M_32_INT:
      case Type::M_64_INT:
      case Type::M_32_FP:
      case Type::M_64_FP:
      case Type::M_80_FP:
      case Type::M_80_BCD:
      case Type::M_2_BYTE:
      case Type::M_28_BYTE:
      case Type::M_108_BYTE:
      case Type::M_512_BYTE:
      case Type::FAR_PTR_16_16:
      case Type::FAR_PTR_16_32:
      case Type::FAR_PTR_16_64:
        ret += get_operand<M8>(i);
        continue;

      case Type::MOFFS_8:
      case Type::MOFFS_16:
      case Type::MOFFS_32:
      case Type::MOFFS_64:
        ret += get_operand<Moffs8>(i);
        continue;

      default:
        break;
    }

    if (!must_read(i)) {
      continue;
    }

    switch (type(i)) {
      case Type::MM:
        ret += get_operand<Mm>(i);
        break;
      case Type::RH:
        ret += get_operand<Rh>(i);
        break;
      case Type::RB:
        ret += get_operand<Rb>(i);
        break;
      case Type::AL:
      case Type::CL:
      case Type::RL:
        ret += get_operand<Rl>(i);
        break;
      case Type::AX:
      case Type::DX:
      case Type::R_16:
        ret += get_operand<R16>(i);
        break;
      case Type::EAX:
      case Type::R_32:
        ret += get_operand<R32>(i);
        break;
      case Type::RAX:
      case Type::R_64:
        ret += get_operand<R64>(i);
        break;
      case Type::FS:
      case Type::GS:
      case Type::SREG:
        ret += get_operand<Sreg>(i);
        break;
      case Type::ST_0:
      case Type::ST:
        ret += get_operand<St>(i);
        break;
      case Type::XMM_0:
      case Type::XMM:
        ret += get_operand<Xmm>(i);
        break;
      case Type::YMM:
        ret += get_operand<Ymm>(i);
        break;

      default:
        break;
    }
  }

  return ret;
}

RegSet& Instruction::explicit_maybe_read_set(RegSet& ret) const {
  if (is_xor_reg_reg()) {
    return ret;
  }
  for (size_t i = 0, ie = arity(); i < ie; ++i) {
    switch (type(i)) {
      case Type::M_8:
      case Type::M_16:
      case Type::M_32:
      case Type::M_64:
      case Type::M_128:
      case Type::M_256:
      case Type::M_16_INT:
      case Type::M_32_INT:
      case Type::M_64_INT:
      case Type::M_32_FP:
      case Type::M_64_FP:
      case Type::M_80_FP:
      case Type::M_80_BCD:
      case Type::M_2_BYTE:
      case Type::M_28_BYTE:
      case Type::M_108_BYTE:
      case Type::M_512_BYTE:
      case Type::FAR_PTR_16_16:
      case Type::FAR_PTR_16_32:
      case Type::FAR_PTR_16_64:
        ret += get_operand<M8>(i);
        continue;

      case Type::MOFFS_8:
      case Type::MOFFS_16:
      case Type::MOFFS_32:
      case Type::MOFFS_64:
        ret += get_operand<Moffs8>(i);
        continue;

      default:
        break;
    }

    if (!maybe_read(i)) {
      continue;
    }

    switch (type(i)) {
      case Type::MM:
        ret += get_operand<Mm>(i);
        break;
      case Type::RH:
        ret += get_operand<Rh>(i);
        break;
      case Type::RB:
        ret += get_operand<Rb>(i);
        break;
      case Type::AL:
      case Type::CL:
      case Type::RL:
        ret += get_operand<Rl>(i);
        break;
      case Type::AX:
      case Type::DX:
      case Type::R_16:
        ret += get_operand<R16>(i);
        break;
      case Type::EAX:
      case Type::R_32:
        ret += get_operand<R32>(i);
        break;
      case Type::RAX:
      case Type::R_64:
        ret += get_operand<R64>(i);
        break;
      case Type::FS:
      case Type::GS:
      case Type::SREG:
        ret += get_operand<Sreg>(i);
        break;
      case Type::ST_0:
      case Type::ST:
        ret += get_operand<St>(i);
        break;
      case Type::XMM_0:
      case Type::XMM:
        ret += get_operand<Xmm>(i);
        break;
      case Type::YMM:
        ret += get_operand<Ymm>(i);
        break;

      default:
        break;
    }
  }

  return ret;
}

RegSet& Instruction::explicit_must_write_set(RegSet& ret) const {
  for (size_t i = 0, ie = arity(); i < ie; ++i) {
    if (must_extend(i))
      switch (type(i)) {
        case Type::EAX:
        case Type::R_32:
          ret += get_operand<R64>(i);
          break;
        case Type::XMM_0:
        case Type::XMM:
          ret += get_operand<Ymm>(i);
          break;
        default:
          assert(false);
          break;
      }
    else if (must_write(i))
      switch (type(i)) {
        case Type::MM:
          ret += get_operand<Mm>(i);
          break;
        case Type::RH:
          ret += get_operand<Rh>(i);
          break;
        case Type::RB:
          ret += get_operand<Rb>(i);
          break;
        case Type::AL:
        case Type::CL:
        case Type::RL:
          ret += get_operand<Rl>(i);
          break;
        case Type::AX:
        case Type::DX:
        case Type::R_16:
          ret += get_operand<R16>(i);
          break;
        case Type::EAX:
        case Type::R_32:
          ret += get_operand<R32>(i);
          break;
        case Type::RAX:
        case Type::R_64:
          ret += get_operand<R64>(i);
          break;
        case Type::FS:
        case Type::GS:
        case Type::SREG:
          ret += get_operand<Sreg>(i);
          break;
        case Type::ST_0:
        case Type::ST:
          ret += get_operand<St>(i);
          break;
        case Type::XMM_0:
        case Type::XMM:
          ret += get_operand<Xmm>(i);
          break;
        case Type::YMM:
          ret += get_operand<Ymm>(i);
          break;

        default:
          break;
      }
    else
      break;
  }

  return ret;
}

RegSet& Instruction::explicit_maybe_write_set(RegSet& ret) const {
  for (size_t i = 0, ie = arity(); i < ie; ++i) {
    if (maybe_extend(i))
      switch (type(i)) {
        case Type::EAX:
        case Type::R_32:
          ret += get_operand<R64>(i);
          break;
        case Type::XMM_0:
        case Type::XMM:
          ret += get_operand<Ymm>(i);
          break;
        default:
          assert(false);
          break;
      }
    else if (maybe_write(i))
      switch (type(i)) {
        case Type::MM:
          ret += get_operand<Mm>(i);
          break;
        case Type::RH:
          ret += get_operand<Rh>(i);
          break;
        case Type::RB:
          ret += get_operand<Rb>(i);
          break;
        case Type::AL:
        case Type::CL:
        case Type::RL:
          ret += get_operand<Rl>(i);
          break;
        case Type::AX:
        case Type::DX:
        case Type::R_16:
          ret += get_operand<R16>(i);
          break;
        case Type::EAX:
        case Type::R_32:
          ret += get_operand<R32>(i);
          break;
        case Type::RAX:
        case Type::R_64:
          ret += get_operand<R64>(i);
          break;
        case Type::FS:
        case Type::GS:
        case Type::SREG:
          ret += get_operand<Sreg>(i);
          break;
        case Type::ST_0:
        case Type::ST:
          ret += get_operand<St>(i);
          break;
        case Type::XMM_0:
        case Type::XMM:
          ret += get_operand<Xmm>(i);
          break;
        case Type::YMM:
          ret += get_operand<Ymm>(i);
          break;

        default:
          break;
      }
    else 
      break;
  }

  return ret;
}

RegSet& Instruction::explicit_must_undef_set(RegSet& ret) const {
  for (size_t i = 0, ie = arity(); i < ie; ++i)
    if (must_undef(i))
      switch (type(i)) {
        case Type::MM:
          ret += get_operand<Mm>(i);
          break;
        case Type::RH:
          ret += get_operand<Rh>(i);
          break;
        case Type::RB:
          ret += get_operand<Rb>(i);
          break;
        case Type::AL:
        case Type::CL:
        case Type::RL:
          ret += get_operand<Rl>(i);
          break;
        case Type::AX:
        case Type::DX:
        case Type::R_16:
          ret += get_operand<R16>(i);
          break;
        case Type::EAX:
        case Type::R_32:
          ret += get_operand<R32>(i);
          break;
        case Type::RAX:
        case Type::R_64:
          ret += get_operand<R64>(i);
          break;
        case Type::FS:
        case Type::GS:
        case Type::SREG:
          ret += get_operand<Sreg>(i);
          break;
        case Type::ST_0:
        case Type::ST:
          ret += get_operand<St>(i);
          break;
        case Type::XMM_0:
        case Type::XMM:
          ret += get_operand<Ymm>(i);
          break;
        case Type::YMM:
          ret += get_operand<Ymm>(i);
          break;

        default:
          break;
      }
    else
      break;

  return ret;
}

RegSet& Instruction::explicit_maybe_undef_set(RegSet& ret) const {
  for (size_t i = 0, ie = arity(); i < ie; ++i)
    if (maybe_undef(i))
      switch (type(i)) {
        case Type::MM:
          ret += get_operand<Mm>(i);
          break;
        case Type::RH:
          ret += get_operand<Rh>(i);
          break;
        case Type::RB:
          ret += get_operand<Rb>(i);
          break;
        case Type::AL:
        case Type::CL:
        case Type::RL:
          ret += get_operand<Rl>(i);
          break;
        case Type::AX:
        case Type::DX:
        case Type::R_16:
          ret += get_operand<R16>(i);
          break;
        case Type::EAX:
        case Type::R_32:
          ret += get_operand<R32>(i);
          break;
        case Type::RAX:
        case Type::R_64:
          ret += get_operand<R64>(i);
          break;
        case Type::FS:
        case Type::GS:
        case Type::SREG:
          ret += get_operand<Sreg>(i);
          break;
        case Type::ST_0:
        case Type::ST:
          ret += get_operand<St>(i);
          break;
        case Type::XMM_0:
        case Type::XMM:
          ret += get_operand<Ymm>(i);
          break;
        case Type::YMM:
          ret += get_operand<Ymm>(i);
          break;

        default:
          break;
      }
    else
      break;

  return ret;
}

bool Instruction::check() const {
  for (size_t i = 0, ie = arity(); i < ie; ++i)
    switch (type(i)) {
      case Type::HINT:
        if (!get_operand<Hint>(i).check()) {
          return false;
        }
        break;

      case Type::IMM_8:
        if (!get_operand<Imm8>(i).check()) {
          return false;
        }
        break;
      case Type::IMM_16:
        if (!get_operand<Imm16>(i).check()) {
          return false;
        }
        break;
      case Type::IMM_32:
        if (!get_operand<Imm32>(i).check()) {
          return false;
        }
        break;
      case Type::IMM_64:
        if (!get_operand<Imm64>(i).check()) {
          return false;
        }
        break;
      case Type::ZERO:
        if (!get_operand<Zero>(i).check()) {
          return false;
        }
        break;
      case Type::ONE:
        if (!get_operand<One>(i).check()) {
          return false;
        }
        break;
      case Type::THREE:
        if (!get_operand<Three>(i).check()) {
          return false;
        }
        break;

      case Type::LABEL:
        if (!get_operand<Label>(i).check()) {
          return false;
        }
        break;

      case Type::M_8:
      case Type::M_16:
      case Type::M_32:
      case Type::M_64:
      case Type::M_128:
      case Type::M_256:
      case Type::M_16_INT:
      case Type::M_32_INT:
      case Type::M_64_INT:
      case Type::M_32_FP:
      case Type::M_64_FP:
      case Type::M_80_FP:
      case Type::M_80_BCD:
      case Type::M_2_BYTE:
      case Type::M_28_BYTE:
      case Type::M_108_BYTE:
      case Type::M_512_BYTE:
      case Type::FAR_PTR_16_16:
      case Type::FAR_PTR_16_32:
      case Type::FAR_PTR_16_64:
        if (!get_operand<M8>(i).check()) {
          return false;
        }
        break;

      case Type::MM:
        if (!get_operand<Mm>(i).check()) {
          return false;
        }
        break;

      case Type::MOFFS_8:
      case Type::MOFFS_16:
      case Type::MOFFS_32:
      case Type::MOFFS_64:
        if (!get_operand<Moffs8>(i).check()) {
          return false;
        }
        break;

      case Type::PREF_66:
        if (!get_operand<Pref66>(i).check()) {
          return false;
        }
        break;
      case Type::PREF_REX_W:
        if (!get_operand<PrefRexW>(i).check()) {
          return false;
        }
        break;
      case Type::FAR:
        if (!get_operand<Far>(i).check()) {
          return false;
        }
        break;

      case Type::RH:
        if (!get_operand<Rh>(i).check()) {
          return false;
        }
        break;
      case Type::RB:
        if (!get_operand<Rb>(i).check()) {
          return false;
        }
        break;
      case Type::AL:
        if (!get_operand<Al>(i).check()) {
          return false;
        }
        break;
      case Type::CL:
        if (!get_operand<Cl>(i).check()) {
          return false;
        }
        break;
      case Type::RL:
        if (!get_operand<Rl>(i).check()) {
          return false;
        }
        break;
      case Type::AX:
        if (!get_operand<Ax>(i).check()) {
          return false;
        }
        break;
      case Type::DX:
        if (!get_operand<Dx>(i).check()) {
          return false;
        }
        break;
      case Type::R_16:
        if (!get_operand<R16>(i).check()) {
          return false;
        }
        break;
      case Type::EAX:
        if (!get_operand<Eax>(i).check()) {
          return false;
        }
        break;
      case Type::R_32:
        if (!get_operand<R32>(i).check()) {
          return false;
        }
        break;
      case Type::RAX:
        if (!get_operand<Rax>(i).check()) {
          return false;
        }
        break;
      case Type::R_64:
        if (!get_operand<R64>(i).check()) {
          return false;
        }
        break;

      case Type::REL_8:
        if (!get_operand<Rel8>(i).check()) {
          return false;
        }
        break;
      case Type::REL_32:
        if (!get_operand<Rel32>(i).check()) {
          return false;
        }
        break;

      case Type::FS:
        if (!get_operand<Fs>(i).check()) {
          return false;
        }
        break;
      case Type::GS:
        if (!get_operand<Gs>(i).check()) {
          return false;
        }
        break;
      case Type::SREG:
        if (!get_operand<Sreg>(i).check()) {
          return false;
        }
        break;

      case Type::ST_0:
        if (!get_operand<St0>(i).check()) {
          return false;
        }
        break;
      case Type::ST:
        if (!get_operand<St>(i).check()) {
          return false;
        }
        break;

      case Type::XMM_0:
        if (!get_operand<Xmm0>(i).check()) {
          return false;
        }
        break;
      case Type::XMM:
        if (!get_operand<Ymm>(i).check()) {
          return false;
        }
        break;

      case Type::YMM:
        if (!get_operand<Ymm>(i).check()) {
          return false;
        }
        break;

      default:
        assert(false);
    }

  return true;
}

ostream& Instruction::write_att(ostream& os) const {
  assert((size_t)get_opcode() < att_.size());

  if (get_opcode() == LABEL_DEFN) {
    get_operand<Label>(0).write_att(os);
    os << ":";
    return os;
  }

  os << att_[get_opcode()] << " ";
  if (arity() > 0)
    for (int i = (int)arity() - 1; i >= 0; --i) {
      switch (type(i)) {
        case Type::HINT:
          get_operand<Hint>(i).write_att(os);
          break;
        case Type::IMM_8:
        case Type::IMM_16:
        case Type::IMM_32:
        case Type::IMM_64:
        case Type::ZERO:
        case Type::ONE:
        case Type::THREE:
          get_operand<Three>(i).write_att(os);
          break;

        case Type::LABEL:
          get_operand<Label>(i).write_att(os);
          break;

        case Type::M_8:
        case Type::M_16:
        case Type::M_32:
        case Type::M_64:
        case Type::M_128:
        case Type::M_256:
        case Type::M_16_INT:
        case Type::M_32_INT:
        case Type::M_64_INT:
        case Type::M_32_FP:
        case Type::M_64_FP:
        case Type::M_80_FP:
        case Type::M_80_BCD:
        case Type::M_2_BYTE:
        case Type::M_28_BYTE:
        case Type::M_108_BYTE:
        case Type::M_512_BYTE:
        case Type::FAR_PTR_16_16:
        case Type::FAR_PTR_16_32:
        case Type::FAR_PTR_16_64:
          get_operand<FarPtr1664>(i).write_att(os);
          break;

        case Type::MM:
          get_operand<Mm>(i).write_att(os);
          break;

        case Type::MOFFS_8:
        case Type::MOFFS_16:
        case Type::MOFFS_32:
        case Type::MOFFS_64:
          get_operand<Moffs64>(i).write_att(os);
          break;

        case Type::PREF_66:
          get_operand<Pref66>(i).write_att(os);
          break;
        case Type::PREF_REX_W:
          get_operand<PrefRexW>(i).write_att(os);
          break;
        case Type::FAR:
          get_operand<Far>(i).write_att(os);
          break;

        case Type::RH:
          get_operand<Rh>(i).write_att(os);
          break;
        case Type::RB:
          get_operand<Rb>(i).write_att(os);
          break;
        case Type::AL:
        case Type::CL:
        case Type::RL:
          get_operand<Rl>(i).write_att(os);
          break;
        case Type::AX:
        case Type::DX:
        case Type::R_16:
          get_operand<R16>(i).write_att(os);
          break;
        case Type::EAX:
        case Type::R_32:
          get_operand<R32>(i).write_att(os);
          break;
        case Type::RAX:
        case Type::R_64:
          get_operand<R64>(i).write_att(os);
          break;

        case Type::REL_8:
        case Type::REL_32:
          get_operand<Rel32>(i).write_att(os);
          break;

        case Type::FS:
        case Type::GS:
        case Type::SREG:
          get_operand<Sreg>(i).write_att(os);
          break;

        case Type::ST_0:
        case Type::ST:
          get_operand<St>(i).write_att(os);
          break;

        case Type::XMM_0:
        case Type::XMM:
          get_operand<Xmm>(i).write_att(os);
          break;

        case Type::YMM:
          get_operand<Ymm>(i).write_att(os);
          break;

        default:
          assert(false);
      }

      if (i != 0) {
        os << ", ";
      }
    }

  return os;
}

bool Instruction::operator<(const Instruction& rhs) const {
  if ( opcode_ != rhs.opcode_ )
    return opcode_ < rhs.opcode_;
  for ( size_t i = 0, ie = arity(); i < ie; ++i )
    switch ( type(i) ) {
      case Type::MOFFS_8:
      case Type::MOFFS_16:
      case Type::MOFFS_32:
      case Type::MOFFS_64:
        if ( operands_[i] != rhs.operands_[i] )
          return operands_[i] < rhs.operands_[i];
        break;
      default:
        if ( get_operand<R64>(i) != rhs.get_operand<R64>(i) )
          return get_operand<R64>(i) < rhs.get_operand<R64>(i);
        break;
    }
  return true;
}

bool Instruction::operator==(const Instruction& rhs) const {
  if ( opcode_ != rhs.opcode_ )
    return false;
  for ( size_t i = 0, ie = arity(); i < ie; ++i )
    switch ( type(i) ) {
      case Type::MOFFS_8:
      case Type::MOFFS_16:
      case Type::MOFFS_32:
      case Type::MOFFS_64:
        if ( operands_[i] != rhs.operands_[i] )
          return false;
        break;
      default:
        if ( get_operand<R64>(i) != rhs.get_operand<R64>(i) )
          return false;
        break;
    }
  return true;
}

size_t Instruction::hash() const {
  auto res = (size_t)opcode_;
  for ( size_t i = 0, ie = arity(); i < ie; ++i )
    switch ( type(i) ) {
      case Type::MOFFS_8:
      case Type::MOFFS_16:
      case Type::MOFFS_32:
      case Type::MOFFS_64:
        res ^= get_operand<Moffs8>(i).hash();
        break;
      default:
        res ^= get_operand<R64>(i).hash();
        break;
    }
  return res;
}

const array<size_t, 3803> Instruction::arity_ {{
  // Internal mnemonics
  1
  // Auto-generated mnemonics
  #include "src/arity.table"
}};

const array<array<Instruction::Properties, 4>, 3803> Instruction::properties_ {{
  // Internal mnemonics
  {Properties::none() + Property::MUST_READ, Properties::none(), Properties::none(), Properties::none()}
  // Auto-generated mnemonics
  #include "src/properties.table"
}};

const array<array<Type, 4>, 3803> Instruction::type_ {{
  // Internal mnemonics
  {{Type::LABEL}}
  // Auto-generated mnemonics
  #include "src/type.table"
}};

const array<int, 3803> Instruction::mem_index_ {{
  // Internal mnemonics
  -1
  // Auto-generated mnemonics
  #include "src/mem_index.table"
}};

const array<RegSet, 3803> Instruction::implicit_must_read_set_ {{
  // Internal mnemonics
  RegSet::empty()
  // Auto-generated mnemonics
  #include "src/must_read.table"
}};

const array<RegSet, 3803> Instruction::implicit_maybe_read_set_ {{
  // Internal mnemonics
  RegSet::empty()
  // Auto-generated mnemonics
  #include "src/maybe_read.table"
}};

const array<RegSet, 3803> Instruction::implicit_must_write_set_ {{
  // Internal mnemonics
  RegSet::empty()
  // Auto-generated mnemonics
  #include "src/must_write.table"
}};

const array<RegSet, 3803> Instruction::implicit_maybe_write_set_ {{
  // Internal mnemonics
  RegSet::empty()
  // Auto-generated mnemonics
  #include "src/maybe_write.table"
}};

const array<RegSet, 3803> Instruction::implicit_must_undef_set_ {{
  // Internal mnemonics
  RegSet::empty()
  // Auto-generated mnemonics
  #include "src/must_undef.table"
}};

const array<RegSet, 3803> Instruction::implicit_maybe_undef_set_ {{
  // Internal mnemonics
  RegSet::empty()
  // Auto-generated mnemonics
  #include "src/maybe_undef.table"
}};

const array<FlagSet, 3803> Instruction::flags_ {{
  // Internal mnemonics
  FlagSet::empty()
  // Auto-generatred mnemonics
  #include "src/flag.table"
}};

} // namespace x64asm
