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
#include "llvm/Transforms/Utils/ValueMapper.h"

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
        void duplicateInstr(Instruction* I, std::map<Instruction*,Instruction*> &duplicatedInstr, ValueToValueMapTy &vmap, std::vector<llvm::Instruction*> new_instructions){
            //depth first traversal of the producer tree
            
            //base case: no more producers or instuction in duplicated instr already
            if(duplicatedInstr.count(I) > 0){
                return;
            }

            for (Use &U : I->operands()) {
                Instruction *Inst = dyn_cast<Instruction>(U);
                if(nullptr == Inst){
                    continue;
                }
                I->print(errs());
                errs() << "-";
                Inst->print(errs());
                errs() << "\n";
                duplicateInstr(Inst,duplicatedInstr,vmap,new_instructions);

            }

            if(!isa<StoreInst>(I)){
                auto *clone_inst = I->clone();
                clone_inst->insertBefore(I);
                new_instructions.push_back(clone_inst);
                vmap[I] = clone_inst;
                llvm::RemapInstruction(clone_inst, vmap,
                         RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
                duplicatedInstr[I] = clone_inst;
            }

            return;
        }

        bool runOnFunction(Function &F) override
        {
            std::map<Instruction*,Instruction*> duplicatedInstr;
            ValueToValueMapTy vmap;
            std::vector<Instruction*> new_instructions;

            
            for (BasicBlock &BB : F)
            {
                errs() << BB.getName() << "\n";
                for (Instruction &I : BB)
                {
                    // I.print(errs());
                    // errs() << " - " << I.getNumOperands() << "\n";
                    if(isa<StoreInst>(I)){
                        duplicateInstr(&I,duplicatedInstr,vmap,new_instructions);
                    }
                }
            }
            return true;
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