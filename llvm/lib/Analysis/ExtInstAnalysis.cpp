#include "llvm/Analysis/ExtInstAnalysis.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include "llvm/IR/Value.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>

#define PASS_NAME "ext-inst-analysis"
#define PASS_DESCRIPTION "Find interget bit extension instructions"
#define DEBUG_TYPE PASS_NAME

using namespace llvm;

ExtInstAResult::ExtInstAResult(llvm::Function &Func,
                               std::set<llvm::CastInst *> &&SetOfExtInst)
    : Function(&Func), SetOfExtInst(std::move(SetOfExtInst)) {}

llvm::Function *ExtInstAResult::function() const { return Function; }

const std::set<llvm::CastInst *> &ExtInstAResult::getExtInsts() const {
  return SetOfExtInst;
}

void ExtInstAResult::print(llvm::raw_ostream &Os) const {
  if (!SetOfExtInst.empty()) {
    // Using a ModuleSlotTracker for printing makes it so full function
    // analysis for slot numbering only occurs once instead of every time an
    // instruction is printed.
    llvm::ModuleSlotTracker slotTracker{Function->getParent()};
    Os << "ext instructions in \"" << Function->getName() << "\":\n";
    for (llvm::CastInst *extinst : SetOfExtInst) {
      extinst->print(Os, slotTracker);
      Os << '\t';

        extinst->getOperand(0)->print(Os, slotTracker);
        Os << "  ";
        assert(extinst->hasName());
        Os << extinst->getDestTy() << "  ";
        Os << extinst->getName();
      Os << '\n';
    }
  }
}

llvm::AnalysisKey ExtInstAnalysis::Key{};

ExtInstAnalysis::Result
ExtInstAnalysis::run(llvm::Function &func,
                     llvm::FunctionAnalysisManager &manager) {
  std::set<llvm::CastInst *> temp;
  for (llvm::BasicBlock &block : func) {
    for (llvm::Instruction &inst : block) {
      if (auto *fext = llvm::dyn_cast<llvm::CastInst>(&inst)) {
        if (fext->isIntegerCast()) {
          temp.insert(fext);
        }
      }
    }
  }
  ExtInstAResult result{func, std::move(temp)};
  return result;
}

ExtInstAnalysisPrinter::ExtInstAnalysisPrinter(llvm::raw_ostream &os)
    : Os(os) {}

llvm::PreservedAnalyses
ExtInstAnalysisPrinter::run(llvm::Function &func,
                            llvm::FunctionAnalysisManager &manager) {
  manager.getResult<ExtInstAnalysis>(func).print(Os);
  return llvm::PreservedAnalyses::all();
}
