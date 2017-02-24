library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity Xchan is
  port(x : inout channel := init_channel(sender => timing(1, 2)));
end Xchan;

architecture behavior of Xchan is
  signal data : std_logic := '0';
begin
  Xchan : process
  begin
    send(x, data);
  end process Xchan;
end behavior;
