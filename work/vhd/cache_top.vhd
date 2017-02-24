-------------------- Main  ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity cache_top is
end cache_top;

architecture structural of cache_top is
  component cache_test
    port (
      mmem_wr_ctrl : inout channel := init_channel;
      inst_adr     : inout channel := init_channel;
      tagcheck     : inout channel := init_channel;
      outselect    : inout channel := init_channel;
      inst_out     : inout channel := init_channel); 
  end component;

  component icache
    port (
      inst_adr       : inout channel := init_channel;
      tagcheck       : inout channel := init_channel;
      outselect      : inout channel := init_channel;
      inst_out       : inout channel := init_channel;
      mmem_rd_addr   : inout channel := init_channel;
      mmem_rd_data   : inout channel := init_channel;
      cmem_rd_addr   : inout channel := init_channel;
      cmem_rd_data   : inout channel := init_channel;
      cmem_wr_ctrl   : inout channel := init_channel;
      cbuf_rd_addr   : inout channel := init_channel;
      cbuf_rd_data   : inout channel := init_channel;
      cbuf_wr_ctrl   : inout channel := init_channel;
      cbuf_all_write : inout channel := init_channel;
      tagm_rd_addr   : inout channel := init_channel;
      tagm_rd_data   : inout channel := init_channel;
      tagm_wr_ctrl   : inout channel := init_channel;
      vlrg_rd_addr   : inout channel := init_channel;
      vlrg_rd_data   : inout channel := init_channel;
      vlrg_wr_ctrl   : inout channel := init_channel;
      exrg_rd_addr   : inout channel := init_channel;
      exrg_rd_data   : inout channel := init_channel;
      exrg_set_addr  : inout channel := init_channel;
      existReset     : inout channel := init_channel);
  end component;
  
  component memory
    generic (
      address_size : natural;
      data_size    : natural);
    port (
      read_address  : inout channel := init_channel;
      read_data     : inout channel := init_channel;
      write_control : inout channel := init_channel);
  end component;

  component exist_body
    port (
      exrg_rd_addr  : inout channel := init_channel;
      exrg_rd_data  : inout channel := init_channel;
      exrg_set_addr : inout channel := init_channel;
      existReset    : inout channel := init_channel);
  end component;
  
  component cmem
    port (
      cmem_rd_addr : inout channel := init_channel;
      cmem_rd_data : inout channel := init_channel;
      cmem_wr_ctrl : inout channel := init_channel);
  end component;

  component cbuf
    port (
      cbuf_all_write : inout channel := init_channel;
      cbuf_wr_ctrl   : inout channel := init_channel;
      cbuf_rd_addr   : inout channel := init_channel;
      cbuf_rd_data   : inout channel := init_channel);
  end component;
  
  signal inst_adr       : channel := init_channel;
  signal tagcheck       : channel := init_channel;
  signal outselect      : channel := init_channel;
  signal inst_out       : channel := init_channel;
  signal mmem_rd_addr   : channel := init_channel;
  signal mmem_rd_data   : channel := init_channel;
  signal mmem_wr_ctrl   : channel := init_channel;
  signal cmem_rd_addr   : channel := init_channel;
  signal cmem_rd_data   : channel := init_channel;
  signal cmem_wr_ctrl   : channel := init_channel;
  signal cbuf_rd_addr   : channel := init_channel;
  signal cbuf_rd_data   : channel := init_channel;
  signal cbuf_wr_ctrl   : channel := init_channel;
  signal cbuf_all_write : channel := init_channel;
  signal tagm_rd_addr   : channel := init_channel;
  signal tagm_rd_data   : channel := init_channel;
  signal tagm_wr_ctrl   : channel := init_channel;
  signal vlrg_rd_addr   : channel := init_channel;
  signal vlrg_rd_data   : channel := init_channel;
  signal vlrg_wr_ctrl   : channel := init_channel;
  signal exrg_rd_addr   : channel := init_channel;
  signal exrg_rd_data   : channel := init_channel;
  signal exrg_set_addr  : channel := init_channel;
  signal existReset     : channel := init_channel;

begin  -- structural
  tester: cache_test
    port map (
      mmem_wr_ctrl => mmem_wr_ctrl,
      inst_adr     => inst_adr,
      tagcheck     => tagcheck,
      outselect    => outselect,
      inst_out     => inst_out);

  cache : icache
    port map (
      inst_adr       => inst_adr,
      tagcheck       => tagcheck,
      outselect      => outselect,
      inst_out       => inst_out,
      mmem_rd_addr   => mmem_rd_addr,
      mmem_rd_data   => mmem_rd_data,
      cmem_rd_addr   => cmem_rd_addr,
      cmem_rd_data   => cmem_rd_data,
      cmem_wr_ctrl   => cmem_wr_ctrl,
      cbuf_rd_addr   => cbuf_rd_addr,
      cbuf_rd_data   => cbuf_rd_data,
      cbuf_wr_ctrl   => cbuf_wr_ctrl,
      cbuf_all_write => cbuf_all_write,
      tagm_rd_addr   => tagm_rd_addr,
      tagm_rd_data   => tagm_rd_data,
      tagm_wr_ctrl   => tagm_wr_ctrl,
      vlrg_rd_addr   => vlrg_rd_addr,
      vlrg_rd_data   => vlrg_rd_data,
      vlrg_wr_ctrl   => vlrg_wr_ctrl,
      exrg_rd_addr   => exrg_rd_addr,
      exrg_rd_data   => exrg_rd_data,
      exrg_set_addr  => exrg_set_addr,
      existReset     => existReset);

  tagm : memory
    generic map (
      address_size => addr_size,
      data_size    => tag_size)
    port map (
      read_address  => tagm_rd_addr,
      read_data     => tagm_rd_data,
      write_control => tagm_wr_ctrl);

  vlrg: memory
    generic map (
      address_size => addr_size,
      data_size    => 1)
    port map (
      read_address  => vlrg_rd_addr,
      read_data     => vlrg_rd_data,
      write_control => vlrg_wr_ctrl);
  
  mmem: memory
    generic map (
      address_size => mmem_addr_size,
      data_size    => word_size)
    port map (
      read_address  => mmem_rd_addr,
      read_data     => mmem_rd_data,
      write_control => mmem_wr_ctrl);
  
  exrg: exist_body
    port map (
      exrg_rd_addr  => exrg_rd_addr,
      exrg_rd_data  => exrg_rd_data,
      exrg_set_addr => exrg_set_addr,
      existReset    => existReset);

  cache_memory: cmem
    port map (
      cmem_rd_addr => cmem_rd_addr,
      cmem_rd_data => cmem_rd_data,
      cmem_wr_ctrl => cmem_wr_ctrl);

  cache_buffer: cbuf
    port map (
      cbuf_all_write => cbuf_all_write,
      cbuf_wr_ctrl   => cbuf_wr_ctrl,
      cbuf_rd_addr   => cbuf_rd_addr,
      cbuf_rd_data   => cbuf_rd_data);
end structural;
