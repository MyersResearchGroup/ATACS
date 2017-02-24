entity E is
end E;

architecture A of E is
  
    signal pre_dec_req: 	std_logic; --@ in    
    signal exception_in0: 	std_logic;  --@ in
    signal exception_in1: 	std_logic;   --@ in
    signal exception_out: 	std_logic;
    signal pre_dec_ack:		std_logic;

    component small_pre_dec
	port (pre_dec_req: 		in std_logic;  --latch first 16 bits and pc and next pc
	      exception_in0:		in std_logic;  
	      exception_in1:      	in std_logic; 
	      exception_out:		out std_logic;
	      pre_dec_ack:		out std_logic);
    end component;	

begin 
    UUT: small_pre_dec
	Port Map(pre_dec_req, pre_dec_ack,
		 exception_in0, exception_in1, exception_out);	 
	TB: block
	begin
	    p1:process
		variable z: integer;
	    begin
		z:=random(2);
		pre_dec_req <= '1' after delay(2,5);
		if (z = 1) then
		    exception_in0 <= '1' after delay(10,15);	
		    wait until pre_dec_ack = '1';
		    pre_dec_req <= '0' after delay(2,5);
		    exception_in0 <= '0' after delay(2,5);
		elsif(z = 2) then
		    exception_in1 <= '1' after delay(10,15);
		    wait until exception_out = '1';
		    pre_dec_req <= '0' after delay(2,5);
		    exception_in1 <= '0' after delay(2,5);
		end if;
	end process;
    end block;
end;


configuration cfg_tb_small_pre_dec of E is
    for A
	for UUT: small_pre_dec
	    use configuration work.cfg_small_pre_dec;
	end for;
    end for;
end cfg_tb_small_pre_dec;
