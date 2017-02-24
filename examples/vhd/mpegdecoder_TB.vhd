---------------------------------------------------------------------------------------------------
--
-- Title       : enviornment for mpegdecoder
-- Design      : datapath
-- Author      : yy
-- date        : 6-8-2002
---------------------------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use std.textio.all;
use work.nondeterminism.all;
use work.channel.all;
use work.handshake.all;

entity mpegdecoder_tb is
  generic(
    fheight : integer := 11;
    fwidth : integer := 15
    );
end mpegdecoder_tb;

architecture TB_ARCHITECTURE of mpegdecoder_tb is

  file fh_cbx : text;
  file fh_crx : text;
  file fh_lum : text;
  file fh_dp : text;
  file fh_out : text;
  signal s_cb : std_logic_vector(7 downto 0);
  signal s_cr : std_logic_vector(7 downto 0);
  signal s_lum : std_logic_vector(7 downto 0);
  signal s_DPdata : std_logic_vector(7 downto 0);
  signal s_output : std_logic_vector(7 downto 0);
  signal s_display : std_logic_vector(7 downto 0);

  component mpegdecoder
    port(
      sampling : out std_logic;
      Display : inout std_logic_vector(7 downto 0);
      DPdata : inout channel;
      cb : inout channel;
      cr : inout channel;
      lum : inout channel );
  end component;

  signal reset : std_logic := '1';
  signal failed : std_logic := '0';
  signal sampling : std_logic;
  signal DPdata : channel := init_channel(sender => timing(1, 2));
  signal cr, cb, lum : channel := init_channel(sender => timing(1, 2));
  signal Display : std_logic_vector(7 downto 0);

begin

  read_file : process
    variable v_line : line;
    variable good : boolean;
    variable v_int : integer;

  begin
    wait for delay (3, 5);
    --@synthesis_off
    reset <= '0';
    --@synthesis_on
    wait for delay (3, 5);
    -- open/read/send translation table to decoder
    --
    file_open(fh_dp, "dp.dat", read_mode);
    --@synthesis_off
    while not endfile(fh_dp) loop
      --@synthesis_on
      --read data from dp.dat
      readline(fh_dp, v_line);
      read(v_line, v_int, good);
      --@synthesis_off
      assert good
        report " dp.dat Text I/O read error!"
        severity error;
      s_DPdata <= conv_std_logic_vector(v_int, 8) after delay(0,1);
      --@synthesis_on
      wait for 1 ns;
      send(DPdata, s_DPdata);
      --@synthesis_off
    end loop;
    --@synthesis_on
    file_close(fh_dp);
    report "dp_file is closed successfully!";

    file_open(fh_out, "out.dat", read_mode);
    file_open(fh_cbx, "cbx.dat", read_mode);
    file_open(fh_crx, "crx.dat", read_mode);
    file_open(fh_lum, "lum.dat", read_mode);

    --@synthesis_off
    while not endfile (fh_cbx) loop
      --@synthesis_on
      --read data from cbx.dat
      readline(fh_cbx, v_line);
      read(v_line, v_int, good);
      --@synthesis_off
      assert good
        report " cbx.dat Text I/O read error!"
        severity error;
      s_cb <= conv_std_logic_vector(v_int, 8) after delay(0,1);
      --@synthesis_on
      wait for delay (3, 5);

      --read data from crx.dat			
      readline(fh_crx, v_line);
      read(v_line, v_int, good);
      --@synthesis_off
      assert good
        report " crx.dat Text I/O read error!"
        severity error;
      s_cr <= conv_std_logic_vector(v_int, 8) after delay(0,1);
      --@synthesis_on
      wait for delay (3, 5);

      send(cb, s_cb);
      send(cr, s_cr);
      --read 4 data from lum.dat
      --@synthesis_off
      for dummy in 1 to 4 loop
        --@synthesis_on
        readline(fh_lum, v_line);
        read(v_line, v_int, good);
        --@synthesis_off
        assert good
          report " lum.dat Text I/O read error!"
          severity error;
        s_lum <= conv_std_logic_vector(v_int, 8) after delay(0,1);
        --@synthesis_on
        wait for delay (3, 5);
        send(lum, s_lum);
        --  Testing
        --read file from out.dat
        --@synthesis_off
        readline(fh_out, v_line);
        read(v_line, v_int, good);
        assert good
          report " out.dat Text I/O read error!"
          severity error;
        s_display <= conv_std_logic_vector(v_int, 8) after delay(0,1);
        wait for delay(1, 3);
        guard(sampling, '1');
        assert Display = s_display
          report "Miss-match! "
          severity warning;
        if Display/=s_display then
          failed <= '1';
        end if;
        wait for delay (1, 1);
        guard(sampling, '0');
        --@synthesis_on
        --@synthesis_off
      end loop;
    end loop;
    --@synthesis_on
    file_close(fh_out);
    report "out_file is closed successfully!";
    file_close(fh_cbx);
    report "cbx_file is closed successfully!";
    file_close(fh_crx);
    report "crx_file is closed successfully!";
    file_close(fh_lum);
    report "lum_file is closed successfully!";
    wait;
  end process;

  please : mpegdecoder
    port map (
      sampling => sampling,
      Display => Display,
      DPdata => DPdata,
      cb => cb,
      cr => cr,
      lum => lum
      );

end TB_ARCHITECTURE;




