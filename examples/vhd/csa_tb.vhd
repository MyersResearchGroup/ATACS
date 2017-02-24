library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity E is
end E;

architecture behavioral of E is

	signal x 	: std_logic;  --@ out
        signal b 	: std_logic;  --@ in
       	signal c	: std_logic;  --@ in

component csa 
  port(b : in  std_logic;
       c : in  std_logic;
       x : inout std_logic);
end component;

begin
   UUT : csa
	port map(b,c,x);
    proc1:process
    begin
        wait until x = '0';
        b <= '1' after delay(10,20);
        c <= '1' after delay(10,20);
        wait until x = '1';
        b <= '0' after delay(10,20);
        c <= '0' after delay(10,20);
    end process;
end behavioral;
