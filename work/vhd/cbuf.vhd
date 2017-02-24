-------------------- Cbuf Block ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity cbuf is
  port(
    cbuf_all_write : inout channel := init_channel;  --input line
    cbuf_wr_ctrl : inout channel := init_channel;  --input cbufCtrlType
    cbuf_rd_addr : inout channel := init_channel;  --input offset
    cbuf_rd_data : inout channel := init_channel);  --output word_t
end cbuf;

architecture behavioral of cbuf is
  signal Cbuf : std_logic_vector(line_t);
begin
  process
    variable lsb, msb : line_t;
  begin
    await_any(cbuf_all_write,cbuf_wr_ctrl,cbuf_rd_addr);
    if probe(cbuf_all_write) then
      receive(cbuf_all_write,Cbuf);
    elsif probe(cbuf_wr_ctrl) then
      lsb := data(cbuf_wr_ctrl,offset_size) * word_size;
      msb := lsb + word_size - 1;
      Cbuf(msb downto lsb) <= data(cbuf_wr_ctrl)(cbuf_wr_data_t);
      receive(cbuf_wr_ctrl);  -- release cbuf_wr_ctrl
    elsif probe(cbuf_rd_addr) then
      lsb := data(cbuf_rd_addr,offset_size) * word_size;
      msb := lsb + word_size - 1;
      send(cbuf_rd_data,Cbuf(msb downto lsb));
      receive(cbuf_rd_addr);  -- release cbuf_rd_addr
    end if;
  end process;
end behavioral;
