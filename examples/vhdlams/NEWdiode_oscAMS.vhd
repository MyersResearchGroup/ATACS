library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity diode is
end diode;

architecture diode_osc of diode is
   quantity Vc : real;   -- TODO: why must be on separate lines
   quantity Il : real;
   quantity watchdogA : real;
   quantity watchdogB : real;
     
begin

   break Vc => span(400000.0,470000.0);
   break Il => span(45000.0,55000.0);     --Initial conditions

   break watchdogA => 0.0 when not Il'above(30000.0) and
     watchdogA'above(1000.0);
   break watchdogB => 0.0 when Il'above(70000.0) and
     watchdogB'above(1000.0);

   watchdogA'dot == 1.0;
   watchdogB'dot == 1.0;
   
   if not Vc'above(10200.0) use
     if not Il'above(4500.0) use
       Vc'dot == -207.0;  
       Il'dot == 29.0;
     elsif Il'above(6700.0) and not Il'above(38200.0) use
       Vc'dot == -5.0;
       Il'dot == 25.0;
     elsif Il'above(40400.0) and not Il'above(103300.0) use
       Vc'dot == 489.0;
       Il'dot == 15.0;
     elsif Il'above(105500.0) use
       Vc'dot == 848.0;
       Il'dot == 8.0;
     end use;
   elsif Vc'above(20400.0) and not Vc'above(163300.0) use
     if not Il'above(45.0) use
       Vc'dot == -875.0;
       Il'dot == 20.0;
     elsif Il'above(6700.0) and not Il'above(38200.0) use
       Vc'dot == -673.0;
       Il'dot == 16.0;
     elsif Il'above(40400.0) and not Il'above(103300.0) use
       Vc'dot == -179.0;
       Il'dot == 6.0;
     elsif Il'above(105500.0) use
--       if b = '1' use
       Vc'dot == 180.0;
       Il'dot == -1.0;
--       end use;
     end use;
   elsif Vc'above(173500.0) and not Vc'above(479600.0) use
     if not Il'above(4500.0) use
       Vc'dot == -287.0;
       Il'dot == -3.0;
     elsif Il'above(6700.0) and not Il'above(38200.0) use
       Vc'dot == -85.0;
       Il'dot == -7.0;
     elsif Il'above(40400.0) and not Il'above(103300.0) use
       Vc'dot == 409.0;
       Il'dot == -17.0;
     elsif Il'above(105500.0) use
--       if b = '1' use
       Vc'dot == 768.0;
       Il'dot == -24.0;
--       end use;
     end use;
   elsif Vc'above(489800.0) use
     if not Il'above(4500.0) use
       Vc'dot == -893.0;
       Il'dot == -20.0;
     elsif Il'above(6700.0) and not Il'above(38200.0) use
       Vc'dot == -690.0;
       Il'dot == -24.0;
     elsif Il'above(40400.0) and not Il'above(103300.0) use
       Vc'dot == -197.0;
       Il'dot == -34.0;
     elsif Il'above(105500.0) use
--       if b = '1' use
       Vc'dot == 163.0;
       Il'dot == -41.0;
--       end use;
     end use;
   end use;

   assert (not watchdogA'above(20000.0) and not watchdogB'above(20000.0)) 
     report "Error: Current not oscillating."
     severity failure;

end diode_osc;
