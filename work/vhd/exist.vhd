--------------------- Exist Register Block  ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity exist is
  port(
    existWriteAddr : inout channel := init_channel;  -- input : offset
    existCheck : inout channel := init_channel;  -- input : offset
    existOK : inout channel := init_channel;  -- sync
    exrg_rd_addr : inout channel := init_channel;  -- output : offset
    exrg_rd_data : inout channel := init_channel;  -- input : bit
    exrg_set_addr : inout channel := init_channel);  -- output : offset
end exist;
architecture behavioral of exist is
  signal eg_val : std_logic;
begin
  exist : process
    variable waitAddr : std_logic_vector(offset_t);
    variable waitFlag : boolean := false;
  begin
    await_any(existWriteAddr, existCheck);
    if(probe(existWriteAddr)) then
      send(exrg_set_addr, data(existWriteAddr));
      if true = waitFlag
        and waitAddr = data(existWriteAddr)(normal_offset_t) then
	send(existOK);  -- ||
        waitFlag := false;
--      else continue  -- this is necesssary for Case1
      end if;
      receive(existWriteAddr);  -- release existWriteAddr
    else  -- existCheck then
      send(exrg_rd_addr, data(existCheck));  -- ||
      receive(exrg_rd_data, eg_val);
      if eg_val = '1' then
        send(existOK);
      else
        waitAddr := data(existCheck)(normal_offset_t);  -- ||
        waitFlag := true;
      end if;
      receive(existCheck);  -- release existCheck
    end if;
  end process;
end behavioral;
