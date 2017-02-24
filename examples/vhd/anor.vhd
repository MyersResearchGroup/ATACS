-- VHDL Model Created from SGE Symbol anor.sym -- Jan 31 10:30:50 1997

library IEEE;
use ieee.math_real.all;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.nondeterminism.all;

entity ANOR is
      Port (      in1 : in    std_logic;
                  in2 : in    std_logic;
                   O : Out   std_logic );
end ANOR;

architecture BEHAVIORAL of ANOR is
     signal   x          : bit	  := '1'; 
   begin
     process
     begin
       wait until in1='1' or in2='1';
       if (in1='1') then  
	 o <= '1' after delay(201,221);
         wait until o='1';
         x <= '0' after delay(99,109);
         wait until x='0';
	 o <= '0' after delay(199,219);
         wait until o='0';
         x <= '1' after delay(101,111);
         wait until x='1';
       elsif (in2='1') then	    
	 o <= '1' after delay(201,221);
         wait until o='1';
         x <= '0' after delay(99,109);
         wait until x='0';
         o <= '0' after delay(199,219);
         wait until o='0';
         x <= '1' after delay(101,111);
         wait until x='1';
       end if;
    end process;
end BEHAVIORAL;

configuration CFG_ANOR_BEHAVIORAL of ANOR is
   for BEHAVIORAL

   end for;

end CFG_ANOR_BEHAVIORAL;
