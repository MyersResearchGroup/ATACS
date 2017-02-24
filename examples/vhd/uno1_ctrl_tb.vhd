library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity E is
end E;

architecture behavioral of E is

	signal Mult_1_req 	: std_logic;  --@ out
        signal Mult_1_ack 	: std_logic;  --@ in
        signal zz_req 		: std_logic;  --@ out
       	signal zz_ack 		: std_logic;  --@ in
       	signal a_req 		: std_logic;  --@ out
       	signal a_ack 		: std_logic;  --@ in
       	signal b_req 		: std_logic;  --@ out
       	signal b_ack 		: std_logic;  --@ in
       	signal uno_req 		: std_logic;  --@ in
       	signal uno_ack 		: std_logic;  --@ out

component uno1_ctrl 
  port(signal Mult_1_req 	: inout std_logic;
       signal Mult_1_ack 	: in  std_logic;
       signal zz_req 	: out std_logic;
       signal zz_ack 	: in  std_logic;
       signal a_req 	: out std_logic;
       signal a_ack 	: in  std_logic;
       signal b_req 	: out std_logic;
       signal b_ack 	: in  std_logic;
       signal uno_req 	: in std_logic;
       signal uno_ack 	: out std_logic);
end component;

begin
   UUT : uno1_ctrl
	port map(Mult_1_req,Mult_1_ack,zz_req,zz_ack,a_req,a_ack,b_req,
       		 b_ack,uno_req,uno_ack);

    -- controls the ack of the entire uno system
    proc1:process
    begin
        wait until uno_ack = '0';
        uno_req <= '1' after delay(10,20);
        wait until uno_ack = '1';
        uno_req <= '0' after delay(5,8);
    end process;

    -- controls l_zz between the nodes multA and sink
    proc2:process
    begin
        wait until zz_req = '1';
        zz_ack <= '1' after delay(10,20);
        wait until zz_req = '0';
        zz_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_a at the source
    proc3:process
    begin
        wait until a_req = '1';
        a_ack <= '1' after delay(10,20);
        wait until a_req = '0';
        a_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_b at the source
    proc4:process
    begin
        wait until b_req = '1';
        b_ack <= '1' after delay(10,20);
        wait until b_req = '0';
        b_ack <= '0' after delay(5,8);
    end process;

    -- controls resource Mult_1
    proc5:process
    begin
        wait until Mult_1_req = '1';
        Mult_1_ack <= '1' after delay(10,20);
        wait until Mult_1_req = '0';
        Mult_1_ack <= '0' after delay(5,8);
    end process;

end behavioral;
