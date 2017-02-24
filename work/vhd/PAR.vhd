library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.channel.all;

entity PAR is
  port(A : inout channel := init_channel(receiver=>timing(1,2));
       B,C : inout channel := init_channel(sender=>timing(1,2)));
end PAR;

architecture behavior of PAR is
  signal bb,cc : std_logic_vector( 2 downto 0 );
begin
  doPAR : process
  begin
    await(A);
    send(B,bb,C,cc);
    receive(A);
  end process doPAR;
end behavior;
