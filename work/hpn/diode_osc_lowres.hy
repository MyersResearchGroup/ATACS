-- Data used to generate automaton:
-- Vcd Vcu Ild Ilu dVc dIl
-- 0 102 0 450 -2 3
-- 0 102 670 3820 -1 3
-- 0 102 4040 10330 5 2
-- 0 102 10550 11000 9 1
-- 204 1633 0 450 -9 2
-- 204 1633 670 3820 -7 2
-- 204 1633 4040 10330 -2 1
-- 204 1633 10550 11000 2 -1
-- 1735 4796 0 450 -3 -1
-- 1735 4796 670 3820 -1 -1
-- 1735 4796 4040 10330 4 -2
-- 1735 4796 10550 11000 8 -2
-- 4898 5000 0 450 -9 -2
-- 4898 5000 670 3820 -7 -2
-- 4898 5000 4040 10330 -2 -3
-- 4898 5000 10550 11000 2 -4

var
  Vc: analog;
  Il: analog;

automaton a1
synclabs:;
initially l0 & Il=1 & Vc=1;

loc ld0r0: while Il>0 & Il<=670 & Vc<=0 wait {dVc=0, dIl=3}
  when Il=670 goto ld0r1;
  when Vc>=0 goto l0;

loc ld0r1: while Il>=450 & Il<=4040 & Vc<=0 wait {dVc=0, dIl=3}
  when Il=450 goto ld0r0;
  when Il=4040 goto ld0r2;
  when Vc>=0 goto l1;

loc ld0r2: while Il>=3820 & Il<=10550 & Vc<=0 wait {dVc=5, dIl=2}
  when Il=3820 goto ld0r1;
  when Il=10550 goto ld0r3;
  when Vc>=0 goto l2;

loc ld0r3: while Il>=10330 & Vc<=0 wait {dVc=9, dIl=1}
  when Il=10330 goto ld0r2;
  when Vc>=0 goto l3;

loc ld1r0: while Il<=0 & Vc>0 & Vc<=204 wait {dVc=-2, dIl=3}
  when Il>=0 goto l0;
  when Vc=204 goto ld1r4;

loc ld1r4: while Il<=0 & Vc>=102 & Vc<=1735 wait {dVc=-9, dIl=2}
  when Il>=0 goto l4;
  when Vc=102 goto ld1r0;
  when Vc=1735 goto ld1r8;

loc ld1r8: while Il<=0 & Vc>=1633 & Vc<=4898 wait {dVc=-3, dIl=0}
  when Il>=0 goto l8;
  when Vc=1633 goto ld1r4;
  when Vc=4898 goto ld1r12;

loc ld1r12: while Il<=0 & Vc>=4796 wait {dVc=-9, dIl=0}
  when Il>=0 goto l12;
  when Vc=4796 goto ld1r8;

loc l0: while Il>=0 & Il<=670 & Vc>=0 & Vc<=204 wait {dVc=-2, dIl=3}
  when Il=0 goto ld1r0;
  when Il=670 goto l1;
  when Vc=0 goto ld0r0;
  when Vc=204 goto l4;

loc l1: while Il>=450 & Il<=4040 & Vc>=0 & Vc<=204 wait {dVc=-1, dIl=3}
  when Il=450 goto l0;
  when Il=4040 goto l2;
  when Vc=0 goto ld0r1;
  when Vc=204 goto l5;

loc l2: while Il>=3820 & Il<=10550 & Vc>=0 & Vc<=204 wait {dVc=5, dIl=2}
  when Il=3820 goto l1;
  when Il=10550 goto l3;
  when Vc=0 goto ld0r2;
  when Vc=204 goto l6;

loc l3: while Il>=10330 & Vc>=0 & Vc<=204 wait {dVc=9, dIl=1}
  when Il=10330 goto l2;
  when Vc=0 goto ld0r3;
  when Vc=204 goto l7;

loc l4: while Il>=0 & Il<=670 & Vc>=102 & Vc<=1735 wait {dVc=-9, dIl=2}
  when Il=0 goto ld1r4;
  when Il=670 goto l5;
  when Vc=102 goto l0;
  when Vc=1735 goto l8;

loc l5: while Il>=450 & Il<=4040 & Vc>=102 & Vc<=1735 wait {dVc=-7, dIl=2}
  when Il=450 goto l4;
  when Il=4040 goto l6;
  when Vc=102 goto l1;
  when Vc=1735 goto l9;

loc l6: while Il>=3820 & Il<=10550 & Vc>=102 & Vc<=1735 wait {dVc=-2, dIl=1}
  when Il=3820 goto l5;
  when Il=10550 goto l7;
  when Vc=102 goto l2;
  when Vc=1735 goto l10;

loc l7: while Il>=10330 & Vc>=102 & Vc<=1735 wait {dVc=2, dIl=-1}
  when Il=10330 goto l6;
  when Vc=102 goto l3;
  when Vc=1735 goto l11;

loc l8: while Il>=0 & Il<=670 & Vc>=1633 & Vc<=4898 wait {dVc=-3, dIl=-1}
  when Il=0 goto ld1r8;
  when Il=670 goto l9;
  when Vc=1633 goto l4;
  when Vc=4898 goto l12;

loc l9: while Il>=450 & Il<=4040 & Vc>=1633 & Vc<=4898 wait {dVc=-1, dIl=-1}
  when Il=450 goto l8;
  when Il=4040 goto l10;
  when Vc=1633 goto l5;
  when Vc=4898 goto l13;

loc l10: while Il>=3820 & Il<=10550 & Vc>=1633 & Vc<=4898 wait {dVc=4, dIl=-2}
  when Il=3820 goto l9;
  when Il=10550 goto l11;
  when Vc=1633 goto l6;
  when Vc=4898 goto l14;

loc l11: while Il>=10330 & Vc>=1633 & Vc<=4898 wait {dVc=8, dIl=-2}
  when Il=10330 goto l10;
  when Vc=1633 goto l7;
  when Vc=4898 goto l15;

loc l12: while Il>=0 & Il<=670 & Vc>=4796 wait {dVc=-9, dIl=-2}
  when Il=0 goto ld1r12;
  when Il=670 goto l13;
  when Vc=4796 goto l8;

loc l13: while Il>=450 & Il<=4040 & Vc>=4796 wait {dVc=-7, dIl=-2}
  when Il=450 goto l12;
  when Il=4040 goto l14;
  when Vc=4796 goto l9;

loc l14: while Il>=3820 & Il<=10550 & Vc>=4796 wait {dVc=-2, dIl=-3}
  when Il=3820 goto l13;
  when Il=10550 goto l15;
  when Vc=4796 goto l10;

loc l15: while Il>=10330 & Vc>=4796 wait {dVc=2, dIl=-4}
  when Il=10330 goto l14;
  when Vc=4796 goto l11;

end

automaton propa
synclabs:;
initially aLow;

loc aLow: while Il>=3000 wait {}
  when Il=3000 goto aHigh;

loc aHigh: while Il<=4000 wait {}
  when Il=4000 goto aLow;
end

automaton propb
synclabs:;
initially bLow;
loc bLow: while Il<=8000 wait {}
  when Il=8000 goto bHigh;

loc bHigh: while Il>=7000 wait {}
  when Il=7000 goto bLow;
end


var f_reachable, b1_reachable, b2_reachable, init_reg: region;
 
init_reg := loc[a1] = l10 & Il <= 5000 & Il >= 4000 & Vc <= 4700 & Vc >= 4000 & loc[propa] = aLow & loc[propb] = bLow;
 
b1_reachable := reach backward from loc[propa] = aLow endreach;
b2_reachable := reach backward from loc[propb] = bLow endreach; 
f_reachable := reach forward from init_reg endreach;

if f_reachable <= b1_reachable and f_reachable <= b2_reachable
  then prints "Requirement Satisfied";
  else prints "Requirement Not Satisfied"";
endif;
