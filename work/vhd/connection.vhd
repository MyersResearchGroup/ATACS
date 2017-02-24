library ieee;
use ieee.std_logic_1164.all;
use work.channel.all;

entity connection is
end connection;

architecture structure of connection is
  component left
    port(x : inout channel);
  end component;

  component right
    port(y : inout channel);
  end component;

  signal L : channel := init_channel(timing(2,4));
begin
  myX : left port map(x => L);
  myY : right port map(y => L);
end structure;
