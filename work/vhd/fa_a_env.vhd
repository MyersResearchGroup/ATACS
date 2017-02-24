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
