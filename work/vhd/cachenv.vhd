-------------------- Main  ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.cache_pack.all;

entity cachenv is
  port(
    inst_adr : inout channel := init_channel;  -- output : word
    tagcheck : inout channel := init_channel;  -- sync
    outselect : inout channel := init_channel;  -- sync
    inst_out : inout channel := init_channel;  -- input : word
    mmem_rd_addr : inout channel := init_channel;  -- input : mmemRdAddrType
    mmem_rd_data : inout channel := init_channel;  -- output : word
    cmem_rd_addr : inout channel := init_channel;  -- input : line addr
    cmem_rd_data : inout channel := init_channel;  -- output : line
    cmem_wr_ctrl : inout channel := init_channel;  -- input : cmemCtrlType
    cbuf_rd_addr : inout channel := init_channel;  -- input : offset
    cbuf_rd_data : inout channel := init_channel;  -- output : word
    cbuf_wr_ctrl : inout channel := init_channel;  -- input : cbufCtrlType
    cbuf_all_write : inout channel := init_channel;  -- input : line
    tagm_rd_addr : inout channel := init_channel;  -- input : line addr
    tagm_rd_data : inout channel := init_channel;  -- output : tag
    tagm_wr_ctrl : inout channel := init_channel;  -- input : tagmCtrlType
    vlrg_rd_addr : inout channel := init_channel;  -- input : line addr
    vlrg_rd_data : inout channel := init_channel;  -- output : bit
    vlrg_wr_ctrl : inout channel := init_channel;  -- input : vlrgCtrlType
    exrg_rd_addr : inout channel := init_channel;  -- input : offset
    exrg_rd_data : inout channel := init_channel;  -- output : bit
    exrg_set_addr : inout channel := init_channel;  -- input : offset
    existReset : inout channel := init_channel);  -- sync
end cachenv;

architecture behavioral of cachenv is
begin
  signal address : std_logic_vector(mmem_rd_addr_t);  -- address bus
  signal cmem_adr : std_logic_vector(line_adr_t);  -- address of line in cache
  signal cbuf_adr : std_logic_vector(offset_t);  -- address of word in line
  signal cmem_control : std_logic_vector(cmem_ctrl_t);

  -- purpose: main memory
  mmem: process
    variable memory:array(0 to words_per_memory-1) of std_logic_vector(word_t);
  begin  -- process mmem
    receive(mmem_rd_addr, address);
    send(mmem_rd_data,memory(conv_integer(address)));
  end process mmem;

  -- purpose: cache memory
  cmem: process
    variable cache: array (0 to lines_per_cache-1) of std_logic_vector(line_t);
    variable buf : std_logic_vector(line_t);
    variable msb, lsb : line_t := 0;    -- indecies into temp
  begin  -- process cmem
    await_any(cmem_rd_addr,cmem_wr_ctrl);
    if probe(cmem_rd_adr) then
      receive(cmem_rd_addr, cmem_adr);
      send(cmem_rd_data,cache(conv_integer(cmem_adr)));
    else
      receive(cmem_wr_ctrl, cmem_control);
      buf := cache(conv_integer(cmem_control(cmem_adr_t)));
      lsb := conv_integer(cmem_control(cmem_offset_t)) * word_size;
      msb := lsb + word_size - 1;
      buf(msb downto lsb) := cmem_control(word_t);
      cache(conv_integer(cmem_control(cmem_adr_t))) := buf;
    end if;
  end process cmem;

  -- purpose: cache buffer
  cbuf: process
    variable buf: array (0 to words_per_line - 1) of std_logic_vector(word_t);
  begin  -- process cbuf
    receive(cbuf_rd_addr, cbuf_adr);
    send(cbuf_rd_data,buf(conv_integer(cbuf_adr)));
  end process cbuf;

end behavioral;
