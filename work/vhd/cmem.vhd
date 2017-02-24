-------------------- Cache Memory Block ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.cache_pack.all;

entity cmem is
  port(
    cmem_rd_addr : inout channel := init_channel;  --input addr
    cmem_rd_data : inout channel := init_channel;  --output line
    cmem_wr_ctrl : inout channel := init_channel);  --input cmemCtrlType
end cmem;

architecture behavioral of cmem is
begin
  process
    variable Cmem : std_logic_vector(cache_bit_index_t);
    variable lsb, msb : cache_bit_index_t;
  begin
    await_any(cmem_rd_addr,cmem_wr_ctrl);
    if probe(cmem_rd_addr) then
      lsb := data(cmem_rd_addr,addr_size) * line_size;
      msb := lsb + line_size - 1;
      send(cmem_rd_data,Cmem(msb downto lsb));
      receive(cmem_rd_addr);            -- release cmem_rd_addr
    elsif probe(cmem_wr_ctrl) then
      lsb := data(cmem_wr_ctrl,cmem_addr_size) * word_size;
      msb := lsb + word_size - 1;
      Cmem(msb downto lsb) := data(cmem_wr_ctrl)(cmem_wr_data_t);
      receive(cmem_wr_ctrl);            -- release cmem_wr_ctrl
    end if;
  end process;
end behavioral;
