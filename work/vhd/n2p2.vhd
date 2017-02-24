library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;

entity n2p2 is
  port(a1n : in std_logic;
       b1n : in std_logic;
       a1p : in std_logic;
       b1p : in std_logic;
       o : inout std_logic :='0';
       obar : inout std_logic := '1');
end n2p2;


architecture behavior of n2p2 is 
  signal o_i : std_logic := '0';

begin
  o_i <= '1' after delay(1,2) when (a1n='1' and b1n='1') else
         '0' after delay(1,2) when (a1p='0' and b1p='0') else
          o_i;
  o <= o_i;
  obar <= not o_i after delay(0,1);

end behavior;

