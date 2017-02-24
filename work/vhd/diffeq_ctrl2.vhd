library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity diffeq_ctrl is
  port(signal ALU_1_mux4_sel 	: inout std_logic_vector(1 downto 0) := "11";
       signal Mult_1_mux2_sel 	: inout std_logic := '1';
       signal Mult_3_mux2_sel 	: inout std_logic := '1';
       signal ALU_1_req 	: inout std_logic := '0';
       signal ALU_1_ack 	: in  std_logic := '0';
       signal ALU_1_op  	: out std_logic_vector(1 downto 0) := "00";
       signal ALU_2_req 	: inout std_logic := '0';
       signal ALU_2_ack 	: in  std_logic := '0';
       signal ALU_2_op  	: out std_logic_vector(1 downto 0) := "00";
       signal Mult_1_req 	: inout std_logic := '0';
       signal Mult_1_ack 	: in  std_logic := '0';
       signal Mult_2_req 	: inout std_logic := '0';
       signal Mult_2_ack 	: in  std_logic := '0';
       signal Mult_3_req 	: inout std_logic := '0';
       signal Mult_3_ack 	: in  std_logic := '0';
       signal Mult_4_req 	: inout std_logic := '0';
       signal Mult_4_ack 	: in  std_logic := '0';
       signal l_1_req 	: out std_logic := '0';
       signal l_1_ack 	: in  std_logic := '0';
       signal l_2_req 	: out std_logic := '0';
       signal l_2_ack 	: in  std_logic := '0';
       signal l_3_req 	: out std_logic := '0';
       signal l_3_ack 	: in  std_logic := '0';
       signal l_4_req 	: out std_logic := '0';
       signal l_4_ack 	: in  std_logic := '0';
       signal l_5_req 	: out std_logic := '0';
       signal l_5_ack 	: in  std_logic := '0';
       signal l_6_req 	: out std_logic := '0';
       signal l_6_ack 	: in  std_logic := '0';
       signal l_7_req 	: out std_logic := '0';
       signal l_7_ack 	: in  std_logic := '0';
       signal l_8_req 	: out std_logic := '0';
       signal l_8_ack 	: in  std_logic := '0';
       signal c_req 	: out std_logic := '0';
       signal c_ack 	: in  std_logic := '0';
       signal u1_req 	: out std_logic := '0';
       signal u1_ack 	: in  std_logic := '0';
       signal y1_req 	: out std_logic := '0';
       signal y1_ack 	: in  std_logic := '0';
       signal a_req 	: out std_logic := '0';
       signal a_ack 	: in  std_logic := '0';
       signal dx_req 	: out std_logic := '0';
       signal dx_ack 	: in  std_logic := '0';
       signal three_req 	: out std_logic := '0';
       signal three_ack 	: in  std_logic := '0';
       signal u_req 	: out std_logic := '0';
       signal u_ack 	: in  std_logic := '0';
       signal x_req 	: out std_logic := '0';
       signal x_ack 	: in  std_logic := '0';
       signal y_req 	: out std_logic := '0';
       signal y_ack 	: in  std_logic := '0';
       signal diffeq_req 	: in std_logic := '0';
       signal diffeq_ack 	: out std_logic := '0');
end diffeq_ctrl;


architecture behavioral of diffeq_ctrl is
begin
    -- controls latch between nodes addA and less
    proc1:process
    begin
        wait until ALU_1_ack = '1' and ALU_1_mux4_sel = "00";
        l_1_req <= '1' after delay(2,4);
        -- wait until ALU_1_req = '1' and ALU_1_mux4_sel = "10";
        wait until diffeq_req = '0';
        l_1_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes asubA and subB
    proc2:process
    begin
        wait until ALU_1_ack = '1' and ALU_1_mux4_sel = "01";
        l_2_req <= '1' after delay(2,4);
        -- wait until ALU_1_req = '1' and ALU_1_mux4_sel = "11";
        wait until diffeq_req = '0';
        l_2_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multA and multE
    proc3:process
    begin
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '0';
        l_3_req <= '1' after delay(2,4);
        -- wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
        wait until diffeq_req = '0';
        l_3_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multB and multE
    proc4:process
    begin
        wait until Mult_2_ack = '1';
        l_4_req <= '1' after delay(2,4);
        -- wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
        wait until diffeq_req = '0';
        l_4_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multC and multF
    proc5:process
    begin
        wait until Mult_3_ack = '1' and Mult_3_mux2_sel = '0';
        l_5_req <= '1' after delay(2,4);
        -- wait until Mult_3_req = '1' and Mult_3_mux2_sel = '1';
        wait until diffeq_req = '0';
        l_5_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multD and addB
    proc6:process
    begin
        wait until Mult_4_ack = '1';
        l_6_req <= '1' after delay(2,4);
        -- wait until ALU_2_req = '1';
        wait until diffeq_req = '0';
        l_6_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multE and asubA
    proc7:process
    begin
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '1';
        l_7_req <= '1' after delay(2,4);
        -- wait until ALU_1_req = '1' and ALU_1_mux4_sel = "01";
        wait until diffeq_req = '0';
        l_7_req <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multF and subB
    proc8:process
    begin
        wait until Mult_3_ack = '1' and Mult_3_mux2_sel = '1';
        l_8_req <= '1' after delay(2,4);
        -- wait until ALU_1_req = '1' and ALU_1_mux4_sel = "11";
        wait until diffeq_req = '0';
        l_8_req <= '0' after delay(2,4);
    end process;

    -- controls the ack of the entire diffeq system
    proc9:process
    begin
        wait until c_ack = '1' and u1_ack = '1' and y1_ack = '1' and diffeq_req = '1';
        diffeq_ack <= '1' after delay(2,4);
        wait until c_ack = '0' and u1_ack = '0' and y1_ack = '0' and diffeq_req = '0';
        diffeq_ack <= '0' after delay(2,4);
    end process;

    -- controls latch l_y1 between the nodes addB and sink
    proc10:process
    begin
        wait until ALU_2_ack = '1' and diffeq_req = '1';
        y1_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        y1_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_c between the nodes less and sink
    proc11:process
    begin
        wait until ALU_1_ack = '1' and ALU_1_mux4_sel = "10" and diffeq_req = '1';
        c_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        c_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_u1 between the nodes subB and sink
    proc12:process
    begin
        wait until ALU_1_ack = '1' and ALU_1_mux4_sel = "11" and diffeq_req = '1';
        u1_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        u1_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_a at the source
    proc13:process
    begin
        wait until diffeq_req = '1';
        a_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        a_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_dx at the source
    proc14:process
    begin
        wait until diffeq_req = '1';
        dx_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        dx_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_three at the source
    proc15:process
    begin
        wait until diffeq_req = '1';
        three_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        three_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_u at the source
    proc16:process
    begin
        wait until diffeq_req = '1';
        u_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        u_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_x at the source
    proc17:process
    begin
        wait until diffeq_req = '1';
        x_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        x_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_y at the source
    proc18:process
    begin
        wait until diffeq_req = '1';
        y_req <= '1' after delay(2,4);
        wait until diffeq_req = '0';
        y_req <= '0' after delay(2,4);
    end process;

    -- controls resource ALU_1
    proc19:process
    begin
      wait until ALU_1_ack = '0' and x_ack = '1' and dx_ack = '1' and diffeq_req = '1';
      ALU_1_mux4_sel <= "00" after delay(0,1);
      ALU_1_req <= '1' after delay(2,4);
      wait until l_1_ack = '1';
      ALU_1_req <= '0' after delay(2,4);
      wait until ALU_1_ack = '0' and u_ack = '1' and l_7_ack = '1' and diffeq_req = '1';
      ALU_1_mux4_sel <= "01" after delay(0,1);
      ALU_1_op <= "01" after delay(0,1);
      ALU_1_req <= '1' after delay(2,4);
      wait until l_2_ack = '1';
      ALU_1_req <= '0' after delay(2,4);
      wait until ALU_1_ack = '0' and a_ack = '1' and l_1_ack = '1' and diffeq_req = '1';
      ALU_1_mux4_sel <= "10" after delay(0,1);
      ALU_1_op <= "10" after delay(0,1);
      ALU_1_req <= '1' after delay(2,4);
      wait until c_ack = '1';
      ALU_1_req <= '0' after delay(2,4);
      wait until ALU_1_ack = '0' and l_2_ack = '1' and l_8_ack = '1' and diffeq_req = '1';
      ALU_1_mux4_sel <= "11" after delay(0,1);
      ALU_1_op <= "01" after delay(0,1);
      ALU_1_req <= '1' after delay(2,4);
      wait until u1_ack = '1';
      ALU_1_req <= '0' after delay(2,4);
      wait until diffeq_req = '0';
    end process;

    -- controls resource ALU_2
    proc20:process
    begin
      wait until ALU_2_ack = '0' and y_ack = '1' and l_6_ack = '1' and diffeq_req = '1';
      ALU_2_req <= '1' after delay(2,4);
      wait until y1_ack = '1';
      ALU_2_req <= '0' after delay(2,4);
      wait until diffeq_req = '0';
    end process;

    -- controls resource Mult_1
    proc21:process
    begin
      wait until Mult_1_ack = '0' and three_ack = '1' and x_ack = '1' and diffeq_req = '1';
      Mult_1_mux2_sel <= '0' after delay(0,1);
      Mult_1_req <= '1' after delay(2,4);
      wait until l_3_ack = '1';
      Mult_1_req <= '0' after delay(2,4);
      wait until Mult_1_ack = '0' and l_3_ack = '1' and l_4_ack = '1' and diffeq_req = '1';
      Mult_1_mux2_sel <= '1' after delay(0,1);
      Mult_1_req <= '1' after delay(2,4);
      wait until l_7_ack = '1';
      Mult_1_req <= '0' after delay(2,4);
      wait until diffeq_req = '0';
    end process;

    -- controls resource Mult_2
    proc22:process
    begin
      wait until Mult_2_ack = '0' and u_ack = '1' and dx_ack = '1' and diffeq_req = '1';
      Mult_2_req <= '1' after delay(2,4);
      wait until l_4_ack = '1';
      Mult_2_req <= '0' after delay(2,4);
      wait until diffeq_req = '0';
    end process;

    -- controls resource Mult_3
    proc23:process
    begin
      wait until Mult_3_ack = '0' and three_ack = '1' and y_ack = '1' and diffeq_req = '1';
      Mult_3_mux2_sel <= '0' after delay(0,1);
      Mult_3_req <= '1' after delay(2,4);
      wait until l_5_ack = '1';
      Mult_3_req <= '0' after delay(2,4);
      wait until Mult_3_ack = '0' and dx_ack = '1' and l_5_ack = '1' and diffeq_req = '1';
      Mult_3_mux2_sel <= '1' after delay(0,1);
      Mult_3_req <= '1' after delay(2,4);
      wait until l_8_ack = '1';
      Mult_3_req <= '0' after delay(2,4);
      wait until diffeq_req = '0';
    end process;

    -- controls resource Mult_4
    proc24:process
    begin
      wait until Mult_4_ack = '0' and u_ack = '1' and dx_ack = '1' and diffeq_req = '1';
      Mult_4_req <= '1' after delay(2,4);
      wait until l_6_ack = '1';
      Mult_4_req <= '0' after delay(2,4);
      wait until diffeq_req = '0';
    end process;

end behavioral;
