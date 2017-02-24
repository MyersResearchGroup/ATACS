library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity billiards is
end billiards;

architecture balls of billiards is
  
  quantity x: REAL; -- X Coordinate of ball
  quantity y: REAL; -- Y Coordinate of ball
  signal xinc: std_logic := '1'; --is x inc?
  signal yinc: std_logic := '1'; --is y inc?
  
begin

  break x=> 0.0, y=> 0.0; -- Initial conditions


  --The Analog portion
  if xinc = '1' use
    x'dot == 1.0;
  else
    x'dot == -1.0;
  end use;

  if yinc = '1' use
    y'dot == 1.0;
  else
    y'dot == -1.0;
  end use;

  --The Digital portion
  xcoord : process
  begin
    wait until x'above(3.0);
    assign(xinc,'0',0,0);
    wait until not x'above(0.0);
    assign(xinc,'1',0,0);
  end process xcoord;

  ycoord : process
  begin
    wait until y'above(8.0);
    assign(yinc,'0',0,0);
    wait until not y'above(0.0);
    assign(yinc,'1',0,0);
  end process ycoord;

  assert x'above(1.0) or not x'above(1.0) or
         y'above(6.0) or not y'above(6.0)
    report "Reached white ball"
    severity FAILURE;
    
end balls;
