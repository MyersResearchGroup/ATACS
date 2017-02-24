library IEEE;
use IEEE.std_logic_1164.all;

entity diffeq_ctrl is
  port(signal ALU_1_mux5_sel 	: inout std_logic_vector(2 downto 0);
       signal Mult_1_mux2_sel 	: inout std_logic;
       signal Mult_3_mux3_sel 	: inout std_logic_vector(1 downto 0);
       signal ALU_1_req 	: inout std_logic;
       signal ALU_1_ack 	: in  std_logic;
       signal ALU_1_op  	: out std_logic_vector(1 downto 0);
       signal Mult_1_req 	: inout std_logic;
       signal Mult_1_ack 	: in  std_logic;
       signal Mult_2_req 	: inout std_logic;
       signal Mult_2_ack 	: in  std_logic;
       signal Mult_3_req 	: inout std_logic;
       signal Mult_3_ack 	: in  std_logic;
       signal l_1_req 	: out std_logic;
       signal l_1_ack 	: in  std_logic;
       signal l_2_req 	: out std_logic;
       signal l_2_ack 	: in  std_logic;
       signal l_3_req 	: out std_logic;
       signal l_3_ack 	: in  std_logic;
       signal l_4_req 	: out std_logic;
       signal l_4_ack 	: in  std_logic;
       signal l_5_req 	: out std_logic;
       signal l_5_ack 	: in  std_logic;
       signal l_6_req 	: out std_logic;
       signal l_6_ack 	: in  std_logic;
       signal l_7_req 	: out std_logic;
       signal l_7_ack 	: in  std_logic;
       signal l_8_req 	: out std_logic;
       signal l_8_ack 	: in  std_logic;
       signal c_req 	: out std_logic;
       signal c_ack 	: in  std_logic;
       signal u1_req 	: out std_logic;
       signal u1_ack 	: in  std_logic;
       signal y1_req 	: out std_logic;
       signal y1_ack 	: in  std_logic;
       signal a_req 	: out std_logic;
       signal a_ack 	: in  std_logic;
       signal dx_req 	: out std_logic;
       signal dx_ack 	: in  std_logic;
       signal three_req 	: out std_logic;
       signal three_ack 	: in  std_logic;
       signal u_req 	: out std_logic;
       signal u_ack 	: in  std_logic;
       signal x_req 	: out std_logic;
       signal x_ack 	: in  std_logic;
       signal y_req 	: out std_logic;
       signal y_ack 	: in  std_logic;
       signal diffeq_req 	: in std_logic;
       signal diffeq_ack 	: out std_logic);
end diffeq_ctrl;


architecture behavioral of diffeq_ctrl is
begin
    -- controls latch between nodes addA and less
    proc1:process
    begin
        l_1_req <= '0';
        wait until ALU_1_ack = '1' and ALU_1_mux5_sel = "000";
        l_1_req <= '1';
        wait until ALU_1_req = '1' and ALU_1_mux5_sel = "011";
    end process;

    -- controls latch between nodes asubA and subB
    proc2:process
    begin
        l_2_req <= '0';
        wait until ALU_1_ack = '1' and ALU_1_mux5_sel = "010";
        l_2_req <= '1';
        wait until ALU_1_req = '1' and ALU_1_mux5_sel = "100";
    end process;

    -- controls latch between nodes multA and multE
    proc3:process
    begin
        l_3_req <= '0';
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '0';
        l_3_req <= '1';
        wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
    end process;

    -- controls latch between nodes multB and multE
    proc4:process
    begin
        l_4_req <= '0';
        wait until Mult_2_ack = '1';
        l_4_req <= '1';
        wait until Mult_1_req = '1' and Mult_1_mux2_sel = '1';
    end process;

    -- controls latch between nodes multC and multF
    proc5:process
    begin
        l_5_req <= '0';
        wait until Mult_3_ack = '1' and Mult_3_mux3_sel = "00";
        l_5_req <= '1';
        wait until Mult_3_req = '1' and Mult_3_mux3_sel = "10";
    end process;

    -- controls latch between nodes multD and addB
    proc6:process
    begin
        l_6_req <= '0';
        wait until Mult_3_ack = '1' and Mult_3_mux3_sel = "01";
        l_6_req <= '1';
        wait until ALU_1_req = '1' and ALU_1_mux5_sel = "001";
    end process;

    -- controls latch between nodes multE and asubA
    proc7:process
    begin
        l_7_req <= '0';
        wait until Mult_1_ack = '1' and Mult_1_mux2_sel = '1';
        l_7_req <= '1';
        wait until ALU_1_req = '1' and ALU_1_mux5_sel = "010";
    end process;

    -- controls latch between nodes multF and subB
    proc8:process
    begin
        l_8_req <= '0';
        wait until Mult_3_ack = '1' and Mult_3_mux3_sel = "10";
        l_8_req <= '1';
        wait until ALU_1_req = '1' and ALU_1_mux5_sel = "100";
    end process;

    -- controls the ack of the entire diffeq system
    proc9:process
    begin
        diffeq_ack <= '0';
        wait until c_ack = '1' and u1_ack = '1' and y1_ack = '1';
        diffeq_ack <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls l_y1 between the nodes addB and sink
    proc10:process
    begin
        y1_req <= '0';
        wait until ALU_1_ack = '1' and ALU_1_mux5_sel = "001";
        y1_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls l_c between the nodes less and sink
    proc11:process
    begin
        c_req <= '0';
        wait until ALU_1_ack = '1' and ALU_1_mux5_sel = "011";
        c_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls l_u1 between the nodes subB and sink
    proc12:process
    begin
        u1_req <= '0';
        wait until ALU_1_ack = '1' and ALU_1_mux5_sel = "100";
        u1_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls latch l_a at the source
    proc13:process
    begin
        a_req <= '0';
        wait until diffeq_req = '1';
        a_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls latch l_dx at the source
    proc14:process
    begin
        dx_req <= '0';
        wait until diffeq_req = '1';
        dx_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls latch l_three at the source
    proc15:process
    begin
        three_req <= '0';
        wait until diffeq_req = '1';
        three_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls latch l_u at the source
    proc16:process
    begin
        u_req <= '0';
        wait until diffeq_req = '1';
        u_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls latch l_x at the source
    proc17:process
    begin
        x_req <= '0';
        wait until diffeq_req = '1';
        x_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls latch l_y at the source
    proc18:process
    begin
        y_req <= '0';
        wait until diffeq_req = '1';
        y_req <= '1';
        wait until diffeq_req = '0';
    end process;

    -- controls resource ALU_1
    proc19:process
    begin
      ALU_1_req <= '0';
      wait until ALU_1_ack = '0' and x_ack = '1' and dx_ack = '1';
      ALU_1_mux5_sel <= "000";
      ALU_1_op <= "00";
      wait for 2 ns;
      ALU_1_req <= '1';
      wait until l_1_ack = '1';
      ALU_1_req <= '0';
      wait until ALU_1_ack = '0' and y_ack = '1' and l_6_ack = '1';
      ALU_1_mux5_sel <= "001";
      wait for 2 ns;
      ALU_1_req <= '1';
      wait until y1_ack = '1';
      ALU_1_req <= '0';
      wait until ALU_1_ack = '0' and u_ack = '1' and l_7_ack = '1';
      ALU_1_mux5_sel <= "010";
      ALU_1_op <= "01";
      wait for 2 ns;
      ALU_1_req <= '1';
      wait until l_2_ack = '1';
      ALU_1_req <= '0';
      wait until ALU_1_ack = '0' and a_ack = '1' and l_1_ack = '1';
      ALU_1_mux5_sel <= "011";
      ALU_1_op <= "10";
      wait for 2 ns;
      ALU_1_req <= '1';
      wait until c_ack = '1';
      ALU_1_req <= '0';
      wait until ALU_1_ack = '0' and l_2_ack = '1' and l_8_ack = '1';
      ALU_1_mux5_sel <= "100";
      ALU_1_op <= "01";
      wait for 2 ns;
      ALU_1_req <= '1';
      wait until u1_ack = '1';
      wait until diffeq_req = '0';
    end process;

    -- controls resource Mult_1
    proc20:process
    begin
      Mult_1_req <= '0';
      wait until Mult_1_ack = '0' and three_ack = '1' and x_ack = '1';
      Mult_1_mux2_sel <= '0';
      wait for 2 ns;
      Mult_1_req <= '1';
      wait until l_3_ack = '1';
      Mult_1_req <= '0';
      wait until Mult_1_ack = '0' and l_3_ack = '1' and l_4_ack = '1';
      Mult_1_mux2_sel <= '1';
      wait for 2 ns;
      Mult_1_req <= '1';
      wait until l_7_ack = '1';
      wait until diffeq_req = '0';
    end process;

    -- controls resource Mult_2
    proc21:process
    begin
      Mult_2_req <= '0';
      wait until Mult_2_ack = '0' and u_ack = '1' and dx_ack = '1';
      Mult_2_req <= '1';
      wait until l_4_ack = '1';
      wait until diffeq_req = '0';
    end process;

    -- controls resource Mult_3
    proc22:process
    begin
      Mult_3_req <= '0';
      wait until Mult_3_ack = '0' and three_ack = '1' and y_ack = '1';
      Mult_3_mux3_sel <= "00";
      wait for 2 ns;
      Mult_3_req <= '1';
      wait until l_5_ack = '1';
      Mult_3_req <= '0';
      wait until Mult_3_ack = '0' and u_ack = '1' and dx_ack = '1';
      Mult_3_mux3_sel <= "01";
      wait for 2 ns;
      Mult_3_req <= '1';
      wait until l_6_ack = '1';
      Mult_3_req <= '0';
      wait until Mult_3_ack = '0' and dx_ack = '1' and l_5_ack = '1';
      Mult_3_mux3_sel <= "10";
      wait for 2 ns;
      Mult_3_req <= '1';
      wait until l_8_ack = '1';
      wait until diffeq_req = '0';
    end process;

end behavioral;
