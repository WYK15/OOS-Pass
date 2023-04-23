#include "PFunctionPass.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Error.h"

#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"

using namespace llvm;

PreservedAnalyses PFunctionPass::run(Function &F, FunctionAnalysisManager &AM) {
    errs() << F.getName() << "\n";
    for(BasicBlock &BB : F) {
        for(auto &ins : BB) {
            errs() << ins << "\n";
        }
    }
    return PreservedAnalyses::all();
}


//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
// https://llvm.org/docs/WritingAnLLVMNewPMPass.html
llvm::PassPluginLibraryInfo getPFunctionPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PFunctionPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "leo-test-pass") {
                    FPM.addPass(PFunctionPass());
                    return true;
                  }
                  return false;
                });
          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize HelloWorld when added to the pass pipeline on the
// command line, i.e. via '-passes=hello-world'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getPFunctionPassPluginInfo();
}
