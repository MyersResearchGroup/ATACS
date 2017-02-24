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
  signal rate : integer := 0;
  begin

    break t=> 0.0, timer1=> 80.0, timer2=> 80.0; -- Initial conditions

    break timer1 => 0.0 when rate = 1 and not t'above(250.0);
    break timer2 => 0.0 when rate = 2 and not t'above(250.0);

    timer1'dot == 1.0;
    timer2'dot == 1.0;

    if rate = 0 use
      t'dot == 32.0;
    elsif rate = 1 use
      t'dot == -25.0;
    elsif rate = 2 use
      t'dot == -10.0;
    elsif rate = 3 use
      t'dot == 0.0; --this is shutdown
    end use;

    rods : process
    begin
      wait until t'above(1100.0);
      if timer1'above(80.0) then
        rate <= 1;
        wait until not t'above(250.0);
        rate <= 0;
      elsif timer2'above(80.0) then
        rate <= 2;
        wait until not t'above(250.0);
        rate <= 0;
      else
        rate <= 3;
      end if;
      
    end process rods;
      
end monitor;
