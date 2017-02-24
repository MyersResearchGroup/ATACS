library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity mutex is
end mutex;

architecture protocol of mutex is
  quantity x: real;                     -- Process 1's clock
  quantity y: real;                     -- Process 2's clock
  signal kx : std_logic := '0';         -- If k wants its turn
  signal ky : std_logic := '0';         -- If y wants its turn
  --constant a : real := 7.0;             -- Max delay time to register intent
  --constant b : real := 11.0;            -- Min time to delay before rechecking
  --constant c : integer := 30;           -- General big integer
  --constant d : real := 30.0;            -- General big real number
  signal xab : std_logic := '0';        -- Resets x clock
  signal yab : std_logic := '0';        -- like xab, but for process 2
  signal cs1 : std_logic := '0';        -- Process 1 in critical section
  signal cs2 : std_logic := '0';        -- Process 2 in critical section
  
begin
   
  break x=> 0.0, y=> 0.0;               -- Initial conditions

  x'dot == span(0.8,1.0);
  y'dot == span(1.0,1.1);

  break x=> 0.0 when xab = '0';
  break x=> 0.0 when xab = '1';
  break y=> 0.0 when yab = '0';
  break y=> 0.0 when yab = '1';

  proc1 : process
  begin
    guard(ky,'0');
    wait for delay(0,30);
    assign(xab,'1',0,0);
    wait until x'above(7.0);
    assign(kx,'1',0,0);
    assign(xab,'0',0,0);
    wait until x'above(11.0);
    if (ky = '0') then
	assign(cs1,'1',0,0);
        assign(cs1,'0',0,30);
        assign(kx,'0',0,0);
    else
        assign(kx,'0',0,0);  
    end if;
  end process proc1;

  proc2 : process
  begin
    guard(kx,'0');
    wait for delay(0,30);
    assign(yab,'1',0,0);
    wait until y'above(7.0);
    assign(ky,'1',0,0);
    assign(yab,'0',0,0);
    wait until y'above(11.0);
    if (kx = '0') then
	assign(cs2,'1',0,0);
        assign(cs2,'0',0,30);
        assign(ky,'0',0,0);
    else
        assign(ky,'0',0,0);
    end if;
  end process proc2;
    
end protocol;
