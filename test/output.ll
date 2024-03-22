; ModuleID = 'input.ll'
source_filename = "input.ll"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define i1 @src(i8 %a) local_unnamed_addr #0 {
entry:
  %conv = sext i8 %a to i32
  %0 = and i32 %conv, -2147483647
  %cmp = icmp eq i32 %0, 1
  ret i1 %cmp
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
