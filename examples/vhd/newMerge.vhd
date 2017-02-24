library ieee;
use ieee.std_logic_1164.all;
use work.channel.all;

entity newMerge is
  port(X, Y : inout channel := init_channel(receiver => timing(1, 2));
       Z : inout channel := init_channel(sender => timing(3, 4)));
end newMerge;

architecture behavior of newMerge is
  signal u : std_logic_vector( 2 downto 0 );
begin
  newMerge : process
  begin
    await_any(X, Y);
    if (probe(X)) then
      receive(X, u);
      send (Z, u);
    else
      receive(Y, u);
      send (Z, u);
    end if;
  end process newMerge;
end behavior;
