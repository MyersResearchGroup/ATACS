library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity temperature is
end temperature;

architecture monitor of temperature is
  
  quantity t: real;      -- Temperature
  quantity timer1: real; -- Rod1 Timer
  quantity timer2: real; -- Rod2 Timer
  signal x1 : std_logic := '1'; -- Rod1 = '1' means it's ready for use
  signal x2 : std_logic := '1'; -- Rod2 = '1' means it's ready for use
  signal ratebool1 : std_logic := '0';
  signal ratebool2 : std_logic := '0';
  begin

    break t=> 0.0, timer1=> 80.0, timer2=> 80.0; -- Initial conditions

    break timer1 => 0.0 when ratebool1 = '1' and not t'above(250.0);
    break timer2 => 0.0 when ratebool2 = '1' and not t'above(250.0);

    timer1'dot == 1.0;
    timer2'dot == 1.0;

    if ratebool1 = '0' use
      if ratebool2 = '0' use
        t'dot == 32.0;
      else
        t'dot == -10.0;
      end use;     
    else
      if ratebool2 = '0' use
        t'dot == -25.0;
      else
        t'dot == 0.0;
      end use;
    end use;

    --elsif ratebool1 = '1' and ratebool2 = '0' use
     -- t'dot == -25.0;
    --elsif ratebool1 = '0' and ratebool2 = '1' use

    --elsif ratebool1 = '1' and ratebool2 = '1' use
     -- t'dot == 0.0; --this is shutdown
    --end use;

    rods : process
    begin
      wait until t'above(1100.0);
      if timer1'above(80.0) then
        assign(ratebool1,'1',0,0);
        wait until not t'above(250.0) and not timer1'above(80.0);
        assign(ratebool1,'0',0,0);
      elsif timer2'above(80.0) then
        assign(ratebool2,'1',0,0);
        wait until not t'above(250.0) and not timer2'above(80.0);
        assign(ratebool2,'0',0,0);
      else
        assign(ratebool2,'1',0,0);
        assign(ratebool1,'1',0,0);
      end if;
      
    end process rods;

    assert ratebool1='0' or ratebool2='0'
      report "Reactor shuts down"
      severity failure;
      
end monitor;
