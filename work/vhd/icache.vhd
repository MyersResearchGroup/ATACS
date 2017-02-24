-------------------- Main  ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity icache is
  port(
    inst_adr : inout channel := init_channel;  -- input : word
    tagcheck : inout channel := init_channel;  -- sync
    outselect : inout channel := init_channel;  -- sync
    inst_out : inout channel := init_channel;  -- output : word
    mmem_rd_addr : inout channel := init_channel;  -- output : mmemRdAddrType
    mmem_rd_data : inout channel := init_channel;  -- input : word
    cmem_rd_addr : inout channel := init_channel;  -- output : line addr
    cmem_rd_data : inout channel := init_channel;  -- input : line
    cmem_wr_ctrl : inout channel := init_channel;  -- output : cmemCtrlType
    cbuf_rd_addr : inout channel := init_channel;  -- output : offset
    cbuf_rd_data : inout channel := init_channel;  -- input : word
    cbuf_wr_ctrl : inout channel := init_channel;  -- output : cbufCtrlType
    cbuf_all_write : inout channel := init_channel;  -- output : line
    tagm_rd_addr : inout channel := init_channel;  -- output : line addr
    tagm_rd_data : inout channel := init_channel;  -- input : tag
    tagm_wr_ctrl : inout channel := init_channel;  -- output : tagmCtrlType
    vlrg_rd_addr : inout channel := init_channel;  -- output : line addr
    vlrg_rd_data : inout channel := init_channel;  -- input : bit
    vlrg_wr_ctrl : inout channel := init_channel;  -- output : vlrgCtrlType
    exrg_rd_addr : inout channel := init_channel;  -- output : offset
    exrg_rd_data : inout channel := init_channel;  -- input : bit
    exrg_set_addr : inout channel := init_channel;  -- output : offset
    existReset : inout channel := init_channel);  -- sync
end icache;

architecture structural of icache is
  component control
    port (
      PC            : inout channel := init_channel;
      tagcheck      : inout channel := init_channel;
      outselect     : inout channel := init_channel;
      inst_out      : inout channel := init_channel;
      checkTagProc  : inout channel := init_channel;
      readCacheProc : inout channel := init_channel;
      cbuf_rd_addr  : inout channel := init_channel;
      cbuf_rd_data  : inout channel := init_channel;
      existCheck    : inout channel := init_channel;
      existOK       : inout channel := init_channel);
  end component;
  component tag_check
    port (
      PC           : inout channel := init_channel;
      TagWrite     : inout channel := init_channel;
      hit          : inout channel := init_channel;
      miss         : inout channel := init_channel;
      tagm_rd_addr : inout channel := init_channel;
      tagm_rd_data : inout channel := init_channel;
      tagm_wr_ctrl : inout channel := init_channel;
      vlrg_rd_addr : inout channel := init_channel;
      vlrg_rd_data : inout channel := init_channel;
      vlrg_wr_ctrl : inout channel := init_channel);
  end component;

  component read_cache
    port (
      PC                 : inout channel := init_channel;
      hit                : inout channel := init_channel;
      miss               : inout channel := init_channel;
      lineFetchPart1Proc : inout channel := init_channel;
      lineFetchComplete  : inout channel := init_channel;
      inst_out           : inout channel := init_channel;
      cbuf_all_write     : inout channel := init_channel;
      cmem_rd_addr       : inout channel := init_channel;
      cmem_rd_data       : inout channel := init_channel);
  end component;

  component fetch
    port (
      PC                : inout channel := init_channel;
      lineFetchComplete : inout channel := init_channel;
      inst_out          : inout channel := init_channel;
      TagWrite          : inout channel := init_channel;
      existWriteAddr    : inout channel := init_channel;
      existReset        : inout channel := init_channel;
      mmem_rd_addr      : inout channel := init_channel;
      mmem_rd_data      : inout channel := init_channel;
      cmem_wr_ctrl      : inout channel := init_channel;
      cbuf_wr_ctrl      : inout channel := init_channel);
  end component;

  component exist
    port (
      existWriteAddr : inout channel := init_channel;
      existCheck     : inout channel := init_channel;
      existOK        : inout channel := init_channel;
      exrg_rd_addr   : inout channel := init_channel;
      exrg_rd_data   : inout channel := init_channel;
      exrg_set_addr  : inout channel := init_channel);
  end component;

  component merge
    port (
      inst_out1, inst_out2, inst_out3 : inout channel := init_channel;
      inst_out                        : inout channel := init_channel);
  end component;
  
  signal checkTagProc : channel := init_channel; -- word
  signal readCacheProc : channel := init_channel; -- word
  signal existOK : channel := init_channel; -- sync
  signal hit : channel := init_channel; -- sync
  signal miss : channel := init_channel; -- sync
  signal lineFetchPart1Proc : channel := init_channel; -- word
  signal lineFetchComplete : channel := init_channel; -- sync
  signal existWriteAddr : channel := init_channel; -- offset
  signal existCheck : channel := init_channel; -- offset
  signal TagWrite : channel := init_channel; -- tagmCtrlType
  signal inst_out1 : channel := init_channel; -- word
  signal inst_out2 : channel := init_channel; -- word
  signal inst_out3 : channel := init_channel; -- word

begin
  controller: control
    port map (
      PC            => inst_adr,
      tagcheck      => tagcheck,
      outselect     => outselect,
      inst_out      => inst_out1,
      checkTagProc  => checkTagProc,
      readCacheProc => readCacheProc,
      cbuf_rd_addr  => cbuf_rd_addr,
      cbuf_rd_data  => cbuf_rd_data,
      existCheck    => existCheck,
      existOK       => existOK);

  tag: tag_check
    port map (
      PC           => checkTagProc,
      TagWrite     => TagWrite,
      hit          => hit,
      miss         => miss,
      tagm_rd_addr => tagm_rd_addr,
      tagm_rd_data => tagm_rd_data,
      tagm_wr_ctrl => tagm_wr_ctrl,
      vlrg_rd_addr => vlrg_rd_addr,
      vlrg_rd_data => vlrg_rd_data,
      vlrg_wr_ctrl => vlrg_wr_ctrl);

  reader: read_cache
    port map (
      PC                 => readCacheProc,
      hit                => hit,
      miss               => miss,
      lineFetchPart1Proc => lineFetchPart1Proc,
      lineFetchComplete  => lineFetchComplete,
      inst_out           => inst_out2,
      cbuf_all_write     => cbuf_all_write,
      cmem_rd_addr       => cmem_rd_addr,
      cmem_rd_data       => cmem_rd_data);

  line_fetch: fetch
    port map (
      PC                => lineFetchPart1Proc,
      lineFetchComplete => lineFetchComplete,
      inst_out          => inst_out3,
      TagWrite          => TagWrite,
      existWriteAddr    => existWriteAddr,
      existReset        => existReset,
      mmem_rd_addr      => mmem_rd_addr,
      mmem_rd_data      => mmem_rd_data,
      cmem_wr_ctrl      => cmem_wr_ctrl,
      cbuf_wr_ctrl      => cbuf_wr_ctrl);

  exrg: exist
    port map (
      existWriteAddr => existWriteAddr,
      existCheck     => existCheck,
      existOK        => existOK,
      exrg_rd_addr   => exrg_rd_addr,
      exrg_rd_data   => exrg_rd_data,
      exrg_set_addr  => exrg_set_addr);
  
  merger: merge
    port map (
      inst_out1 => inst_out1,
      inst_out2 => inst_out2,
      inst_out3 => inst_out3,
      inst_out  => inst_out);
end structural;
