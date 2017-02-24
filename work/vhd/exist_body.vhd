-------------------- Exist Register Block ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;
use work.defs.all;

entity exist_body is
  port(
    exrg_rd_addr : inout channel := init_channel;  --input offset
    exrg_rd_data : inout channel := init_channel;  --output bit
    exrg_set_addr : inout channel := init_channel;  --input offset
    existReset : inout channel := init_channel);  --sync
end exist_body;

architecture behavioral of exist_body is
begin
  process
    variable ExtReg : std_logic_vector(words_per_line - 1 downto 0);
  begin
    await_any(exrg_rd_addr,exrg_set_addr,existReset);
    if probe(exrg_rd_addr) then
      send(exrg_rd_data,ExtReg(data(exrg_rd_addr,offset_size)));
      receive(exrg_rd_addr);  -- release exrg_rd_addr
    elsif probe(exrg_set_addr) then
      ExtReg(data(exrg_set_addr,offset_size)) := '1';
      receive(exrg_set_addr);  -- release exrg_set_addr
    elsif probe(existReset) then
      ExtReg := (others => '0');
      receive(existReset);              -- complete sync on existReset
    end if;
  end process;
end behavioral;
