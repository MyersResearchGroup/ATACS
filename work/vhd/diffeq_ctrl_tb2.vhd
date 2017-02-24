library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity E is
end E;

architecture behavioral of E is

    signal ALU_1_mux4_sel 	: std_logic_vector(1 downto 0); 	 --@ out
    signal Mult_1_mux2_sel 	: std_logic; 	 --@ out
    signal Mult_3_mux2_sel 	: std_logic; 	 --@ out
    signal ALU_1_req 	: std_logic; 	 --@ out
    signal ALU_1_ack 	: std_logic; 	 --@ in
    signal ALU_1_op  	: std_logic_vector(1 downto 0); 	  --@ out
    signal ALU_2_req 	: std_logic; 	 --@ out
    signal ALU_2_ack 	: std_logic; 	 --@ in
    signal ALU_2_op  	: std_logic_vector(1 downto 0); 	  --@ out
    signal Mult_1_req 	: std_logic; 	 --@ out
    signal Mult_1_ack 	: std_logic; 	 --@ in
    signal Mult_2_req 	: std_logic; 	 --@ out
    signal Mult_2_ack 	: std_logic; 	 --@ in
    signal Mult_3_req 	: std_logic; 	 --@ out
    signal Mult_3_ack 	: std_logic; 	 --@ in
    signal Mult_4_req 	: std_logic; 	 --@ out
    signal Mult_4_ack 	: std_logic; 	 --@ in
    signal l_1_req 	: std_logic; 	 --@ out
    signal l_1_ack 	: std_logic; 	 --@ in
    signal l_2_req 	: std_logic; 	 --@ out
    signal l_2_ack 	: std_logic; 	 --@ in
    signal l_3_req 	: std_logic; 	 --@ out
    signal l_3_ack 	: std_logic; 	 --@ in
    signal l_4_req 	: std_logic; 	 --@ out
    signal l_4_ack 	: std_logic; 	 --@ in
    signal l_5_req 	: std_logic; 	 --@ out
    signal l_5_ack 	: std_logic; 	 --@ in
    signal l_6_req 	: std_logic; 	 --@ out
    signal l_6_ack 	: std_logic; 	 --@ in
    signal l_7_req 	: std_logic; 	 --@ out
    signal l_7_ack 	: std_logic; 	 --@ in
    signal l_8_req 	: std_logic; 	 --@ out
    signal l_8_ack 	: std_logic; 	 --@ in
    signal c_req 	: std_logic; 	 --@ out
    signal c_ack 	: std_logic; 	 --@ in
    signal u1_req 	: std_logic; 	 --@ out
    signal u1_ack 	: std_logic; 	 --@ in
    signal y1_req 	: std_logic; 	 --@ out
    signal y1_ack 	: std_logic; 	 --@ in
    signal a_req 	: std_logic; 	 --@ out
    signal a_ack 	: std_logic; 	 --@ in
    signal dx_req 	: std_logic; 	 --@ out
    signal dx_ack 	: std_logic; 	 --@ in
    signal three_req 	: std_logic; 	 --@ out
    signal three_ack 	: std_logic; 	 --@ in
    signal u_req 	: std_logic; 	 --@ out
    signal u_ack 	: std_logic; 	 --@ in
    signal x_req 	: std_logic; 	 --@ out
    signal x_ack 	: std_logic; 	 --@ in
    signal y_req 	: std_logic; 	 --@ out
    signal y_ack 	: std_logic; 	 --@ in
    signal diffeq_req 	: std_logic; 	 --@ in
    signal diffeq_ack 	: std_logic; 	 --@ out

component diffeq_ctrl
  port(signal ALU_1_mux4_sel 	: inout std_logic_vector(1 downto 0);
       signal Mult_1_mux2_sel 	: inout std_logic;
       signal Mult_3_mux2_sel 	: inout std_logic;
       signal ALU_1_req 	: inout std_logic;
       signal ALU_1_ack 	: in  std_logic;
       signal ALU_1_op  	: out std_logic_vector(1 downto 0);
       signal ALU_2_req 	: inout std_logic;
       signal ALU_2_ack 	: in  std_logic;
       signal ALU_2_op  	: out std_logic_vector(1 downto 0);
       signal Mult_1_req 	: inout std_logic;
       signal Mult_1_ack 	: in  std_logic;
       signal Mult_2_req 	: inout std_logic;
       signal Mult_2_ack 	: in  std_logic;
       signal Mult_3_req 	: inout std_logic;
       signal Mult_3_ack 	: in  std_logic;
       signal Mult_4_req 	: inout std_logic;
       signal Mult_4_ack 	: in  std_logic;
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
end component;


begin
   UUT : diffeq_ctrl
        port map(ALU_1_mux4_sel,Mult_1_mux2_sel,Mult_3_mux2_sel,ALU_1_req,ALU_1_ack,ALU_1_op,ALU_2_req,ALU_2_ack,ALU_2_op,Mult_1_req,Mult_1_ack,Mult_2_req,Mult_2_ack,Mult_3_req,Mult_3_ack,Mult_4_req,Mult_4_ack,l_1_req,l_1_ack,l_2_req,l_2_ack,l_3_req,l_3_ack,l_4_req,l_4_ack,l_5_req,l_5_ack,l_6_req,l_6_ack,l_7_req,l_7_ack,l_8_req,l_8_ack,
         c_req,c_ack,u1_req,u1_ack,y1_req,y1_ack,
         a_req,a_ack,dx_req,dx_ack,three_req,three_ack,u_req,u_ack,x_req,x_ack,y_req,y_ack,
         diffeq_req,diffeq_ack);

    -- controls latch between nodes addA and less
    proc1:process
    begin
        wait until l_1_req = '1';
        l_1_ack <= '1' after delay(2,4);
        wait until l_1_req = '0';
        l_1_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes asubA and subB
    proc2:process
    begin
        wait until l_2_req = '1';
        l_2_ack <= '1' after delay(2,4);
        wait until l_2_req = '0';
        l_2_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multA and multE
    proc3:process
    begin
        wait until l_3_req = '1';
        l_3_ack <= '1' after delay(2,4);
        wait until l_3_req = '0';
        l_3_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multB and multE
    proc4:process
    begin
        wait until l_4_req = '1';
        l_4_ack <= '1' after delay(2,4);
        wait until l_4_req = '0';
        l_4_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multC and multF
    proc5:process
    begin
        wait until l_5_req = '1';
        l_5_ack <= '1' after delay(2,4);
        wait until l_5_req = '0';
        l_5_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multD and addB
    proc6:process
    begin
        wait until l_6_req = '1';
        l_6_ack <= '1' after delay(2,4);
        wait until l_6_req = '0';
        l_6_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multE and asubA
    proc7:process
    begin
        wait until l_7_req = '1';
        l_7_ack <= '1' after delay(2,4);
        wait until l_7_req = '0';
        l_7_ack <= '0' after delay(2,4);
    end process;

    -- controls latch between nodes multF and subB
    proc8:process
    begin
        wait until l_8_req = '1';
        l_8_ack <= '1' after delay(2,4);
        wait until l_8_req = '0';
        l_8_ack <= '0' after delay(2,4);
    end process;

    -- controls the ack of the entire diffeq system
    proc9:process
    begin
       wait until diffeq_ack = '0';
       diffeq_req <= '1' after delay(10,20);
       wait until diffeq_ack = '1';
       diffeq_req <= '0' after delay(5,8);
    end process;

    -- controls l_y1 between the nodes addB and sink
    proc10:process
    begin
        wait until y1_req = '1';
        y1_ack <= '1' after delay(10,20);
        wait until y1_req = '0';
        y1_ack <= '0' after delay(5,8);
    end process;

    -- controls l_c between the nodes less and sink
    proc11:process
    begin
        wait until c_req = '1';
        c_ack <= '1' after delay(10,20);
        wait until c_req = '0';
        c_ack <= '0' after delay(5,8);
    end process;

    -- controls l_u1 between the nodes subB and sink
    proc12:process
    begin
        wait until u1_req = '1';
        u1_ack <= '1' after delay(10,20);
        wait until u1_req = '0';
        u1_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_a at the source
    proc13:process
    begin
        wait until a_req = '1';
        a_ack <= '1' after delay(10,20);
        wait until a_req = '0';
        a_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_dx at the source
    proc14:process
    begin
        wait until dx_req = '1';
        dx_ack <= '1' after delay(10,20);
        wait until dx_req = '0';
        dx_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_three at the source
    proc15:process
    begin
        wait until three_req = '1';
        three_ack <= '1' after delay(10,20);
        wait until three_req = '0';
        three_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_u at the source
    proc16:process
    begin
        wait until u_req = '1';
        u_ack <= '1' after delay(10,20);
        wait until u_req = '0';
        u_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_x at the source
    proc17:process
    begin
        wait until x_req = '1';
        x_ack <= '1' after delay(10,20);
        wait until x_req = '0';
        x_ack <= '0' after delay(5,8);
    end process;

    -- controls latch l_y at the source
    proc18:process
    begin
        wait until y_req = '1';
        y_ack <= '1' after delay(10,20);
        wait until y_req = '0';
        y_ack <= '0' after delay(5,8);
    end process;

    -- controls resource ALU_1
    proc19:process
    begin
      wait until ALU_1_req = '1';
      ALU_1_ack <= '1' after delay(15,45);
      wait until ALU_1_req = '0';
      ALU_1_ack <= '0' after delay(5,8);
    end process;

    -- controls resource ALU_2
    proc20:process
    begin
      wait until ALU_2_req = '1';
      ALU_2_ack <= '1' after delay(15,45);
      wait until ALU_2_req = '0';
      ALU_2_ack <= '0' after delay(5,8);
    end process;

    -- controls resource Mult_1
    proc21:process
    begin
      wait until Mult_1_req = '1';
      Mult_1_ack <= '1' after delay(15,45);
      wait until Mult_1_req = '0';
      Mult_1_ack <= '0' after delay(5,8);
    end process;

    -- controls resource Mult_2
    proc22:process
    begin
      wait until Mult_2_req = '1';
      Mult_2_ack <= '1' after delay(15,45);
      wait until Mult_2_req = '0';
      Mult_2_ack <= '0' after delay(5,8);
    end process;

    -- controls resource Mult_3
    proc23:process
    begin
      wait until Mult_3_req = '1';
      Mult_3_ack <= '1' after delay(15,45);
      wait until Mult_3_req = '0';
      Mult_3_ack <= '0' after delay(5,8);
    end process;

    -- controls resource Mult_4
    proc24:process
    begin
      wait until Mult_4_req = '1';
      Mult_4_ack <= '1' after delay(15,45);
      wait until Mult_4_req = '0';
      Mult_4_ack <= '0' after delay(5,8);
    end process;

end behavioral;
