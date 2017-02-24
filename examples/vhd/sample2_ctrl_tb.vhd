library IEEE; 
use IEEE.std_logic_1164.all; 
use work.nond.all;

entity E is
end E;

Architecture A of E is


       signal Mult_1_mux2_sel 	: std_logic; --@ in
       signal ALU_1_req 	: std_logic; --@ out
       signal ALU_1_ack 	: std_logic; --@ in
       signal ALU_1_op  	: std_logic; --@ in 
       signal Mult_1_req 	: std_logic; --@ out
       signal Mult_1_ack 	: std_logic; --@ in
       signal l_1_req 	        : std_logic; --@ out
       signal l_1_ack 	        : std_logic; --@ in
       signal l_2_req 	        : std_logic; --@ out
       signal l_2_ack        	: std_logic; --@ in
       signal D_req 	        : std_logic; --@ out
       signal D_ack 	        : std_logic; --@ in
       signal A_req 	        : std_logic; --@ out
       signal A_ack 	        : std_logic; --@ in
       signal B_req 	        : std_logic; --@ out
       signal B_ack 	        : std_logic; --@ in
       signal C_req 	        : std_logic; --@ out
       signal C_ack 	        : std_logic; --@ in
       signal sample_req 	: std_logic; --@ in
       signal sample_ack 	: std_logic; --@ out


component sample2_ctrl
  port(signal Mult_1_mux2_sel 	: inout std_logic;
       signal ALU_1_req 	: inout std_logic;
       signal ALU_1_ack 	: in  std_logic;
       signal ALU_1_op  	: out std_logic;
       signal Mult_1_req 	: inout std_logic;
       signal Mult_1_ack 	: in  std_logic;
       signal l_1_req 	        : out std_logic;
       signal l_1_ack 	        : in  std_logic;
       signal l_2_req 	        : out std_logic;
       signal l_2_ack        	: in  std_logic;
       signal D_req 	        : out std_logic;
       signal D_ack 	        : in  std_logic;
       signal A_req 	        : out std_logic;
       signal A_ack 	        : in  std_logic;
       signal B_req 	        : out std_logic;
       signal B_ack 	        : in  std_logic;
       signal C_req 	        : out std_logic;
       signal C_ack 	        : in  std_logic;
       signal sample_req 	: in std_logic;
       signal sample_ack 	: out std_logic);
end component;

begin
   UUT : sample2_ctrl
	port map(Mult_1_mux2_sel, ALU_1_req, ALU_1_ack, ALU_1_op,  
  	         Mult_1_req, Mult_1_ack, l_1_req, l_1_ack, 	
    	         l_2_req, l_2_ack, D_req, D_ack, A_req, 	
      	         A_ack, B_req, B_ack, C_req, C_ack, 	
       	         sample_req, sample_ack);


p9:process
   begin 
       wait until sample_ack='0';
       sample_req <= '1' after delay(10,20);
       wait until sample_ack='1';
       sample_req <='0' after delay(10,20);
end process;


p0:process
   begin
	wait until ALU_1_req='1';
        ALU_1_ack <= '1' after delay(10,20);
	wait until ALU_1_req='0';
        ALU_1_ack <= '0' after delay(5,10);
end process;


p1:process
   begin 
        wait until Mult_1_req='1';
        Mult_1_ack <= '1' after delay(20,30);
        wait until Mult_1_req='0';
	Mult_1_ack <= '0' after delay(5,10);
end process;

p2:process
   begin 
        wait until l_1_req='1';
	l_1_ack <= '1' after delay(5,10);
        wait until l_1_req='0';
	l_1_ack <= '0' after delay(5,10);
end process;

p3:process
   begin 
        wait until D_req='1';
	D_ack <= '1' after delay(5,10);
        wait until D_req='0';
	D_ack <= '0' after delay(5,10);
end process;

p4:process
   begin 
        wait until l_2_req='1';
	l_2_ack <= '1' after delay(5,10);
        wait until l_2_req='0';
	l_2_ack <= '0' after delay(5,10);
end process;


p5:process
   begin 
        wait until A_req='1';
	A_ack <= '1' after delay(5,10);
        wait until A_req='0';
	A_ack <= '0' after delay(5,10);	
end process;

p6:process
   begin 
        wait until B_req='1';
	B_ack <= '1' after delay(5,10);
        wait until B_req='0';
	B_ack <= '0' after delay(5,10);
end process;

p7:process
   begin 
        wait until C_req='1';
        C_ack <= '1' after delay(5,10);
        wait until C_req='0';
	C_ack <= '0' after delay(5,10);
end process;

end A;