-------------------- Tag Memory Block  ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity tag_check is
  port(
    PC : inout channel := init_channel;  -- input : word
    TagWrite : inout channel := init_channel;  -- input : tagmCtrlType
    hit : inout channel := init_channel;  -- sync
    miss : inout channel := init_channel;  -- sync
    tagm_rd_addr : inout channel := init_channel;  -- output : line addr
    tagm_rd_data : inout channel := init_channel;  -- input : tag
    tagm_wr_ctrl : inout channel := init_channel;  -- output : tagmCtrlType
    vlrg_rd_addr : inout channel := init_channel;  -- output : line addr
    vlrg_rd_data : inout channel := init_channel;  -- input : bit
    vlrg_wr_ctrl : inout channel := init_channel);  -- output : vlrgCtrlType
end tag_check;
architecture behavioral of tag_check is
  signal tag : std_logic_vector(tag_t);
  signal valid : std_logic_vector(0 downto 0);
begin
  tag_memory : process
  begin
    await_any(PC, TagWrite);
    if(probe(PC)) then
      send(tagm_rd_addr, data(PC)(line_adr_t),
           vlrg_rd_addr, data(PC)(line_adr_t));  -- ||
      receive(tagm_rd_data, tag, vlrg_rd_data, valid);
      if valid="1" and (tag = data(PC)(tag_t)) then
	send(hit);
      else
        send(miss);  -- Finish when target word is read
      end if;
      receive(PC);  -- release PC
    else
      send(tagm_wr_ctrl, data(TagWrite),
           vlrg_wr_ctrl, '1' & data(TagWrite)(normal_line_adr_t));
      receive(TagWrite);  -- release TagWrite
    end if;
  end process;
end behavioral;
