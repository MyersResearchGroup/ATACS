library ieee;
use ieee.std_logic_1164.all;
use work.channel.all;

entity merge is
  port(X, Y : inout channel := init_channel(receiver => timing(1, 2));
       Z : inout channel := init_channel(sender => timing(3, 4)));
end merge;

architecture behavior of merge is
  signal u : std_logic_vector( 2 downto 0 );
begin
  merge : process
  begin
    await_any(X, Y);
    if (probe(X)) then
      receive(X, u);
    else
      receive(Y, u);
    end if;
    send (Z, u);
  end process merge;
end behavior;
