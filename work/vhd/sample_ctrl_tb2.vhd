library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity E is
end E;

architecture behavioral of E is

    signal Mult_1_mux2_sel 	: std_logic; 	 --@ out
    signal ALU_1_req 	: std_logic; 	 --@ out
    signal ALU_1_ack 	: std_logic; 	 --@ in
    signal ALU_1_op  	: std_logic_vector(1 downto 0); 	  --@ out
    signal Mult_1_req 	: std_logic; 	 --@ out
    signal Mult_1_ack 	: std_logic; 	 --@ in
    signal l_1_req 	: std_logic; 	 --@ out
    signal l_1_ack 	: std_logic; 	 --@ in
    signal l_2_req 	: std_logic; 	 --@ out
    signal l_2_ack 	: std_logic; 	 --@ in
    signal D_req 	: std_logic; 	 --@ out
    signal D_ack 	: std_logic; 	 --@ in
    signal A_req 	: std_logic; 	 --@ out
    signal A_ack 	: std_logic; 	 --@ in
    signal B_req 	: std_logic; 	 --@ out
    signal B_ack 	: std_logic; 	 --@ in
    signal C_req 	: std_logic; 	 --@ out
    signal C_ack 	: std_logic; 	 --@ in
    signal sample_req 	: std_logic; 	 --@ in
    signal sample_ack 	: std_logic; 	 --@ out

component sample_ctrl
  port(signal Mult_1_mux2_sel 	: inout std_logic;
       signal ALU_1_req 	: inout std_logic;
       signal ALU_1_ack 	: in  std_logic;
       signal ALU_1_op  	: out std_logic_vector(1 downto 0);
       signal Mult_1_req 	: inout std_logic;
       signal Mult_1_ack 	: in  std_logic;
       signal l_1_req 	: out std_logic;
       signal l_1_ack 	: in  std_logic;
       signal l_2_req 	: out std_logic;
       signal l_2_ack 	: in  std_logic;
       signal D_req 	: out std_logic;
       signal D_ack 	: in  std_logic;
       signal A_req 	: out std_logic;
       signal A_ack 	: in  std_logic;
       signal B_req 	: out std_logic;
       signal B_ack 	: in  std_logic;
       signal C_req 	: out std_logic;
       signal C_ack 	: in  std_logic;
       signal sample_req 	: in std_logic;
       signal sample_ack 	: out std_logic);
end component;


begin
   UUT : sample_ctrl
        port map(Mult_1_mux2_sel,ALU_1_req,ALU_1_ack,ALU_1_op,Mult_1_req,Mult_1_ack,l_1_req,l_1_ack,l_2_req,l_2_ack,
         D_req,D_ack,
         A_req,A_ack,B_req,B_ack,C_req,C_ack,
         sample_req,sample_ack);

    -- controls latch between nodes opA and opC
    proc1:process
    begin
        wait until l_1_req = '1';
        l_1_ack <= '1' after delay(2,4);
        wait until l_1_req = '0';
        l_1_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes opB and opC
    proc2:process
    begin
        wait until l_2_req = '1';
        l_2_ack <= '1' after delay(2,4);
        wait until l_2_req = '0';
        l_2_ack <= '0' after delay(2,4);
    end process;

    -- controls the ack of the entire sample system
    proc3:process
    begin
       wait until sample_ack = '0';
       sample_req <= '1' after delay(10,20);
       wait until sample_ack = '1';
       sample_req <= '0' after delay(5,8);
    end process;

    -- controls l_D between the nodes opC and sink
    proc4:process
    begin
        wait until D_req = '1';
        D_ack <= '1' after delay(10,20);
        wait until D_req = '0';
        D_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_A at the source
    proc5:process
    begin
        wait until A_req = '1';
        A_ack <= '1' after delay(10,20);
        wait until A_req = '0';
        A_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_B at the source
    proc6:process
    begin
        wait until B_req = '1';
        B_ack <= '1' after delay(10,20);
        wait until B_req = '0';
        B_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_C at the source
    proc7:process
    begin
        wait until C_req = '1';
        C_ack <= '1' after delay(10,20);
        wait until C_req = '0';
        C_ack <= '0' after delay(5,8);
    end process;

    -- controls resource ALU_1
    proc8:process
    begin
      wait until ALU_1_req = '1';
      ALU_1_ack <= '1' after delay(15,45);
      wait until ALU_1_req = '0';
      ALU_1_ack <= '0' after delay(5,8);
    end process;

    -- controls resource Mult_1
    proc9:process
    begin
      wait until Mult_1_req = '1';
      Mult_1_ack <= '1' after delay(15,45);
      wait until Mult_1_req = '0';
      Mult_1_ack <= '0' after delay(5,8);
    end process;

end behavioral;
