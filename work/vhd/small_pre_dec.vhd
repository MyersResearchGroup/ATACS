entity small_pre_dec is

    port (pre_dec_req: 		in std_logic;  
                                      --latch first 16 bits and pc and next pc
	  exception_in0:	in std_logic;  
	  exception_in1:      	in std_logic; 
	  exception_out:	out std_logic:= '0';
	  pre_dec_ack:		out std_logic);
end small_pre_dec;

architecture control of small_pre_dec is
begin
    circuit:process
    begin
	wait until pre_dec_req = '1' and 
			(exception_in0 = '1' or exception_in1 = '0');
	if exception_in0 = '1' then
	    pre_dec_ack <= '1' after delay(2,5);
	    wait until pre_dec_req = '0' ;
	    pre_dec_ack <= '0';
	elsif exception_in1 = '1' then	    
	    exception_out <= '1' after delay(2,5);
	    wait until pre_dec_req = '0';
	    exception_out <= '0';
	end if;
    end process;
end;
	    
configuration cfg_small_pre_dec of small_pre_dec is
    for control
    end for;
end cfg_small_pre_dec;
	    
