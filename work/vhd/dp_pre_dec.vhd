library IEEE;
use IEEE.std_logic_1164.all;

entity dp_pre_dec is

    port( instr_in: in std_logic_vector (15 downto 0); 
	  pc_in: in std_logic_vector (15 downto 0);
	  latch_main: in std_logic;
	  latch_imm:   in std_logic;
	  multiword:  out std_logic;
	  exception:  out std_logic;
	  pc_out:     out std_logic_vector(15 downto 0);
	  instr_out:  out std_logic_vector(31 downto 0));
end dp_pre_dec;

architecture datapath of dp_pre_dec is

    signal curr_inst: std_logic_vector(15 downto 0);

begin

    pc_reg: process
    begin
	wait until latch_main'event and latch_main = '1';
	pc_out <= pc_in;
	curr_inst  <= instr_in;
    end process;
    
    imm_reg: process
    begin
	wait until latch_imm'event and latch_imm = '1';
	instr_out (31 downto 16) <= instr_in;  
    end process;

    instr_out (15 downto 0) <= curr_inst;       
    multword <= (curr_inst(15) and curr_inst(12) and curr_inst(0)) or
		(not curr_inst(15) and not curr_inst(14) 
		 and ((curr_inst(4) and not curr_inst(3) and not curr_inst(2)
		      and not curr_inst(1) and curr_inst(0)) 
		      or (curr_inst(12) and not curr_inst(11) and curr_inst(10) 
			  and not curr_inst(9))));

    exception <= not curr_inst(15) and curr_inst(14) and curr_inst(13) 
		 and curr_inst(12) and curr_inst(11) and not curr_inst(10)
		 and curr_inst(9) and not curr_inst(0);
end;

