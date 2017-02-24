-------------------- Cache Memory Read Block  ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity read_cache is
  port(
    PC : inout channel := init_channel;  -- input : word
    hit : inout channel := init_channel;  -- sync
    miss : inout channel := init_channel;  -- sync
    lineFetchPart1Proc : inout channel := init_channel;  -- output : word
    lineFetchComplete : inout channel := init_channel;  -- sync
    inst_out : inout channel := init_channel;  -- output : word
    cbuf_all_write : inout channel := init_channel;  -- output : line
    cmem_rd_addr : inout channel := init_channel;  -- output : line addr
    cmem_rd_data : inout channel := init_channel);  -- input : line
end read_cache;

architecture behavioral of read_cache is
  signal cbuf : std_logic_vector(line_t);  --local variable cbuf : line
begin
  cache_read : process
    variable line_fetch_continue : boolean := false;
    variable msb, lsb : line_t := 0;    -- indecies into cbuf
  begin
    await(PC);
    send(cmem_rd_addr, data(PC)(line_adr_t));  -- ||
    receive(cmem_rd_data, cbuf);
    await_any(hit, miss);
    if(probe(hit)) then
      lsb := conv_integer(data(PC)(offset_t)) * word_size;
      msb := lsb + word_size - 1;
      send(cbuf_all_write, cbuf, inst_out, cbuf(msb downto lsb));
      receive(hit);  -- complete sync on hit
    else
      send(lineFetchPart1Proc, data(PC));  -- ||
      line_fetch_continue := true;
      receive(miss);  -- complete sync on miss
    end if;
    receive(PC);  -- release PC
    if true = line_fetch_continue then
      await(lineFetchComplete);
      line_fetch_continue := false;
      receive(lineFetchComplete);  --complete sync on lineFetchComplete
    end if;
  end process;
end behavioral;
