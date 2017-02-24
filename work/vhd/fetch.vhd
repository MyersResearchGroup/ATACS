-------------------- Cache Line Fetch Block  ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity fetch is
  port(
    PC : inout channel := init_channel;  -- input : word
    lineFetchComplete : inout channel := init_channel;  -- sync
    inst_out : inout channel := init_channel;  -- output : word
    TagWrite : inout channel := init_channel;  -- output : tagmCtrlType
    existWriteAddr : inout channel := init_channel;  -- output : offset
    existReset : inout channel := init_channel;  -- sync
    mmem_rd_addr : inout channel := init_channel;  -- output : mmemRdAddrType
    mmem_rd_data : inout channel := init_channel;  -- input : word
    cmem_wr_ctrl : inout channel := init_channel;  -- output : cmemCtrlType
    cbuf_wr_ctrl : inout channel := init_channel);  -- output : cbufCtrlType
end fetch;
architecture behavioral of fetch is
  signal addr_counter, add_tmp : std_logic_vector(offset_t);
  signal mbuf, CNT : word_t;
begin
  fetch : process
    variable count, count_tmp : counter_t;
    constant one : std_logic_vector(offset_t):=(offset_t'low=>'1',others=>'0');
  begin
    send(existReset);
    loop
      await(PC);
      send(existReset);  -- ||
      addr_counter <= (data(PC)(offset_t) + one);  -- ||
      count := 7;  -- ||
      --begin
      send(mmem_rd_addr, data(PC)(mmem_rd_addr_t));--||
      receive(mmem_rd_data, mbuf);
      send(inst_out, mbuf,
           cmem_wr_ctrl, mbuf & data(PC)(addr_offset_t),
           cbuf_wr_ctrl, mbuf & data(PC)(offset_t));
      send(existWriteAddr, data(PC)(offset_t));
      --end;  -- ||
      receive(PC, CNT);
      while count > 0 loop
        add_tmp <= (addr_counter + one);  -- ||
        count_tmp := count - 1;  -- ||
        --begin
        send(mmem_rd_addr, addr_counter & CNT(line_frame_adr_t));  --||
        receive(mmem_rd_data, mbuf);
        send(cmem_wr_ctrl, mbuf & CNT(line_adr_t) & addr_counter,
             cbuf_wr_ctrl, mbuf & addr_counter);
        send(existWriteAddr, addr_counter);
        --end;
        addr_counter <= add_tmp;  -- ||
        count := count_tmp;
      end loop;  -- ||
      send(TagWrite, CNT(line_frame_adr_t));
      send(lineFetchComplete);
    end loop;
  end process;
end behavioral;
