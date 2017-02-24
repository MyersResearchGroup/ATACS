N F T1 b+/1
N F T2 b-/1
N F T3 c+/1
N F T4 c-/1
N F T5 x+/1
N F T6 x-/1
N F T7 $0+_csa_uut/1
N F T8 $1+_csa_uut/1
N F T9 $3+/1
N F T10 $2+/1
N E P1 [0,0][((b='0'_uut)&(c='0'_uut))&(~((b='1'_uut)&(c='1'_uut)))]d
E P T5 0 P1 0
E P P1 0 T8 0
N M P2 [0,0][(b='1'_uut)&(c='1'_uut)]d
E P T6 0 P2 0
E P P2 0 T7 0
N E P3 [0,inf]
E P T7 0 P3 0
E P P3 0 T5 0
N E P4 [0,inf]
E P T8 0 P4 0
E P P4 0 T6 0
N M P5 [10,20]
E P T9 0 P5 0
E P P5 0 T2 0
N M P6 [10,20]
E P T9 0 P6 0
E P P6 0 T4 0
N M P7 [0,0][x='0']
E P T9 0 P7 0
E P P7 0 T10 0
N M P8 [10,20]
E P T10 0 P8 0
E P P8 0 T1 0
N M P9 [10,20]
E P T10 0 P9 0
E P P9 0 T3 0
N M P10 [0,0][x='1']
E P T10 0 P10 0
E P P10 0 T9 0
ZZZZZ