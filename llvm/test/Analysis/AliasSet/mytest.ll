define i1 @src1(i8 %a, i8 %b) {
entry:
  %conv_a = sext i8 %a to i32
  %conv_b = sext i8 %b to i32
  %0 = and i32 %conv_a, %conv_b
  %cmp = icmp eq i32 %0, 1
  ret i1 %cmp
}