library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity serial_ctrl is
  port(signal ALU_1_mux2_sel 	: inout std_logic:='1';
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
end serial_ctrl;


architecture behavioral of serial_ctrl is
begin
    -- controls latch between nodes addA and addB
    proc1:process
    begin
        wait until ALU_1_ack = '1' and ALU_1_mux2_sel = '0';
        l_1_req <= '1' after delay(2,4);
        wait until ALU_1_req = '1' and ALU_1_mux2_sel = '1';
        l_1_req <= '0' after delay(2,4);
    end process;

    -- controls the ack of the entire serial system
    proc2:process
    begin
        wait until zz_ack = '1' and serial_req = '1';
        serial_ack <= '1' after delay(2,4);
        wait until zz_ack = '0' and serial_req = '0';
        serial_ack <= '0' after delay(2,4);
    end process;

    -- controls l_zz between the nodes addB and sink
    proc3:process
    begin
        wait until ALU_1_ack = '1' and ALU_1_mux2_sel = '1' and 
		serial_req = '1';
        zz_req <= '1' after delay(2,4);
        wait until serial_req = '0';
        zz_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_a at the source
    proc4:process
    begin
        wait until serial_req = '1';
        a_req <= '1' after delay(2,4);
        wait until serial_req = '0';
        a_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_b at the source
    proc5:process
    begin
        wait until serial_req = '1';
        b_req <= '1' after delay(2,4);
        wait until serial_req = '0';
        b_req <= '0' after delay(2,4);
    end process;

    -- controls resource ALU_1
    proc6:process
    begin
      wait until ALU_1_ack = '0' and a_ack = '1' and b_ack = '1' and 
	serial_req='1';
      ALU_1_mux2_sel <= '0' after delay(0,1);
--      ALU_1_op <= "00" after delay(0,1);
      ALU_1_req <= '1' after delay(2,4);
      wait until l_1_ack = '1';
      ALU_1_req <= '0' after delay(2,4);
      wait until ALU_1_ack = '0' and l_1_ack = '1' and serial_req='1';
      ALU_1_mux2_sel <= '1' after delay(0,1);
      ALU_1_req <= '1' after delay(2,4);
      wait until zz_ack = '1';
      ALU_1_req <= '0' after delay(2,4);
      wait until serial_req = '0';
    end process;

end behavioral;
