//===----------------------------------------------------------------------===//
//
//  INSERT DESCRIPTION OF FILE HERE
//
////===----------------------------------------------------------------------===//
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"

#include <map>
#include <string>
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "tfr"

namespace TFR
{
    struct TFRPass : public FunctionPass
    {
        static char ID;
        TFRPass() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override
        {

            for (BasicBlock &BB : F)
            {
                errs() << BB.getName() << "\n";
                for (Instruction &I : BB)
                {
                    errs() << I.getOpcodeName() << '\n';
                }
            }
            return false;
        }

        void getAnalysisUsage(AnalysisUsage &AU) const override
        {
            AU.addRequired<BranchProbabilityInfoWrapperPass>();
            AU.addRequired<BlockFrequencyInfoWrapperPass>();
            AU.addRequired<LoopInfoWrapperPass>();
        }

    private:
        static int dummyVar;
    };
} // namespace ISPRE

char TFR::TFRPass::ID = 0;
static RegisterPass<TFR::TFRPass>
    X("tfr", "Transient Fault Resiliency using Instruction Duplication", false, false);