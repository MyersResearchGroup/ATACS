-- VHDL Test Bench Created from SGE Symbol synor.sym.sym -- Jan 31 10:30:50 1997

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use ieee.math_real.all;
use work.nondeterminism.all;


entity E is
end E;

Architecture A of E is

   signal       IN1 : std_logic;    --@ in
   signal       IN2 : std_logic;    --@ in
   signal        O : std_logic;

   component synor
      Port (      IN1 : In    std_logic;
                  IN2 : In    std_logic;
                   O : Out   std_logic);
   end component;

begin
   UUT : synor
      Port Map ( IN1, IN2, O );

-- *** Test Bench - User Defined Section ***
   TB : block
   begin
   p1:process
      variable z : integer; 
   begin
     z:=random(2);
     if(z = 1)then
       in1 <= '1' after delay(500,550);
       wait until in1 = '1';
       in1 <= '0' after delay(269,299);
       wait until in1 = '0';
     else
       in2 <= '1' after delay(500,550); 
       wait until in2 = '1';
       in2 <= '0' after delay(269,299);
       wait until in2 = '0';
     end if;
   end process;	

   end block;
-- *** End Test Bench - User Defined Section ***

end A;

configuration CFG_TB_SYNOR_BEHAVIORAL of E is
   for A
      for UUT : SYNOR
         use configuration WORK.CFG_SYNOR_SCHEMATIC;
      end for;

-- *** User Defined Configuration ***
--      for TB
--      end for;
-- *** End User Defined Configuration ***

   end for;
end CFG_TB_SYNOR_BEHAVIORAL;





