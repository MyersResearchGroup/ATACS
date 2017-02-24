-------------------- Controller Block ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.cache_pack.all;

entity control is
  port(
    PC : inout channel := init_channel;  -- input : word
    tagcheck : inout channel := init_channel;  -- sync
    outselect : inout channel := init_channel;  -- sync
    inst_out : inout channel := init_channel;  -- output : word
    checkTagProc : inout channel := init_channel;  -- output : word
    readCacheProc : inout channel := init_channel;  -- output : word
    cbuf_rd_addr : inout channel := init_channel;  -- output : offset
    cbuf_rd_data : inout channel := init_channel;  -- input : word
    existCheck : inout channel := init_channel;  -- output : offset
    existOK : inout channel := init_channel);  -- sync
end control;

architecture behavioral of control is
begin
  control : process
  begin
    await(PC);
    await_any(tagcheck, outselect);
    if(probe(tagcheck)) then
      send(checkTagProc, data(PC),  -- Finish when tag check finishes
           -- (on miss, when target word is read)
           readCacheProc, data(PC));  -- Finish when cache is read
      receive(tagcheck);  -- complete sync on tagcheck
    else
      send(existCheck, data(PC)(offset_t));  -- ||
      await(existOK);
      send(cbuf_rd_addr, data(PC)(offset_t));  -- ||
      await(cbuf_rd_data);
      send(inst_out, data(cbuf_rd_data));
      receive(cbuf_rd_data);  -- release cbuf_rd_data
      receive(existOK);  -- complete sync on existOK
    end if;
    receive(PC);  -- release PC
  end process;
end behavioral;
