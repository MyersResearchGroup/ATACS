library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity csa is
  port(b : in  std_logic;
       c : in  std_logic;
       x : inout std_logic);
end csa;


architecture behavioral of csa is
begin
	x <= '1' when b = '1' and c = '1' else
	     '0' when b = '0' and c = '0' else
	     unaffected;
end behavioral;
