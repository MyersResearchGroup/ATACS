library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
use work.channel.all;
use work.defs.all;

entity cache_test is
  port (
    mmem_wr_ctrl : inout channel := init_channel;   -- output mmemCtrlType
    inst_adr     : inout channel := init_channel;   -- output word
    tagcheck     : inout channel := init_channel;   -- sync
    outselect    : inout channel := init_channel;   -- sync
    inst_out     : inout channel := init_channel);  -- input word
end cache_test;

architecture behavioral of cache_test is
begin  -- behavioral

  -- purpose: produce test data
  producer : process
    variable x : boolean := false;
    constant sync : std_logic_vector := "0";  -- dummy value
  begin  -- process producer
    send(mmem_wr_ctrl, mmemCtrlType ( 0, 0));
    send(mmem_wr_ctrl, mmemCtrlType ( 1, 10));
    send(mmem_wr_ctrl, mmemCtrlType ( 2, 20));
    send(mmem_wr_ctrl, mmemCtrlType ( 3, 30));
    send(mmem_wr_ctrl, mmemCtrlType ( 4, 40));
    send(mmem_wr_ctrl, mmemCtrlType ( 5, 50));
    send(mmem_wr_ctrl, mmemCtrlType ( 6, 60));
    send(mmem_wr_ctrl, mmemCtrlType ( 7, 70));
    send(mmem_wr_ctrl, mmemCtrlType ( 8, 5));
    send(mmem_wr_ctrl, mmemCtrlType ( 9, 15));
    send(mmem_wr_ctrl, mmemCtrlType (10, 25));
    send(mmem_wr_ctrl, mmemCtrlType (11, 35));
    send(mmem_wr_ctrl, mmemCtrlType (12, 45));
    send(mmem_wr_ctrl, mmemCtrlType (13, 55));
    send(mmem_wr_ctrl, mmemCtrlType (14, 65));
    send(mmem_wr_ctrl, mmemCtrlType (15, 75));
    if true = x then
      send(inst_adr, conv_std_logic_vector(4,word_size), outselect, sync);
    else
      send(inst_adr, conv_std_logic_vector(4,word_size), tagcheck, sync);
    end if;
    send(inst_adr, conv_std_logic_vector(4,word_size), outselect, sync);
    send(inst_adr, conv_std_logic_vector(8,word_size), outselect, sync);
    send(inst_adr, conv_std_logic_vector(12,word_size), outselect, sync);
    send(inst_adr, conv_std_logic_vector(16,word_size), outselect, sync);
  end process producer;

  -- purpose: consume instructions
  consumer : process
  begin  -- process consumer
    receive(inst_out);
  end process consumer;
end behavioral;
