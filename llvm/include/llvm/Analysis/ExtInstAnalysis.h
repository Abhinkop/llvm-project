//===- llvm/Analysis/AliasAnalysis.h - Alias Analysis Interface -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the generic AliasAnalysis interface, which is used as the
// common interface used by all clients of alias analysis information, and
// implemented by all alias analysis implementations.  Mod/Ref information is
// also captured by this interface.
//
// Implementations of this interface must implement the various virtual methods,
// which automatically provides functionality for the entire suite of client
// APIs.
//
// This API identifies memory regions with the MemoryLocation class. The pointer
// component specifies the base memory address of the region. The Size specifies
// the maximum size (in address units) of the memory region, or
// MemoryLocation::UnknownSize if the size is not known. The TBAA tag
// identifies the "type" of the memory reference; see the
// TypeBasedAliasAnalysis class for details.
//
// Some non-obvious details include:
//  - Pointers that point to two completely different objects in memory never
//    alias, regardless of the value of the Size component.
//  - NoAlias doesn't imply inequal pointers. The most obvious example of this
//    is two pointers to constant memory. Even if they are equal, constant
//    memory is never stored to, so there will never be any dependencies.
//    In this and other situations, the pointers may be both NoAlias and
//    MustAlias at the same time. The current API can only return one result,
//    though this is rarely a problem in practice.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_EXTINSTANALYSIS_H
#define LLVM_ANALYSIS_EXTINSTANALYSIS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include <set>

namespace llvm {

class CastInst;
class Function;
class Module;
class raw_ostream;

class ExtInstAResult {
  llvm::Function *Function{nullptr};
  std::set<CastInst *> SetOfExtInst;

public:
  ExtInstAResult(llvm::Function &Func,
                 std::set<llvm::CastInst *> &&SetOfExtInst);

  llvm::Function *function() const;
  const std::set<CastInst *> &getExtInsts() const;
  void print(llvm::raw_ostream &Os) const;
};

/// A manager for alias analyses.
///
/// This class can have analyses registered with it and when run, it will run
/// all of them and aggregate their results into single AA results interface
/// that dispatches across all of the alias analysis results available.
///
/// Note that the order in which analyses are registered is very significant.
/// That is the order in which the results will be aggregated and queried.
///
/// This manager effectively wraps the AnalysisManager for registering alias
/// analyses. When you register your alias analysis with this manager, it will
/// ensure the analysis itself is registered with its AnalysisManager.
///
/// The result of this analysis is only invalidated if one of the particular
/// aggregated AA results end up being invalidated. This removes the need to
/// explicitly preserve the results of `ExtInstAnalysis`. Note that analyses
/// should no longer be registered once the `ExtInstAnalysis` is run.
class ExtInstAnalysis : public AnalysisInfoMixin<ExtInstAnalysis> {
public:
  using Result = ExtInstAResult;

  Result run(llvm::Function &func, llvm::FunctionAnalysisManager &manager);

private:
  friend AnalysisInfoMixin<ExtInstAnalysis>;

  static AnalysisKey Key;
};

class ExtInstAnalysisPrinter
    : public llvm::PassInfoMixin<ExtInstAnalysisPrinter> {
  llvm::raw_ostream &Os;

public:
  explicit ExtInstAnalysisPrinter(llvm::raw_ostream &Os);
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &manager);
};

} // namespace llvm

#endif // LLVM_ANALYSIS_EXTINSTANALYSIS_H
