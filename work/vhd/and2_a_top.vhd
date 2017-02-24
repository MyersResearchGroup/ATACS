LIBRARY ieee, work;
USE ieee.std_logic_1164.all;

ENTITY and2_a_top IS
END and2_a_top;

ARCHITECTURE behavior of and2_a_top IS
  component and2_a_env
    PORT(
      c0 : IN std_logic;
      c1 : IN std_logic;
      en : IN std_logic;
      a0 : INOUT std_logic := '0';
      a1 : INOUT std_logic := '0';
      b0 : INOUT std_logic := '0';
      b1 : INOUT std_logic := '0';
      ce : INOUT std_logic := '0'
      );
  END component;

  component and2_a
    PORT(
      c0 : INOUT std_logic := '0';
      c1 : INOUT std_logic := '0';
      en : INOUT std_logic := '1';
      a0 : IN std_logic;
      a1 : IN std_logic;
      b0 : IN std_logic;
      b1 : IN std_logic;
      ce : IN std_logic
      );
  END component;

--  output event c1+/1 is disabled
--  signal c0 : std_logic;
--  signal c1 : std_logic;
--  signal a0 : std_logic;
--  signal a1 : std_logic;
--  signal b0 : std_logic;
--  signal b1 : std_logic;
--  signal en : std_logic;
--  signal ce : std_logic;

  -- csc violation
     signal c0 : std_logic := '0';
     signal c1 : std_logic := '0';
     signal a0 : std_logic := '0';
     signal a1 : std_logic := '0';
     signal b0 : std_logic := '0';
     signal b1 : std_logic := '0';
     signal en : std_logic := '1';
     signal ce : std_logic := '0';

BEGIN
  env : and2_a_env
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

  the_and2 : and2_a
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
