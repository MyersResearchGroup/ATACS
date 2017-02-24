-- Data used to generate automaton:
-- Vcd Vcu Ild Ilu dVc dIl
-- 0 10200 0 4500 -207 29
-- 0 10200 6700 38200 -5 25
-- 0 10200 40400 103300 489 15
-- 0 10200 105500 110000 848 8
-- 20400 163300 0 4500 -875 20
-- 20400 163300 6700 38200 -673 16
-- 20400 163300 40400 103300 -179 6
-- 20400 163300 105500 110000 180 -1
-- 173500 479600 0 4500 -287 -3
-- 173500 479600 6700 38200 -85 -7
-- 173500 479600 40400 103300 409 -17
-- 173500 479600 105500 110000 768 -24
-- 489800 500000 0 4500 -893 -20
-- 489800 500000 6700 38200 -690 -24
-- 489800 500000 40400 103300 -197 -34
-- 489800 500000 105500 110000 163 -41

var
  Vc: analog;
  Il: analog;

automaton a1
synclabs:;
initially l0 & Il=1 & Vc=1;

loc ld0r0: while Il>0 & Il<=6700 & Vc<=0 wait {dVc=0, dIl=29}
  when Il=6700 goto ld0r1;
  when Vc>=0 goto l0;

loc ld0r1: while Il>=4500 & Il<=40400 & Vc<=0 wait {dVc=0, dIl=25}
  when Il=4500 goto ld0r0;
  when Il=40400 goto ld0r2;
  when Vc>=0 goto l1;

loc ld0r2: while Il>=38200 & Il<=105500 & Vc<=0 wait {dVc=489, dIl=15}
  when Il=38200 goto ld0r1;
  when Il=105500 goto ld0r3;
  when Vc>=0 goto l2;

loc ld0r3: while Il>=103300 & Vc<=0 wait {dVc=848, dIl=8}
  when Il=103300 goto ld0r2;
  when Vc>=0 goto l3;

loc ld1r0: while Il<=0 & Vc>0 & Vc<=20400 wait {dVc=-207, dIl=29}
  when Il>=0 goto l0;
  when Vc=20400 goto ld1r4;

loc ld1r4: while Il<=0 & Vc>=10200 & Vc<=173500 wait {dVc=-875, dIl=20}
  when Il>=0 goto l4;
  when Vc=10200 goto ld1r0;
  when Vc=173500 goto ld1r8;

loc ld1r8: while Il<=0 & Vc>=163300 & Vc<=489800 wait {dVc=-287, dIl=0}
  when Il>=0 goto l8;
  when Vc=163300 goto ld1r4;
  when Vc=489800 goto ld1r12;

loc ld1r12: while Il<=0 & Vc>=479600 wait {dVc=-893, dIl=0}
  when Il>=0 goto l12;
  when Vc=479600 goto ld1r8;

loc l0: while Il>=0 & Il<=6700 & Vc>=0 & Vc<=20400 wait {dVc=-207, dIl=29}
  when Il=0 goto ld1r0;
  when Il=6700 goto l1;
  when Vc=0 goto ld0r0;
  when Vc=20400 goto l4;

loc l1: while Il>=4500 & Il<=40400 & Vc>=0 & Vc<=20400 wait {dVc=-5, dIl=25}
  when Il=4500 goto l0;
  when Il=40400 goto l2;
  when Vc=0 goto ld0r1;
  when Vc=20400 goto l5;

loc l2: while Il>=38200 & Il<=105500 & Vc>=0 & Vc<=20400 wait {dVc=489, dIl=15}
  when Il=38200 goto l1;
  when Il=105500 goto l3;
  when Vc=0 goto ld0r2;
  when Vc=20400 goto l6;

loc l3: while Il>=103300 & Vc>=0 & Vc<=20400 wait {dVc=848, dIl=8}
  when Il=103300 goto l2;
  when Vc=0 goto ld0r3;
  when Vc=20400 goto l7;

loc l4: while Il>=0 & Il<=6700 & Vc>=10200 & Vc<=173500 wait {dVc=-875, dIl=20}
  when Il=0 goto ld1r4;
  when Il=6700 goto l5;
  when Vc=10200 goto l0;
  when Vc=173500 goto l8;

loc l5: while Il>=4500 & Il<=40400 & Vc>=10200 & Vc<=173500 wait {dVc=-673, dIl=16}
  when Il=4500 goto l4;
  when Il=40400 goto l6;
  when Vc=10200 goto l1;
  when Vc=173500 goto l9;

loc l6: while Il>=38200 & Il<=105500 & Vc>=10200 & Vc<=173500 wait {dVc=-179, dIl=6}
  when Il=38200 goto l5;
  when Il=105500 goto l7;
  when Vc=10200 goto l2;
  when Vc=173500 goto l10;

loc l7: while Il>=103300 & Vc>=10200 & Vc<=173500 wait {dVc=180, dIl=-1}
  when Il=103300 goto l6;
  when Vc=10200 goto l3;
  when Vc=173500 goto l11;

loc l8: while Il>=0 & Il<=6700 & Vc>=163300 & Vc<=489800 wait {dVc=-287, dIl=-3}
  when Il=0 goto ld1r8;
  when Il=6700 goto l9;
  when Vc=163300 goto l4;
  when Vc=489800 goto l12;

loc l9: while Il>=4500 & Il<=40400 & Vc>=163300 & Vc<=489800 wait {dVc=-85, dIl=-7}
  when Il=4500 goto l8;
  when Il=40400 goto l10;
  when Vc=163300 goto l5;
  when Vc=489800 goto l13;

loc l10: while Il>=38200 & Il<=105500 & Vc>=163300 & Vc<=489800 wait {dVc=409, dIl=-17}
  when Il=38200 goto l9;
  when Il=105500 goto l11;
  when Vc=163300 goto l6;
  when Vc=489800 goto l14;

loc l11: while Il>=103300 & Vc>=163300 & Vc<=489800 wait {dVc=768, dIl=-24}
  when Il=103300 goto l10;
  when Vc=163300 goto l7;
  when Vc=489800 goto l15;

loc l12: while Il>=0 & Il<=6700 & Vc>=479600 wait {dVc=-893, dIl=-20}
  when Il=0 goto ld1r12;
  when Il=6700 goto l13;
  when Vc=479600 goto l8;

loc l13: while Il>=4500 & Il<=40400 & Vc>=479600 wait {dVc=-690, dIl=-24}
  when Il=4500 goto l12;
  when Il=40400 goto l14;
  when Vc=479600 goto l9;

loc l14: while Il>=38200 & Il<=105500 & Vc>=479600 wait {dVc=-197, dIl=-34}
  when Il=38200 goto l13;
  when Il=105500 goto l15;
  when Vc=479600 goto l10;

loc l15: while Il>=103300 & Vc>=479600 wait {dVc=163, dIl=-41}
  when Il=103300 goto l14;
  when Vc=479600 goto l11;

end

automaton propa
synclabs:;
initially aLow;

loc aLow: while Il>=30000 wait {}
  when Il=30000 goto aHigh;

loc aHigh: while Il<=40000 wait {}
  when Il=40000 goto aLow;
end

automaton propb
synclabs:;
initially bLow;
loc bLow: while Il<=80000 wait {}
  when Il=80000 goto bHigh;

loc bHigh: while Il>=70000 wait {}
  when Il=70000 goto bLow;
end


var f_reachable, b1_reachable, b2_reachable, init_reg: region;
 
init_reg := loc[a1] = l10 & Il <= 50000 & Il >= 40000 & Vc <= 470000 & Vc >= 400000 & loc[propa] = aLow & loc[propb] = bLow;
 
f_reachable := reach forward from init_reg endreach;
b1_reachable := reach backward from loc[propa] = aLow endreach;
b2_reachable := reach backward from loc[propb] = bLow endreach; 

if f_reachable <= b1_reachable and f_reachable <= b2_reachable
  then prints "Requirement Satisfied";
  else prints "Requirement Not Satisfied"";
endif;

