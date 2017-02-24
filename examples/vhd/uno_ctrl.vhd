library IEEE;
use IEEE.std_logic_1164.all;
use work.nond.all;

entity uno_ctrl is
  port(signal Mult_1_req 	: inout std_logic := '0';
       signal Mult_1_ack 	: in  std_logic := '0';
       signal zz_req 	: out std_logic := '0';
       signal zz_ack 	: in  std_logic := '0';
       signal a_req 	: out std_logic := '0';
       signal a_ack 	: in  std_logic := '0';
       signal b_req 	: out std_logic := '0';
       signal b_ack 	: in  std_logic := '0';
       signal uno_req 	: in std_logic := '0';
       signal uno_ack 	: out std_logic := '0');
end uno_ctrl;


architecture behavioral of uno_ctrl is
begin
    -- controls the ack of the entire uno system
    proc1:process
    begin
        wait until zz_ack = '1' and uno_req = '1';
        uno_ack <= '1' after delay(2,4);
        wait until zz_ack = '0' and uno_req = '0';
        uno_ack <= '0' after delay(2,4);
    end process;

    -- controls latch l_zz between the nodes multA and sink
    proc2:process
    begin
        wait until Mult_1_ack = '1' and uno_req = '1';
        zz_req <= '1' after delay(2,4);
        wait until uno_req = '0';
        zz_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_a at the source
    proc3:process
    begin
        wait until uno_req = '1';
        a_req <= '1' after delay(2,4);
        wait until uno_req = '0';
        a_req <= '0' after delay(2,4);
    end process;

    -- controls latch l_b at the source
    proc4:process
    begin
        wait until uno_req = '1';
        b_req <= '1' after delay(2,4);
        wait until uno_req = '0';
        b_req <= '0' after delay(2,4);
    end process;

    -- controls resource Mult_1
    proc5:process
    begin
      wait until Mult_1_ack = '0' and a_ack = '1' and b_ack = '1' and uno_req = '1';
      Mult_1_req <= '1' after delay(2,4);
      wait until zz_ack = '1';
      Mult_1_req <= '0' after delay(2,4);
      wait until uno_req = '0';
    end process;

end behavioral;
