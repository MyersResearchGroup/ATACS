library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity Xchan is
  port(x : inout channel := init_channel);
end Xchan;

architecture behavior of Xchan is
  signal a,b : std_logic := '0';
begin
  Xchan : process
  begin
    send(x, a);
    send(x, b);
  end process Xchan;
end behavior;
