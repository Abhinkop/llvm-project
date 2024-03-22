define i1 @src(i8 %a) {
entry:
  %conv = sext i8 %a to i32
  %0 = and i32 %conv, -2147483647
  %cmp = icmp eq i32 %0, 1
  ret i1 %cmp
}