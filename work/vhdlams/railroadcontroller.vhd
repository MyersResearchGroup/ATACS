library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity railroad is
end railroad;

architecture controller of railroad is
  quantity x: real;                     -- distance between gate and train
  quantity g: real;                     -- position of gate
  signal app : std_logic := '0';        -- train approaching gate
  signal ex : std_logic := '0';         -- train leaving gate
  signal raise : std_logic := '0';      -- raise gate
  signal lower : std_logic := '0';      -- lower gate

begin

    break x=> 2000.0, g=> 90.0;         -- Initial condition

   
    if app = '0' and ex = '0' use
      x'dot == span(-50.0,-40.0);
    elsif app = '0' and ex = '1' use
      x'dot == span(30.0,50.0);
    elsif app = '1' and ex = '0' use
      x'dot == span(-50.0,-30.0);
    end use;

    if raise = '0' and lower = '0' use
      g'dot == (0.0);
    elsif raise = '1' and lower = '0' use 
      g'dot == 9.0;
    elsif lower = '1' and raise = '0' use
      g'dot == -9.0;
    end use;

    process
    begin
      wait until x'above(100.0) and not x'above(1000.0);
      assign(app,'1',0,0);
      wait until not x'above(0.0);
      assign(ex,'1',0,0);
      assign(app,'0',0,0);
      wait until x'above(2000.0);
      assign(ex,'0',0,0);
    end process;

    process
    begin
     guard(app,'1');
     assign(lower,'1',0,9);
     wait until not g'above(0.0);
     assign(lower,'0',0,0);
     wait until x'above(1000.0);
     assign(raise,'1',0,9);
     wait until g'above(90.0);
     assign(raise,'0',0,0);
    end process;

end controller;
