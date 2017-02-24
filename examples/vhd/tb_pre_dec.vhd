--library IEEE;
--use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;
--use ieee.math_real.all;
--use work.nondeterminism.all;

entity E is
end E;

architecture A of E is

    signal pre_dec_req: 	std_logic; --@ in
    signal multiword_in0:	std_logic; --@ in
    signal multiword_in1:	std_logic; --@ in
    signal multiword_out:	std_logic;
    signal pre_dec_ack:		std_logic;
    signal word2_i:		std_logic;  --@ in
    signal exception_in0: 	std_logic;  --@ in
    signal exception_in1: 	std_logic;   --@ in
    signal exception_out: 	std_logic;

    component pre_dec
         port (pre_dec_req: 		in std_logic; 
	       multiword_in0:		in std_logic;  
	       multiword_in1: 		in std_logic;  
	       multiword_out: 		out std_logic;
	       pre_dec_ack:		out std_logic;
	       word2_i:			in std_logic;  
	       exception_in0:		in std_logic;  
	       exception_in1:      	in std_logic; 
	       exception_out:		out std_logic);		  
    end component;

begin 
    UUT: pre_dec
	Port Map(pre_dec_req, multiword_in0, multiword_in1, multiword_out, pre_dec_ack,
		 word2_i, exception_in0, exception_in1, exception_out);	 
	TB: block
	begin
	    p1:process
		variable z: integer;
	    begin
		z:=random(4);
		if(z = 1) then
		    exception_in0 <= '1' after delay(10,15);
		    multiword_in0 <= '1' after delay(10,15);	
		    pre_dec_req <= '1' after delay(2,5);	
		    wait until pre_dec_ack = '1';
		    pre_dec_req <= '0' after delay(2,5);
		    exception_in0 <= '0' after delay(2,5);
		    multiword_in0 <= '0' after delay(2,5);
		    wait until pre_dec_ack = '0';		    
		elsif(z = 2) then
		    exception_in0 <= '1' after delay(10,15);
		    multiword_in1 <= '1' after delay(10,15);
		    pre_dec_req <= '1' after delay(2,5);
		    wait until multiword_out = '1';
		    word2_i <= '1' after delay(20,30);
		    wait until pre_dec_ack = '1' and multiword_out = '0';
		    word2_i <= '0' after delay(2,5);
		    pre_dec_req <= '0' after delay(2,5);
		    exception_in0 <= '0' after delay(2,5);
		    multiword_in1 <= '0' after delay(2,5);
		    wait until pre_dec_ack = '0';		    
		elsif(z = 3) then
		    exception_in1 <= '1' after delay(10,15);
		    multiword_in0 <= '1' after delay(10,15);
		    pre_dec_req <= '1' after delay(2,5);
		    wait until exception_out = '1';
		    pre_dec_req <= '0' after delay(2,5);
		    exception_in1 <= '0' after delay(2,5);
		    multiword_in0 <= '0' after delay(2,5);
		    wait until exception_out = '0';
		else
		    exception_in1 <= '1' after delay(10,15);
		    multiword_in1 <= '1' after delay(10,15);
		    pre_dec_req <= '1' after delay(2,5);
		    wait until exception_out = '1';
		    pre_dec_req <= '0' after delay(2,5);
		    exception_in1 <= '0' after delay(2,5);
		    multiword_in1 <= '0' after delay(2,5);
		    wait until exception_out = '0';
		end if;
	    end process;
	end block;
end;

		      
configuration cfg_tb_pre_dec of E is
    for A
	for UUT: pre_dec
	    use configuration work.cfg_pre_dec;
	end for;
    end for;
end cfg_tb_pre_dec;
