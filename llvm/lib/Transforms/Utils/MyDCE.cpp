//===-- MyDCE.cpp - Example Transformations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/MyDCE.h"
#include "llvm/IR/Function.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Analysis/TargetLibraryInfo.h"

using namespace llvm;

bool deadCodeEliminationSinglePass(Function &F,
                                    FunctionAnalysisManager &AM) {
  auto *TLI = &AM.getResult<TargetLibraryAnalysis>(F);
  bool changed = false;
  for(auto &BB : F) {
    for(auto it = BB.begin(); it != BB.end(); ) {
      Instruction &I = *(it++);
      if(isInstructionTriviallyDead(&I, TLI)) {
        I.eraseFromParent();
        changed = true;
      }
    }
  }
  return changed;
}

PreservedAnalyses MyDCEPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  bool changed = false;
  while (deadCodeEliminationSinglePass(F, AM)) {
    changed = true;
  }
  // F.print(errs());
  
  if(changed) {
    PreservedAnalyses PA;
    PA.preserveSet<CFGAnalyses>();
    return PA;
  }
  return PreservedAnalyses::all();
}
