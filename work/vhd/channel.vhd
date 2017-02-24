--\begin{packit}
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;
use work.nondeterminism.all;

package channel is

  constant MAX_BIT_WIDTH : natural := 256;
  subtype datatype is std_logic_vector((MAX_BIT_WIDTH-1) downto 0);
  constant dataZ : datatype := datatype'(others => 'Z');
  constant data0 : datatype := datatype'(others => '0');

  type sequence is (ultranarrow, narrow, weak_broad, broad);

  type encoding is  -- type of data encoding
    record
      m : natural;  -- weight (number of ones in each valid code word)
      n : natural;  -- length (number of bits in each code word)
    end record;

  type bounds is  -- bounds on timing
    record
      lower, upper : datatype;
    end record;

  type delay_type is  -- timing for a channel
    record
      rise, fall : bounds;
    end record;

  constant invalid_delay : delay_type := (others => (others => dataZ));

  impure function delay(b : bounds) return time;

  type channel is
    record
      dataright, dataleft : datatype;
      pending_send, pending_recv, sync : std_logic;
      send, receive : delay_type;
    end record;

  type bools is array (natural range <>) of boolean;

  function encoded(
    constant m : natural;  -- weight (number of ones in each valid code word)
    constant n : natural)  -- length (number of bits in each code word)
    return encoding;

  function timing(constant lower, upper : natural) return delay_type;

  function timing(constant rise_min, rise_max, fall_min, fall_max : natural)
    return delay_type;

  -- Used to send data on a channel
  procedure send(signal c1 : inout channel);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic;
    signal c3 : inout channel; constant d3 : in std_logic);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic;
    signal c3 : inout channel; constant d3 : in std_logic;
    signal c4 : inout channel; constant d4 : in std_logic);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic;
    signal c3 : inout channel; constant d3 : in std_logic;
    signal c4 : inout channel; constant d4 : in std_logic;
    signal c5 : inout channel; constant d5 : in std_logic);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector;
    signal c3 : inout channel; constant d3 : in std_logic_vector);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector;
    signal c3 : inout channel; constant d3 : in std_logic_vector;
    signal c4 : inout channel; constant d4 : in std_logic_vector);

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector;
    signal c3 : inout channel; constant d3 : in std_logic_vector;
    signal c4 : inout channel; constant d4 : in std_logic_vector;
    signal c5 : inout channel; constant d5 : in std_logic_vector);

  -- Used to receive data on a channel
  procedure receive(signal c1 : inout channel);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic;
    signal c3 : inout channel; signal d3 : inout std_logic);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic;
    signal c3 : inout channel; signal d3 : inout std_logic;
    signal c4 : inout channel; signal d4 : inout std_logic);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic;
    signal c3 : inout channel; signal d3 : inout std_logic;
    signal c4 : inout channel; signal d4 : inout std_logic;
    signal c5 : inout channel; signal d5 : inout std_logic);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector;
    signal c3 : inout channel; signal d3 : inout std_logic_vector);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector;
    signal c3 : inout channel; signal d3 : inout std_logic_vector;
    signal c4 : inout channel; signal d4 : inout std_logic_vector);

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector;
    signal c3 : inout channel; signal d3 : inout std_logic_vector;
    signal c4 : inout channel; signal d4 : inout std_logic_vector;
    signal c5 : inout channel; signal d5 : inout std_logic_vector);

  -- Iniialization function called in a port declaration 
  -- as a default value to initialize a channel.
  function init_channel(
    constant sender : delay_type := invalid_delay;
    constant receiver : delay_type := invalid_delay;
    constant data : encoding := (1, 2);  -- type of data encoding to use
    constant sequencing : sequence := ultranarrow)
    return channel;

  function active(
    constant sender : delay_type := invalid_delay;
    constant receiver : delay_type := invalid_delay;
    constant data : encoding := (1, 2);  -- type of data encoding to use
    constant sequencing : sequence := ultranarrow)
    return channel;

  function passive(
    constant sender : delay_type := invalid_delay;
    constant receiver : delay_type := invalid_delay;
    constant data : encoding := (1, 2);  -- type of data encoding to use
    constant sequencing : sequence := ultranarrow)
    return channel;

  -- purpose: extract and return the data from a channel
  function data(signal chan : in channel) return datatype;

  -- purpose: extract width bits from chan and convert it to a natural number
  function data(signal chan : in channel; constant width : in natural)
    return natural;

  -- Test for pending communication on a channel
  function probe(signal chan : in channel) return boolean;

  -- Wait until channel has pending communication:
  procedure await(signal chan : in channel);

  -- Wait until any of the channels have pending communications:
  procedure await_any(signal chan1, chan2 : in channel);
  procedure await_any(signal chan1, chan2, chan3 : in channel);
  procedure await_any(signal chan1, chan2, chan3, chan4 : in channel);
  procedure await_any(signal chan1, chan2, chan3, chan4, chan5 : in channel);

  -- Wait until all of the channels have pending communications:
  procedure await_all(signal chan1, chan2 : in channel);
  procedure await_all(signal chan1, chan2, chan3 : in channel);
  procedure await_all(signal chan1, chan2, chan3, chan4 : in channel);
  procedure await_all(signal chan1, chan2, chan3, chan4, chan5 : in channel);

end channel;

package body channel is
  procedure validate(constant data : in std_logic_vector) is
  begin
    assert(data'length <= MAX_BIT_WIDTH)
      report "Bit width is too wide" severity failure;
  end validate;

  function init_channel(
    constant sender : delay_type := invalid_delay;
    constant receiver : delay_type := invalid_delay;
    constant data : encoding := (1, 2);  -- type of data encoding to use
    constant sequencing : sequence := ultranarrow)
    return channel is
  begin
    return (pending_send => 'Z', pending_recv => 'Z', sync => 'Z',
            send => sender, receive => receiver,
            others => dataZ);
  end init_channel;


  function active(
    constant sender : delay_type := invalid_delay;
    constant receiver : delay_type := invalid_delay;
    constant data : encoding := (1, 2);  -- type of data encoding to use
    constant sequencing : sequence := ultranarrow)
    return channel is
  begin
    return init_channel(sender, receiver, data, sequencing);
  end active;

  function passive(
    constant sender : delay_type := invalid_delay;
    constant receiver : delay_type := invalid_delay;
    constant data : encoding := (1, 2);  -- type of data encoding to use
    constant sequencing : sequence := ultranarrow)
    return channel is
  begin
    return init_channel(sender, receiver, data, sequencing);
  end passive;

  procedure send_handshake(variable done : inout boolean;
                           variable reset : inout boolean;
                           signal chan : inout channel) is
  begin
    if (done = false) then
      if (reset = false) then
        if (chan.pending_send = 'Z') then
          chan.pending_send <= '1' after delay(chan.send.rise);
        end if;
        if (chan.sync = '1') then
          chan.pending_send <= 'Z' after delay(chan.send.fall);
          reset := true;
        end if;
      elsif (chan.sync = 'Z') then
        done := true;
      end if;
    end if;
  end send_handshake;

  function zero_extend(constant data : in std_logic_vector)
    return datatype is
    variable extdata : datatype;
  begin
    validate(data);
    extdata := data0;
    extdata(data'length - 1 downto 0) := data;
    return extdata;
  end zero_extend;

  procedure send(signal c1 : inout channel) is
  begin
    send(c1, '0');
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic) is
  begin
    send(c1, d1, c1, d1);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic) is
  begin
    send(c1, d1, c2, d2, c2, d2);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic;
    signal c3 : inout channel; constant d3 : in std_logic) is
  begin
    send(c1, d1, c2, d2, c3, d3, c3, d3);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic;
    signal c3 : inout channel; constant d3 : in std_logic;
    signal c4 : inout channel; constant d4 : in std_logic) is
  begin
    send(c1, d1, c2, d2, c3, d3, c4, d4, c4, d4);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic;
    signal c2 : inout channel; constant d2 : in std_logic;
    signal c3 : inout channel; constant d3 : in std_logic;
    signal c4 : inout channel; constant d4 : in std_logic;
    signal c5 : inout channel; constant d5 : in std_logic) is
    constant v1 : std_logic_vector(0 to 0) := (others => d1);
    constant v2 : std_logic_vector(0 to 0) := (others => d2);
    constant v3 : std_logic_vector(0 to 0) := (others => d3);
    constant v4 : std_logic_vector(0 to 0) := (others => d4);
    constant v5 : std_logic_vector(0 to 0) := (others => d5);
  begin
    send(c1, v1, c2, v2, c3, v3, c4, v4, c5, v5);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector) is
  begin
    send(c1, d1, c1, d1);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector) is
  begin
    send(c1, d1, c2, d2, c2, d2);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector;
    signal c3 : inout channel; constant d3 : in std_logic_vector) is
  begin
    send(c1, d1, c2, d2, c3, d3, c3, d3);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector;
    signal c3 : inout channel; constant d3 : in std_logic_vector;
    signal c4 : inout channel; constant d4 : in std_logic_vector) is
  begin
    send(c1, d1, c2, d2, c3, d3, c4, d4, c4, d4);
  end send;

  procedure send(
    signal c1 : inout channel; constant d1 : in std_logic_vector;
    signal c2 : inout channel; constant d2 : in std_logic_vector;
    signal c3 : inout channel; constant d3 : in std_logic_vector;
    signal c4 : inout channel; constant d4 : in std_logic_vector;
    signal c5 : inout channel; constant d5 : in std_logic_vector) is
    variable done, reset : bools(1 to 5) := (others => false);
    constant ALL_DONE : bools(1 to 5) := (others => true);
  begin
    c1.dataright <= zero_extend(d1);
    c2.dataright <= zero_extend(d2);
    c3.dataright <= zero_extend(d3);
    c4.dataright <= zero_extend(d4);
    c5.dataright <= zero_extend(d5);
    loop
      send_handshake(done(1), reset(1), c1);
      send_handshake(done(2), reset(2), c2);
      send_handshake(done(3), reset(3), c3);
      send_handshake(done(4), reset(4), c4);
      send_handshake(done(5), reset(5), c5);
      exit when done = ALL_DONE;
      wait on c1.sync, c2.sync, c3.sync, c4.sync, c5.sync;
    end loop;
  end send;

  procedure recv_hse(variable done : inout boolean;
                     variable reset : inout boolean;
                     signal chan : inout channel;
                     signal data : out std_logic_vector) is
  begin
    if (done = false) then
      if (reset = false) then
        if ((chan.pending_recv = 'Z') and
            (chan.sync = 'Z')) then
          chan.pending_recv <= '1' after delay(chan.receive.rise);
        end if;
        if (chan.sync = '1') then
          chan.pending_recv <= 'Z' after delay(chan.receive.fall);
        end if;
        if ((chan.pending_send = '1') and
            (chan.pending_recv = '1') and
            (chan.sync = 'Z')) then
          chan.sync <= '1';
          data <= chan.dataright(data'length - 1 downto 0);
        end if;
        if ((chan.pending_send = 'Z') and
            (chan.pending_recv = 'Z') and
            (chan.sync = '1')) then
          chan.sync <= 'Z';
          reset := true;
        end if;
      elsif (chan.sync = 'Z') then
        done := true;
      end if;
    end if;
  end recv_hse;

  procedure receive (signal c1 : inout channel) is
  begin
    receive(c1, c1.dataleft);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic) is
  begin
    receive(c1, d1, c1, d1);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic) is
  begin
    receive(c1, d1, c2, d2, c2, d2);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic;
    signal c3 : inout channel; signal d3 : inout std_logic) is
  begin
    receive(c1, d1, c2, d2, c3, d3, c3, d3);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic;
    signal c3 : inout channel; signal d3 : inout std_logic;
    signal c4 : inout channel; signal d4 : inout std_logic) is
  begin
    receive(c1, d1, c2, d2, c3, d3, c4, d4, c4, d4);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic;
    signal c2 : inout channel; signal d2 : inout std_logic;
    signal c3 : inout channel; signal d3 : inout std_logic;
    signal c4 : inout channel; signal d4 : inout std_logic;
    signal c5 : inout channel; signal d5 : inout std_logic) is
  begin
    receive(c1, c1.dataleft,
            c2, c2.dataleft,
            c3, c3.dataleft,
            c4, c4.dataleft,
            c5, c5.dataleft);
    d1 <= c1.dataleft(0);
    d2 <= c2.dataleft(0);
    d3 <= c3.dataleft(0);
    d4 <= c4.dataleft(0);
    d5 <= c5.dataleft(0);
    wait for 0 ns;
  end receive;

  procedure receive(signal c1 : inout channel;
                    signal d1 : inout std_logic_vector) is
  begin
    receive(c1, d1, c1, d1);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector) is
  begin
    receive(c1, d1, c2, d2, c2, d2);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector;
    signal c3 : inout channel; signal d3 : inout std_logic_vector) is
  begin
    receive(c1, d1, c2, d2, c3, d3, c3, d3);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector;
    signal c3 : inout channel; signal d3 : inout std_logic_vector;
    signal c4 : inout channel; signal d4 : inout std_logic_vector) is
  begin
    receive(c1, d1, c2, d2, c3, d3, c4, d4, c4, d4);
  end receive;

  procedure receive(
    signal c1 : inout channel; signal d1 : inout std_logic_vector;
    signal c2 : inout channel; signal d2 : inout std_logic_vector;
    signal c3 : inout channel; signal d3 : inout std_logic_vector;
    signal c4 : inout channel; signal d4 : inout std_logic_vector;
    signal c5 : inout channel; signal d5 : inout std_logic_vector) is
    variable done, reset : bools(1 to 5) := (others => false);
    constant ALL_DONE : bools(1 to 5) := (others => true);
  begin
    validate(d1);
    validate(d2);
    validate(d3);
    validate(d4);
    validate(d5);
    loop
      recv_hse(done(1), reset(1), c1, d1);
      recv_hse(done(2), reset(2), c2, d2);
      recv_hse(done(3), reset(3), c3, d3);
      recv_hse(done(4), reset(4), c4, d4);
      recv_hse(done(5), reset(5), c5, d5);
      exit when done = ALL_DONE;
      wait on
        c1.pending_send, c1.sync, c1.pending_recv,
        c2.pending_send, c2.sync, c2.pending_recv,
        c3.pending_send, c3.sync, c3.pending_recv,
        c4.pending_send, c4.sync, c4.pending_recv,
        c5.pending_send, c5.sync, c5.pending_recv;
    end loop;
  end receive;

  -- purpose: extract and return the data from a channel
  function data (signal chan : in channel) return datatype is
  begin  -- data
    assert probe(chan) report "No valid data available on channel"
      severity failure;
    return chan.dataright;
  end data;

  -- purpose: extract width bits from chan and convert it to a natural number
  function data(signal chan : in channel; constant width : in natural)
    return natural is
  begin
    return conv_integer(data(chan)(width-1 downto 0));
  end data;

  function probe(signal chan : in channel) return boolean is
  begin
    return (chan.pending_send = '1') or (chan.pending_recv = '1');
  end probe;

-------------------------------------------------------------------------------
-- Wait until channel has pending communication:
  procedure await (signal chan : in channel) is
  begin
    await_any(chan, chan);
  end await;
-------------------------------------------------------------------------------
-- Wait until any of the channels have pending communications:
  procedure await_any(signal chan1, chan2 : in channel) is
  begin
    await_any(chan1, chan2, chan2);
  end await_any;

  procedure await_any(signal chan1, chan2, chan3 : in channel) is
  begin
    await_any(chan1, chan2, chan3, chan3);
  end await_any;

  procedure await_any(signal chan1, chan2, chan3, chan4 : in channel) is
  begin
    await_any(chan1, chan2, chan3, chan4, chan4);
  end await_any;

  procedure await_any(signal chan1, chan2, chan3, chan4, chan5 : in channel) is
  begin
    if not (probe(chan1) or probe(chan2) or probe(chan3) or
            probe(chan4) or probe(chan5)) then
      wait until probe(chan1) or probe(chan2) or probe(chan3) or
        probe(chan4) or probe(chan5);
    end if;
  end await_any;
-------------------------------------------------------------------------------
-- Wait until all of the channels have pending communications:
  procedure await_all(signal chan1, chan2 : in channel) is
  begin
    await_all(chan1, chan2, chan2);
  end await_all;

  procedure await_all(signal chan1, chan2, chan3 : in channel) is
  begin
    await_all(chan1, chan2, chan3, chan3);
  end await_all;

  procedure await_all(signal chan1, chan2, chan3, chan4 : in channel) is
  begin
    await_all(chan1, chan2, chan3, chan4, chan4);
  end await_all;

  procedure await_all(signal chan1, chan2, chan3, chan4, chan5 : in channel) is
  begin
    if not (probe(chan1) and
            probe(chan2) and
            probe(chan3) and
            probe(chan4) and
            probe(chan5))
    then
      wait until
        probe(chan1) and
        probe(chan2) and
        probe(chan3) and
        probe(chan4) and
        probe(chan5);
    end if;
  end await_all;
-------------------------------------------------------------------------------
  function encoded(
    constant m : natural;  -- weight (number of ones in each valid code word)
    constant n : natural)  -- length (number of bits in each code word)
    return encoding is
  begin
    return (m, n);
  end encoded;

  function timing(constant lower, upper : natural) return delay_type is
  begin
    return timing(lower, upper, lower, upper);
  end timing;

  function timing(constant rise_min, rise_max, fall_min, fall_max : natural)
    return delay_type is
  begin
    assert rise_min <= rise_max and fall_min <= fall_max
                       report "Lower bound larger than upper bound!"
                       severity failure;
    return ((conv_std_logic_vector(rise_min, MAX_BIT_WIDTH),
             conv_std_logic_vector(rise_max, MAX_BIT_WIDTH)),
            (conv_std_logic_vector(fall_min, MAX_BIT_WIDTH),
             conv_std_logic_vector(fall_max, MAX_BIT_WIDTH)));
  end timing;

  -- purpose: convert from encoded timing bound to integer timing bound
  function myInt(constant vector : dataType) return natural is
  begin  -- myInt
    for i in vector'range loop
      if vector(i) = 'Z' then
        return 0;
      end if;
      assert not(IS_X(vector(i)))
        report "each side of channel must have same timing" severity failure;
    end loop;
    return conv_integer(vector);
  end myInt;

  impure function delay(b : bounds) return time is
  begin
    return delay(myInt(b.lower), myInt(b.upper));
  end delay;
end channel;
--\end{packit}
