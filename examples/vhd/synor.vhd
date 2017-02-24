-- VHDL Model Created from SGE Schematic synor.sch -- Jan 31 10:38:47 1997

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity SYNOR is
      Port (      in1 : in    std_logic;
                  in2 : in    std_logic;
                   O : Out   std_logic );
end SYNOR;

architecture SCHEMATIC of SYNOR is


   component anor
      Port (      in1 : in    std_logic;
                  in2 : in    std_logic;
                   O : Out   std_logic );
   end component;

begin

   I_1 : anor
      Port Map ( in1=>in1, in2=>in2, O=>O );

end SCHEMATIC;

configuration CFG_SYNOR_SCHEMATIC of SYNOR is

   for SCHEMATIC
      for I_1: ANOR
         use configuration WORK.CFG_ANOR_BEHAVIORAL;
      end for;
   end for;

end CFG_SYNOR_SCHEMATIC;
