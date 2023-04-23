#ifndef LLVM_TRANSFORM_PFUNCTIONPASS_H
#define LLVM_TRANSFORM_PFUNCTIONPASS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace llvm
{
    class PFunctionPass : public PassInfoMixin<PFunctionPass>
    {
    private:
        /* data */
    public:
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM); 
        static bool isRequired() { return true; } 
        PFunctionPass(/* args */);
        ~PFunctionPass();
    };
    
    PFunctionPass::PFunctionPass(/* args */)
    {
    }
    
    PFunctionPass::~PFunctionPass()
    {
    }
    
} // namespace llvm



#endif