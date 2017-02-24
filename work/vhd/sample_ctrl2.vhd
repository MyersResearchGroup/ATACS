library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity sample_ctrl is
  port(signal Mult_1_mux2_sel 	: inout std_logic := '1';
       signal ALU_1_req 	: inout std_logic := '0';
       signal ALU_1_ack 	: in  std_logic := '0';
       signal ALU_1_op  	: out std_logic_vector(1 downto 0) := "00";
       signal Mult_1_req 	: inout std_logic := '0';
       signal Mult_1_ack 	: in  std_logic := '0';
       signal l_1_req 	: out std_logic := '0';
       signal l_1_ack 	: in  std_logic := '0';
       signal l_2_req 	: out std_logic := '0';
       signal l_2_ack 	: in  std_logic := '0';
       signal D_req 	: out std_logic := '0';
       signal D_ack 	: in  std_logic := '0';
       signal A_req 	: out std_logic := '0';
       signal A_ack 	: in  std_logic := '0';
       signal B_req 	: out std_logic := '0';
       signal B_ack 	: in  std_logic := '0';
       signal C_req 	: out std_logic := '0';
       signal C_ack 	: in  std_logic := '0';
       signal sample_req 	: in std_logic := '0';
       signal sample_ack 	: out std_logic := '0');
end sample_ctrl;


architecture behavioral of sample_ctrl is
begin
    -- controls latch between nodes opA and opC
    proc1:process
    begin
        wait until ALU_1_ack = '1' and sample_req='1';
        l_1_req <= '1' after delay(2,4);
        -- wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
        wait until sample_req = '0';
        l_1_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes opB and opC
    proc2:process
    begin
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '0';
        l_2_req <= '1' after delay(2,4);
        -- wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
        wait until sample_req = '0';
        l_2_req <= '0' after delay(2,4);
    end process;

    -- controls the ack of the entire sample system
    proc3:process
    begin
        wait until D_ack = '1' and sample_req = '1';
        sample_ack <= '1' after delay(2,4);
        wait until D_ack = '0' and sample_req = '0';
        sample_ack <= '0' after delay(2,4);
    end process;

    -- controls latch l_D between the nodes opC and sink
    proc4:process
    begin
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '1' and sample_req = '1';
        D_req <= '1' after delay(2,4);
        wait until sample_req = '0';
        D_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_A at the source
    proc5:process
    begin
        wait until sample_req = '1';
        A_req <= '1' after delay(2,4);
        wait until sample_req = '0';
        A_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_B at the source
    proc6:process
    begin
        wait until sample_req = '1';
        B_req <= '1' after delay(2,4);
        wait until sample_req = '0';
        B_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_C at the source
    proc7:process
    begin
        wait until sample_req = '1';
        C_req <= '1' after delay(2,4);
        wait until sample_req = '0';
        C_req <= '0' after delay(2,4);
    end process;

    -- controls resource ALU_1
    proc8:process
    begin
      wait until ALU_1_ack = '0' and A_ack = '1' and B_ack = '1' and sample_req = '1';
      ALU_1_req <= '1' after delay(2,4);
      wait until l_1_ack = '1';
      ALU_1_req <= '0' after delay(2,4);
      wait until sample_req = '0';
    end process;

    -- controls resource Mult_1
    proc9:process
    begin
      wait until Mult_1_ack = '0' and B_ack = '1' and C_ack = '1' and sample_req = '1';
      Mult_1_mux2_sel <= '0' after delay(0,1);
      Mult_1_req <= '1' after delay(2,4);
      wait until l_2_ack = '1';
      Mult_1_req <= '0' after delay(2,4);
      wait until Mult_1_ack = '0' and l_1_ack = '1' and l_2_ack = '1' and sample_req = '1';
      Mult_1_mux2_sel <= '1' after delay(0,1);
      Mult_1_req <= '1' after delay(2,4);
      wait until D_ack = '1';
      Mult_1_req <= '0' after delay(2,4);
      wait until sample_req = '0';
    end process;

end behavioral;
