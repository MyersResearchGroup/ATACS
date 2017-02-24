-- atacs -mV -mz -mU -mP -mT -ms -mD -lv -M20 -ya -sv buf_a_top.vhd
LIBRARY ieee, work;
USE ieee.std_logic_1164.all;

ENTITY buf_a_top IS
END buf_a_top;

ARCHITECTURE behavior of buf_a_top IS

  component buf_a_env           
    PORT(       
      le : IN std_logic;        
      r0 : IN std_logic;        
      r1 : IN std_logic;        
      l0 : INOUT std_logic;     
      l1 : INOUT std_logic;     
      re : INOUT std_logic
      );        
  END component;        

  component buf_a
    PORT(
      le : INOUT std_logic;
      r0 : INOUT std_logic;
      r1 : INOUT std_logic;
      l0 : IN std_logic;
      l1 : IN std_logic;
      re : IN std_logic
      ); 
  END component;

  signal l0 : std_logic;
  signal l1 : std_logic;
  signal le : std_logic:='1';
  signal r0 : std_logic;
  signal r1 : std_logic;
  signal re : std_logic;

BEGIN
  env : buf_a_env
    port map ( le => le,
               r0 => r0,
               r1 => r1,
               l0 => l0,
               l1 => l1,
               re => re
               );

  the_buf : buf_a
    port map ( le => le,
               r0 => r0,
               r1 => r1,
               l0 => l0,
               l1 => l1,
               re => re
               );

end behavior;
