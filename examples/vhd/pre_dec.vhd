--library ieee;
--use ieee.math_real.all;
--use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;
--use work.nondeterminism.all;

-- *[[predec_req & timeack]; 
--   [~exception_in & ~multiword_in -> pre_dec_ack+; [~pre_dec_req & ~timeack]; 
--                                     pre_dec_ack-;
--   |~exception_in & multiword_in -> multiword_out+; [word2_i]; pre_dec_ack+; 
--                                    multiword_out-; 
--                                    [~word2_i & ~pre_dec_req & ~timeack];
--                                    pre_dec_ack-;
--   |exception_in ->  exception_out+; [~pre_dec_req & ~timeack]; exception_out-;
--   ]
-- ]


entity pre_dec is

    port (pre_dec_req: 		in std_logic;  --latch first 16 bits and pc and next pc
	  multiword_in0:	in std_logic;
	  multiword_in1: 	in std_logic;
	  multiword_out: 	out std_logic:= '0';
	  pre_dec_ack:		out std_logic:= '0';
	  word2_i:		in std_logic;  -- latch second 16 bits and next pc
	  exception_in0:	in std_logic;
	  exception_in1:        in std_logic;
	  exception_out:	out std_logic:= '0');
end pre_dec;

architecture control of pre_dec is
begin
    circuit:process
    begin

	wait until (exception_in0 = '1' or exception_in1 = '1') and
	           (multiword_in0 = '1' or multiword_in1 = '1')	and  
	    pre_dec_req = '1';	
	if exception_in0 = '1' and multiword_in0 = '1' then
	    pre_dec_ack <= '1' after delay(2,5);
	    wait until pre_dec_req = '0' and exception_in0 = '0' and
		multiword_in0 = '0';
	    pre_dec_ack <= '0' after delay(2,5);
	elsif exception_in0 = '1' and multiword_in1 = '1' then
	    multiword_out <= '1' after delay(2,5);
	    wait until word2_i = '1';
	    pre_dec_ack <= '1' after delay(2,5);
	    multiword_out <= '0' after delay(2,5);
	    wait until word2_i = '0' and pre_dec_req = '0' and 
		exception_in0 = '0' and multiword_in1 = '0';
	    pre_dec_ack <= '0' after delay(2,5);
	elsif exception_in1 = '1' and multiword_in0 = '1' then
	    exception_out <= '1' after delay(2,5);
	    wait until pre_dec_req = '0' and exception_in1 = '0' and 
		multiword_in0 = '0';
	    exception_out <= '0' after delay(2,5);
	elsif exception_in1 = '1' and multiword_in1 = '1' then
	    exception_out <= '1' after delay(2,5);
	    wait until pre_dec_req = '0' and exception_in1 = '0' and 
		multiword_in1 = '0';
	    exception_out <= '0' after delay(2,5);
	end if;
    end process;
end;
		
configuration cfg_pre_dec of pre_dec is
    for control
    end for;
end cfg_pre_dec;
