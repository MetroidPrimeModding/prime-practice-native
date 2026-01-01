#include "utils/HookStubConstants.h"

  .text
  .globl HookStub
  .type HookStub, @function
HookStub:
  // Create stack frame
  stwu %r1, -HOOK_FRAME_SIZE(%r1)
  // Save GPR0
  stw %r0, HOOK_REGS_OFFSET + HOOK_REG_GPR_OFFSET(%r1)
  // Save stack pointer (we do not restore it from here, but this way the c++ gets it correctly)
  stw %r1, HOOK_REGS_OFFSET + HOOK_REG_GPR_OFFSET + 4(%r1)
  // save LR to restore later
  mflr %r0
  stw %r0, HOOK_REGS_OFFSET + HOOK_REG_LR_OFFSET(%r1)

  //set HOOK_REG_EARLY_RET_OFFSET to 0
  li %r0, 0
  stw %r0, HOOK_REGS_OFFSET + HOOK_REG_EARLY_RET_OFFSET(%r1)

  // Save LR, CR, XER, CTR
  mflr %r0
  stw %r0, HOOK_REGS_OFFSET + HOOK_REG_LR_OFFSET(%r1)
  mfcr %r0
  stw %r0, HOOK_REGS_OFFSET + HOOK_REG_CR_OFFSET(%r1)
  mfspr %r0, 1
  stw %r0, HOOK_REGS_OFFSET + HOOK_REG_XER_OFFSET(%r1)
  mfspr %r0, 9
  stw %r0, HOOK_REGS_OFFSET + HOOK_REG_CTR_OFFSET(%r1)

  // Save GPRs 2-31
  stmw %r2, HOOK_REGS_OFFSET + HOOK_REG_GPR_OFFSET + 8(%r1)

  // Save FPRs
  stfd %f0, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 0(%r1)
  stfd %f1, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 8(%r1)
  stfd %f2, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 16(%r1)
  stfd %f3, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 24(%r1)
  stfd %f4, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 32(%r1)
  stfd %f5, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 40(%r1)
  stfd %f6, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 48(%r1)
  stfd %f7, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 56(%r1)
  stfd %f8, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 64(%r1)
  stfd %f9, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 72(%r1)
  stfd %f10, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 80(%r1)
  stfd %f11, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 88(%r1)
  stfd %f12, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 96(%r1)
  stfd %f13, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 104(%r1)
  stfd %f14, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 112(%r1)
  stfd %f15, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 120(%r1)
  stfd %f16, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 128(%r1)
  stfd %f17, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 136(%r1)
  stfd %f18, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 144(%r1)
  stfd %f19, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 152(%r1)
  stfd %f20, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 160(%r1)
  stfd %f21, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 168(%r1)
  stfd %f22, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 176(%r1)
  stfd %f23, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 184(%r1)
  stfd %f24, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 192(%r1)
  stfd %f25, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 200(%r1)
  stfd %f26, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 208(%r1)
  stfd %f27, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 216(%r1)
  stfd %f28, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 224(%r1)
  stfd %f29, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 232(%r1)
  stfd %f30, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 240(%r1)
  stfd %f31, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 248(%r1)

  // Call hook function
  // arg1 = pointer to HookRegisters struct
  // address is stored in HOOK_REG_TARGET_OFFSET
  addi %r3, %r1, HOOK_REGS_OFFSET
  lwz %r0, HOOK_REGS_OFFSET + HOOK_REG_TARGET_OFFSET(%r1)
  mtctr %r0
  bctrl

  // Restore fp registers
  lfd %f0, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 0(%r1)
  lfd %f1, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 8(%r1)
  lfd %f2, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 16(%r1)
  lfd %f3, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 24(%r1)
  lfd %f4, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 32(%r1)
  lfd %f5, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 40(%r1)
  lfd %f6, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 48(%r1)
  lfd %f7, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 56(%r1)
  lfd %f8, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 64(%r1)
  lfd %f9, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 72(%r1)
  lfd %f10, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 80(%r1)
  lfd %f11, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 88(%r1)
  lfd %f12, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 96(%r1)
  lfd %f13, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 104(%r1)
  lfd %f14, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 112(%r1)
  lfd %f15, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 120(%r1)
  lfd %f16, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 128(%r1)
  lfd %f17, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 136(%r1)
  lfd %f18, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 144(%r1)
  lfd %f19, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 152(%r1)
  lfd %f20, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 160(%r1)
  lfd %f21, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 168(%r1)
  lfd %f22, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 176(%r1)
  lfd %f23, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 184(%r1)
  lfd %f24, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 192(%r1)
  lfd %f25, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 200(%r1)
  lfd %f26, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 208(%r1)
  lfd %f27, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 216(%r1)
  lfd %f28, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 224(%r1)
  lfd %f29, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 232(%r1)
  lfd %f30, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 240(%r1)
  lfd %f31, HOOK_REGS_OFFSET + HOOK_REG_FPR_OFFSET + 248(%r1)

  // Restore GPRs 2-31
  lmw %r2, HOOK_REGS_OFFSET + HOOK_REG_GPR_OFFSET + 8(%r1)

  // Restore LR
  lwz %r0, HOOK_REGS_OFFSET + HOOK_REG_LR_OFFSET(%r1)
  mtlr %r0

  // load the trampoline address into CTR
  lwz %r0,  HOOK_REGS_OFFSET + HOOK_REG_TRAMPOLINE_OFFSET(%r1)
  mtctr %r0
  // Check for early return
  lwz %r0, HOOK_REGS_OFFSET + HOOK_REG_EARLY_RET_OFFSET(%r1)
  cmpwi %r0, 0
  beq 1f
  // Early return requested, use saved LR to return to original caller
  // if not, CTR contains the trampoline address
  mflr %r0
  mtctr %r0
1:
  // Restore CR and XER
  lwz %r0, HOOK_REGS_OFFSET + HOOK_REG_CR_OFFSET(%r1)
  mtcrf 0xFF, %r0
  lwz %r0, HOOK_REGS_OFFSET + HOOK_REG_XER_OFFSET(%r1)
  mtspr 1, %r0
  // Restore GPR0 and stack frame
  lwz %r0, HOOK_REGS_OFFSET + HOOK_REG_GPR_OFFSET(%r1)
  addi %r1, %r1, HOOK_FRAME_SIZE

  bctr

  .size HookStub, . - HookStub
