library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity E is
end E;

architecture behavioral of E is

    signal ALU_1_mux2_sel 	: std_logic; 	 --@ out
    signal ALU_1_req 	: std_logic; 	 --@ out
    signal ALU_1_ack 	: std_logic; 	 --@ in
    signal ALU_1_op  	: std_logic_vector(1 downto 0); 	  --@ out
    signal l_1_req 	: std_logic; 	 --@ out
    signal l_1_ack 	: std_logic; 	 --@ in
    signal zz_req 	: std_logic; 	 --@ out
    signal zz_ack 	: std_logic; 	 --@ in
    signal a_req 	: std_logic; 	 --@ out
    signal a_ack 	: std_logic; 	 --@ in
    signal b_req 	: std_logic; 	 --@ out
    signal b_ack 	: std_logic; 	 --@ in
    signal serial_req 	: std_logic; 	 --@ in
    signal serial_ack 	: std_logic; 	 --@ out

component serial_ctrl
  port(signal ALU_1_mux2_sel 	: inout std_logic;
       signal ALU_1_req 	: inout std_logic;
       signal ALU_1_ack 	: in  std_logic;
       signal ALU_1_op  	: out std_logic_vector(1 downto 0);
       signal l_1_req 	: out std_logic;
       signal l_1_ack 	: in  std_logic;
       signal zz_req 	: out std_logic;
       signal zz_ack 	: in  std_logic;
       signal a_req 	: out std_logic;
       signal a_ack 	: in  std_logic;
       signal b_req 	: out std_logic;
       signal b_ack 	: in  std_logic;
       signal serial_req 	: in std_logic;
       signal serial_ack 	: out std_logic);
end component;


begin
   UUT : serial_ctrl
        port map(ALU_1_mux2_sel,ALU_1_req,ALU_1_ack,ALU_1_op,l_1_req,l_1_ack,
         zz_req,zz_ack,
         a_req,a_ack,b_req,b_ack,
         serial_req,serial_ack);

    -- controls latch between nodes addA and addB
    proc1:process
    begin
        wait until l_1_req = '1';
        l_1_ack <= '1' after delay(2,4);
        wait until l_1_req = '0';
        l_1_ack <= '0' after delay(2,4);
    end process;

    -- controls the ack of the entire serial system
    proc2:process
    begin
       wait until serial_ack = '0';
       serial_req <= '1' after delay(10,20);
       wait until serial_ack = '1';
       serial_req <= '0' after delay(5,8);
    end process;

    -- controls l_zz between the nodes addB and sink
    proc3:process
    begin
        wait until zz_req = '1';
        zz_ack <= '1' after delay(10,20);
        wait until zz_req = '0';
        zz_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_a at the source
    proc4:process
    begin
        wait until a_req = '1';
        a_ack <= '1' after delay(10,20);
        wait until a_req = '0';
        a_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_b at the source
    proc5:process
    begin
        wait until b_req = '1';
        b_ack <= '1' after delay(10,20);
        wait until b_req = '0';
        b_ack <= '0' after delay(5,8);
    end process;

    -- controls resource ALU_1
    proc6:process
    begin
      wait until ALU_1_req = '1';
      ALU_1_ack <= '1' after delay(15,45);
      wait until ALU_1_req = '0';
      ALU_1_ack <= '0' after delay(5,8);
    end process;

end behavioral;
