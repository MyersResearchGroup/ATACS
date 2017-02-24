-- atacs -mV -mU -mP -mT -ms -mc -mD -lv -M20 -ya -sv and2_top and2
LIBRARY ieee, work;
USE ieee.std_logic_1164.all;

ENTITY and2_top IS
END and2_top;

ARCHITECTURE behavior of and2_top IS

  component and2_env
    PORT(
      c0 : IN std_logic;
      c1 : IN std_logic;
      en : IN std_logic;
      a0 : INOUT std_logic;
      a1 : INOUT std_logic;
      b0 : INOUT std_logic;
      b1 : INOUT std_logic;
      ce : INOUT std_logic
      );
  END component;

  component and2
    PORT(
      c0 : INOUT std_logic;
      c1 : INOUT std_logic;
      en : INOUT std_logic;
      a0 : IN std_logic;
      a1 : IN std_logic;
      b0 : IN std_logic;
      b1 : IN std_logic;
      ce : IN std_logic
      );
  END component;

  signal a0 : std_logic;
  signal a1 : std_logic;
  signal b0 : std_logic;
  signal b1 : std_logic;
  signal en : std_logic := '1';
  signal c0 : std_logic;
  signal c1 : std_logic;
  signal ce : std_logic;

BEGIN
  env : and2_env
    port map (
      c0 => c0,
      c1 => c1,
      en => en,
      a0 => a0,
      a1 => a1,
      b0 => b0,
      b1 => b1,
      ce => ce
      );

  the_and2 : and2
    port map (
      c0 => c0,
      c1 => c1,
      en => en,
      a0 => a0,
      a1 => a1,
      b0 => b0,
      b1 => b1,
      ce => ce
      );

end behavior;

