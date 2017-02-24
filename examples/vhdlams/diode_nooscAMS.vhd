library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity diode is
end diode;

architecture diode_osc of diode is
   quantity Vc : real;   -- TODO: why must be on separate lines
   quantity Il : real;
   signal a,b : std_logic := '0';
     
begin

   break Vc => span(4000.0,4700.0);
   break Il => span(450.0,550.0);     --Initial conditions

   if not Vc'above(102.0) use
     if not Il'above(45.0) use
       Vc'dot == -207.0;  
       Il'dot == 29.0;
     elsif Il'above(67.0) and not Il'above(382.0) use
       Vc'dot == -5.0;
       Il'dot == 24.0;
     elsif Il'above(404.0) and not Il'above(1033.0) use
       Vc'dot == 489.0;
       Il'dot == 12.0;
     elsif Il'above(1055.0) use
       Vc'dot == 848.0;
       Il'dot == 3.0;
     end use;
   elsif Vc'above(204.0) and not Vc'above(1633.0) use
     if not Il'above(45.0) use
       Vc'dot == -875.0;
       Il'dot == 20.0;
     elsif Il'above(67.0) and not Il'above(382.0) use
       Vc'dot == -673.0;
       Il'dot == 15.0;
     elsif Il'above(404.0) and not Il'above(1033.0) use
       Vc'dot == -179.0;
       Il'dot == 4.0; -- 3.0
     elsif Il'above(1055.0) use
       Vc'dot == 180.0;
       Il'dot == -5.0;
     end use;
   elsif Vc'above(1735.0) and not Vc'above(4796.0) use
     if not Il'above(45.0) use
       Vc'dot == -287.0;
       Il'dot == -3.0;
     elsif Il'above(67.0) and not Il'above(382.0) use
       Vc'dot == -85.0;
       Il'dot == -8.0;
     elsif Il'above(404.0) and not Il'above(1033.0) use
       Vc'dot == 409.0;
       Il'dot == -20.0;
     elsif Il'above(1055.0) use
       Vc'dot == 768.0;
       Il'dot == -29.0;
     end use;
   elsif Vc'above(4898.0) use
     if not Il'above(45.0) use
       Vc'dot == -893.0;
       Il'dot == -19.0; -- -20.0;
     elsif Il'above(67.0) and not Il'above(382.0) use
       Vc'dot == -690.0;
       Il'dot == -25.0;
     elsif Il'above(404.0) and not Il'above(1033.0) use
       Vc'dot == -197.0;
       Il'dot == -37.0;
     elsif Il'above(1055.0) use
       Vc'dot == 163.0;
       Il'dot == -46.0;
     end use;
   end use;

   process
   begin
     wait until not Il'above(300.0);
     assign(a,'1',0,0);
     wait until Il'above(300.0);
     assign(a,'0',0,0);
   end process;

   process
   begin
     wait until Il'above(700.0);
     assign(b,'1',0,0);
     wait until not Il'above(700.0);
     assign(b,'0',0,0);
   end process;
     
end diode_osc;
