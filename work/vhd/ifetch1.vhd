library ieee;
use ieee.math_real.all;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.nondeterminism.all;

entity ifetch1 is
   
    port ( pc_req: in std_logic;
	   mem_req: out std_logic;
	   pc_ack: out std_logic;	   
	   mem_ack: in std_logic;
	   if_done_o: out std_logic;
	   if_done_i: in std_logic);
          
end ifetch1;

-- *[[pc_req]; (pc_ack+ || mem_req+); [~pc_req & mem_ack ];
--    if_done_o+; (pc_ack- || mem_req-); [if_done_i]; if_done_o-; 
--    [~if_done_i &~mem_ack] ];


architecture control of ifetch1 is

begin
    circuit:process
    begin
	wait until pc_req = '1';
	mem_req <= '1' after delay(2,5); 
	pc_ack <= '1' after delay(2,5);
	wait until mem_ack = '1' and pc_req = '0'; 
	if_done_o <= '1' after delay(2,5);
	mem_req <= '0' after delay(2,5);
	pc_ack <= '0' after delay(2,5); 
	wait until if_done_i = '1';
	if_done_o <= '0' after delay(2,5);
	wait until if_done_i = '0' and mem_ack = '0';
    end process;
end;


configuration cfg_ifetch_behavioral of ifetch1 is
    for behavioral
    end for;
end cfg_ifetch_behavioral;
