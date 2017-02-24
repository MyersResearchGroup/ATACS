
LIBRARY ieee, work;
USE ieee.std_logic_1164.all;

ENTITY fa_a_kill_top IS
END fa_a_kill_top;

ARCHITECTURE behavior of fa_a_kill_top IS

  component fa_a_env
    PORT(
         en : IN std_logic;
         d0 : IN std_logic;
         d1 : IN std_logic;
         s0 : IN std_logic;
         s1 : IN std_logic;
         a0 : INOUT std_logic;
         a1 : INOUT std_logic;
         b0 : INOUT std_logic;
         b1 : INOUT std_logic;
         c0 : INOUT std_logic;
         c1 : INOUT std_logic;
         de : INOUT std_logic;
         se : INOUT std_logic
         );
  end component;

  component fa_a_kill
    PORT(
         d0 : INOUT std_logic;
         d1 : INOUT std_logic;
         s0 : INOUT std_logic;
         s1 : INOUT std_logic;
         en : INOUT std_logic;
         a0 : IN std_logic;
         a1 : IN std_logic;
         b0 : IN std_logic;
         b1 : IN std_logic;
         c0 : IN std_logic;
         c1 : IN std_logic;
         de : IN std_logic;
         se : IN std_logic
         );
end component;

  signal a0 : std_logic;
  signal a1 : std_logic;
  signal b0 : std_logic;
  signal b1 : std_logic;
  signal c0 : std_logic;
  signal c1 : std_logic;
  signal en : std_logic := '1';
  signal d0 : std_logic;
  signal d1 : std_logic;
  signal de : std_logic;
  signal s0 : std_logic;
  signal s1 : std_logic;
  signal se : std_logic;

BEGIN
  env : fa_a_env
    port map (
      a0 => a0,
      a1 => a1,
      b0 => b0,
      b1 => b1,
      c0 => c0,
      c1 => c1,
      en => en,
      s0 => s0,
      s1 => s1,
      se => se,
      d0 => d0,
      d1 => d1,
      de => de
      );

  the_fa : fa_a_kill
    port map (
      a0 => a0,
      a1 => a1,
      b0 => b0,
      b1 => b1,
      c0 => c0,
      c1 => c1,
      en => en,
      s0 => s0,
      s1 => s1,
      se => se,
      d0 => d0,
      d1 => d1,
      de => de
      );

end behavior;
environment::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;

ENTITY fa_a_env IS
    PORT(
         en : IN std_logic;
         d0 : IN std_logic;
         d1 : IN std_logic;
         s0 : IN std_logic;
         s1 : IN std_logic;
         a0 : INOUT std_logic := '0';
         a1 : INOUT std_logic := '0';
         b0 : INOUT std_logic := '0';
         b1 : INOUT std_logic := '0';
         c0 : INOUT std_logic := '0';
         c1 : INOUT std_logic := '0';
         de : INOUT std_logic := '0';
         se : INOUT std_logic := '0'
         );
END fa_a_env;

ARCHITECTURE behavior of fa_a_env IS
BEGIN

  a : process
    variable choosea : integer;
  begin
    choosea := selection(2);
    case choosea is
      when 1 =>
        assign(a0, '1', 200, inf);
      when others =>
        assign(a1, '1', 200, inf);
    end case;
    guard(en, '0');
    if (a0 = '1') then
      assign(a0, '0', 200, inf);
    elsif (a1 = '1') then
      assign(a1, '0', 200, inf);
    end if;
    guard(en, '1');
  end process;

  b : process
    variable chooseb : integer;
  begin
    chooseb := selection(2);
    case chooseb is
      when 1 =>
        assign(b0, '1', 200, inf);
      when others =>
        assign(b1, '1', 200, inf);
    end case;
    guard(en, '0');
    if (b0 = '1') then
      assign(b0, '0', 200, inf);
    elsif (b1 = '1') then
      assign(b1, '0', 200, inf);
    end if;
    guard(en, '1');
  end process;

  c : process
    variable choosec : integer;
  begin
    choosec := selection(2);
    case choosec is
      when 1 =>
        assign(c0, '1', 200, inf);
      when others =>
        assign(c1, '1', 200, inf);
    end case;
    guard(en, '0');
    if (c0 = '1') then
      assign(c0, '0', 200, inf);
    elsif (c1 = '1') then
      assign(c1, '0', 200, inf);
    end if;
    guard(en, '1');
  end process;

  s : process
  begin
    assign(se, '1', 200, inf);
    guard_or(s0, '1', s1, '1');
    assign(se, '0', 200, inf);
    guard_and(s0, '0', s1, '0');    
  end process;

  d : process
  begin
    assign(de, '1', 200, inf);
    guard_or(d0, '1', d1, '1');
    assign(de, '0', 200, inf);
    guard_and(d0, '0', d1, '0');    
  end process;

END behavior;
fa_kill::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
LIBRARY ieee, work;
USE ieee.std_logic_1164.all;
use work.handshake.all;

ENTITY fa_a_kill IS
    PORT(
         d0 : INOUT std_logic := '0';
         d1 : INOUT std_logic := '0';
         en : INOUT std_logic := '1';
         s0 : INOUT std_logic := '0';
         s1 : INOUT std_logic := '0';
         a0 : IN std_logic;
         a1 : IN std_logic;
         b0 : IN std_logic;
         b1 : IN std_logic;
         c0 : IN std_logic;
         c1 : IN std_logic;
         de : IN std_logic;
         se : IN std_logic
    );
END fa_a_kill;

ARCHITECTURE behavior of fa_a_kill IS
BEGIN

  sum : process
  begin
    guard(se, '1');
    guard_or(a0, '1', a1, '1');
    guard_or(b0, '1', b1, '1');
    guard_or(c0, '1', c1, '1');
    -- evaluate                                             abc => s
    if    (a0 = '1' and b0 = '1' and c0 = '1') then --000 => 0
      assign(s0, '1', 200, 300);
    elsif (a0 = '1' and b0 = '1' and c1 = '1') then --001 => 1
      assign(s1, '1', 200, 300);
    elsif (a0 = '1' and b1 = '1' and c0 = '1') then --010 => 1
      assign(s1, '1', 200, 300);
    elsif (a0 = '1' and b1 = '1' and c1 = '1') then --011 => 0
      assign(s0, '1', 200, 300);
    elsif (a1 = '1' and b0 = '1' and c0 = '1') then --100 => 1
      assign(s1, '1', 200, 300);
    elsif (a1 = '1' and b0 = '1' and c1 = '1') then --101 => 0
      assign(s0, '1', 200, 300);
    elsif (a1 = '1' and b1 = '1' and c0 = '1') then --110 => 0
      assign(s0, '1', 200, 300);
    elsif (a1 = '1' and b1 = '1' and c1 = '1') then --111 => 1
      assign(s1, '1', 200, 300);
    end if;
    guard_or(d0, '1', d1, '1');  -- make sure carry has been set
    -- start reset cycle
    assign(en, '0', 200, 300);
    guard(se, '0');
    if (s0 = '1') then
      assign (s0, '0', 200, 300);
    elsif (s1 = '1') then
      assign (s1, '0', 200, 300);
    end if;
    guard_and(d0, '0', d1, '0');  -- make sure carry has reset
    guard_and(a0, '0', a1, '0');
    guard_and(b0, '0', b1, '0');
    guard_and(c0, '0', c1, '0');
    assign(en, '1', 200, 300);
  end process;

  carry : process
  begin
    guard(de, '1');
    guard_or(a0, '1', a1, '1');
    guard_or(b0, '1', b1, '1');
    if    (a0 = '1' and b0 = '1') then -- kill
      assign(d0,'1', 200, 300);
    elsif ( a1 = '1' and b1 = '1') then -- generate
      assign(d1,'1', 200, 300);
    else  -- must get c to know.
      guard_or(c0, '1', c1, '1');
      if    (a0 = '1' and b1 = '1' and c0 = '1') then  -- 010 => 0
        assign(d0, '1', 200, 300);
      elsif (a0 = '1' and b1 = '1' and c1 = '1') then  -- 011 => 1
        assign(d1, '1', 200, 300);
      elsif (a1 = '1' and b0 = '1' and c0 = '1') then  -- 100 => 0
        assign(d0, '1', 200, 300);
      elsif (a1 = '1' and b0 = '1' and c1 = '1') then  -- 101 => 1
        assign(d1, '1', 200, 300);
      end if;
    end if;
    guard(en,'0');
    guard(de,'0');
    if (d0 = '1') then
      assign (d0, '0', 200, 300);
    elsif (d1 = '1') then
      assign (d1, '0', 200, 300);
    end if;
    guard_and(a0, '0', a1, '0');
    guard_and(b0, '0', b1, '0');
    guard_and(c0, '0', c1, '0');
    guard(en, '1');
  end process;

END behavior;
