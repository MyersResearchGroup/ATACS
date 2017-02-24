-- VHDL Model Created from SGE Symbol bcmp.sym -- Jul 30 08:53:58 1996

library IEEE;
--use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;

entity BCMP is
      Port (      I0 : In    std_logic;
                  I1 : In    std_logic;
                 REQ : In    std_logic;
                  EQ : Out   std_logic;
                  GT : Out   std_logic;
                  LT : Out   std_logic );
end BCMP;

architecture BEHAVIORAL of BCMP is

   begin
	process
	begin
          EQ <= '0' after 1 ns;
          GT <= '0' after 1 ns;
          LT <= '0' after 1 ns;

	  wait until REQ='1' or I0='1';
          if( I0 > I1 ) then
              EQ <= '0' after 1 ns;
              GT <= '1' after 1 ns;
              LT <= '0' after 1 ns;
          elsif( I0=I1 ) then
              EQ <= '1' after 1 ns;
              GT <= '0' after 1 ns;
              LT <= '0' after 1 ns;
	  elsif(I0 < I1) then
              EQ <= '0' after 1 ns;
   	      GT <= '0' after 1 ns;
	      LT <= '1' after 1 ns;
	  end if;

          wait until REQ='0';
          EQ <= '0' after 1 ns;
   	  GT <= '0' after 1 ns;
	  LT <= '0' after 1 ns;
   
	end process;

end BEHAVIORAL;

configuration CFG_BCMP_BEHAVIORAL of BCMP is
   for BEHAVIORAL

   end for;

end CFG_BCMP_BEHAVIORAL;
