-------------------- Memory ---------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.channel.all;

entity memory is
  generic (
    address_size : natural := 1;   -- width of address in bits
    data_size    : natural := 1);  -- width of each datum in bits
  port (
    read_address  : inout channel := init_channel;
    read_data     : inout channel := init_channel;
    write_control : inout channel := init_channel);
end memory;

architecture behavioral of memory is
  type mem_t is array (0 to 2**address_size - 1)
    of std_logic_vector(data_size - 1 downto 0);
begin
  process
    variable memory : mem_t;
  begin
    await_any(read_address, write_control);
    if probe(read_address) then
      send(read_data, memory(data(read_address, address_size)));
      receive(read_address);  -- release read_address
    elsif probe(write_control) then
      memory(data(write_control, address_size)) :=
        data(write_control)(data_size + address_size - 1 downto address_size);
      receive(write_control);  -- release write_control
    end if;
  end process;
end behavioral;
