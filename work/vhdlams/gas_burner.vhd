library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity burner is
end burner;

architecture leakage of burner is
  quantity x: real;                     -- Time spent in state
  quantity y: real;                     -- Total time elapsed
  quantity t: real;                     -- Total leakage time
  signal leak : std_logic := '1';         -- Whether or not burner is leaking

begin

  break x=> 0.0, y=> 0.0, t=> 0.0;      -- Inital conditions

  break x=> 0.0 when leak = '0';
  break x=> 0.0 when leak = '1';

  x'dot == 1.0;
  y'dot == 1.0;

  if leak = '0' use
    t'dot == 0.0;
  elsif leak = '1' use
    t'dot == 1.0;
  end use;

  process
  begin
    wait until x'above(1.0);
    assign(leak,'0',0,0);
    wait until x'above(30.0);
    assign(leak,'1',0,0);
  end process;

end leakage;
