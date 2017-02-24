library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.cache_pack.all;

entity merge is
  port (inst_out1, inst_out2, inst_out3 : inout channel := init_channel;
        inst_out : inout channel := init_channel);
end merge;

architecture merge of merge is
begin  -- merge
-- purpose: merge words from multiple mutually exclusive channels
  merge_proc : process
  begin  -- process merge_proc
    await_any(inst_out1, inst_out2, inst_out3);
    if probe(inst_out1) then
      send(inst_out, data(inst_out1));
      receive(inst_out1);  -- release inst_out1
    elsif probe(inst_out2) then
      send(inst_out, data(inst_out2));
      receive(inst_out2);  -- release inst_out2
    else
      send(inst_out, data(inst_out3));
      receive(inst_out3);  -- release inst_out3
    end if;
  end process merge_proc;
end merge;
