----------------------
-- mergexample3.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.channel.all;

entity mergexample3 is
end mergexample3;

architecture new_structure of mergexample3 is 
    component route3
        port(W,X,Y   : inout channel);
    end component;
    
    component merge3
        port(W,X,Y,Z : inout channel);
    end component;
    
    component collect 
        port(      Z : inout channel);
    end component;

    signal W,X,Y,Z : channel := init_channel;
begin
    WXYenv: route3  port map(W=>W, X=>X, Y=>Y      );
    MERGE : merge3  port map(W=>W, X=>X, Y=>Y, Z=>Z);
    Zenv  : collect port map(                  Z=>Z);
end new_structure;
