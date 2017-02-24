library IEEE;
use IEEE.std_logic_1164.all;

entity sample_ctrl is
  port(signal Mult_1_mux2_sel 	: inout std_logic;
       signal ALU_1_req 	: inout std_logic;
       signal ALU_1_ack 	: in  std_logic;
       signal ALU_1_op  	: out std_logic_vector(1 downto 0);
       --signal ALU_1_op_1  	: out std_logic;
       --signal ALU_1_op_2  	: out std_logic;
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
end sample_ctrl;


architecture behavioral of sample_ctrl is
begin
    -- controls latch between nodes opA and opC
    proc1:process
    begin
        l_1_req <= '0';
        wait until ALU_1_ack = '1';
        l_1_req <= '1';
        wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
    end process;

    -- controls latch between nodes opB and opC
    proc2:process
    begin
        l_2_req <= '0';
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '0';
        l_2_req <= '1';
        wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
    end process;

    -- controls the ack of the entire sample system
    proc3:process
    begin
        sample_ack <= '0';
        wait until D_ack = '1';
        sample_ack <= '1';
        wait until sample_req = '0';
    end process;

    -- controls l_D between the nodes opC and sink
    proc4:process
    begin
        D_req <= '0';
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '1';
        D_req <= '1';
        wait until sample_req = '0';
    end process;

    -- controls latch l_A at the source
    proc5:process
    begin
        A_req <= '0';
        wait until sample_req = '1';
        A_req <= '1';
        wait until sample_req = '0';
    end process;

    -- controls latch l_B at the source
    proc6:process
    begin
        B_req <= '0';
        wait until sample_req = '1';
        B_req <= '1';
        wait until sample_req = '0';
    end process;

    -- controls latch l_C at the source
    proc7:process
    begin
        C_req <= '0';
        wait until sample_req = '1';
        C_req <= '1';
        wait until sample_req = '0';
    end process;

    -- controls resource ALU_1
    proc8:process
    begin
      ALU_1_req <= '0';
      wait until ALU_1_ack = '0' and A_ack = '1' and B_ack = '1';
      ALU_1_op <= "00";
      --ALU_1_op_1 <= '0';
      --ALU_1_op_2 <= '0';
      --ALU_1_req <= '1' after 2 ns;
      wait until l_1_ack = '1';
      wait until sample_req = '0';
    end process;

    -- controls resource Mult_1
    proc9:process
    begin
      Mult_1_req <= '0';
      wait until Mult_1_ack = '0' and B_ack = '1' and C_ack = '1';
      Mult_1_mux2_sel <= '0';
      Mult_1_req <= '1' after 2 ns;
      wait until l_2_ack = '1';
      Mult_1_req <= '0';
      wait until Mult_1_ack = '0' and l_1_ack = '1' and l_2_ack = '1';
      Mult_1_mux2_sel <= '1';
      Mult_1_req <= '1' after 2 ns;
      wait until D_ack = '1';
      wait until sample_req = '0';
    end process;

end behavioral;
