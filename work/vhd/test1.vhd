library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use ieee.math_real.all;
use work.nondeterminism.all;

entity E is
end E;

architecture A of E is

    signal pc_req: std_logic; --@ in
    signal mem_req: std_logic;
    signal pc_ack: std_logic; 
    signal mem_ack: std_logic; --@ in
    signal if_done_o: std_logic;
    signal if_done_i: std_logic; --@ in

    component ifetch1
	port( pc_req: in std_logic;
	      mem_req: out std_logic;
	      pc_ack: out std_logic;
	      mem_ack: in std_logic;
	      if_done_o: out std_logic; 
	      if_done_i: in std_logic);
    end component;

begin UUT: ifetch1
	  Port Map(pc_req, mem_req, pc_ack, mem_ack, if_done_o, if_done_i);
      
	  TB: block
	  begin
	      mem_delay:process
	      begin
		  wait until mem_req = '1';
		  mem_ack <= '1' after delay(10, 15);
		  wait until mem_req = '0';
		  mem_ack <= '0' after delay(2,5);
	      end process;

	      main_control: process
	      begin
		  pc_req <= '1' after delay(2,5);
		  wait until pc_ack = '1';
		  pc_req <= '0' after delay(2,5);
		  wait until pc_ack = '0' and if_done_o = '1';
		  if_done_i <= '1' after delay(2,5);
		  wait until if_done_o = '0';
		  if_done_i <= '0' after delay(2,5);
	      end process;
	  end block;
end A;

configuration cfg_tb_ifetch_behavioral of E is
    for A
	for UUT: ifetch
	    use configuration work.cfg_ifetch_behavioral;
	end for;
    end for;
end cfg_tb_ifetch_behavioral;




