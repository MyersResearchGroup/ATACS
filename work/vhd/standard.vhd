package standard is

  type boolean is (false, true);

  type bit is ('0', '1');

  type character;

  type severity_level is ( note, warning, error, failure );

  type integer;

  subtype natural is integer range 0 to 2147483647;
  subtype positive is integer range 1 to 2147483647;

  type real;

  type time is range  -2147483648 to 2147483647
    units
      fs;
      ps = 1000 fs;
      ns = 1000 ps;
      us = 1000 ns;  
      ms = 1000 us;
      sec = 1000 ms;
      min = 60 sec;
      hr = 60 min;
    end units;

--  subtype delay_length is time range 0 fs to time'high;

--  impure function now return delay_length;

--  type string is array ( positive range <> ) of character;

--  type bit_vector is array ( natural range <> ) of bit;

--  type file_open_kind is ( read_mode, write_mode, append_mode );

--  type file_open_status is ( open_ok, status_error, name_error,
--                             mode_error );

--  attribute foreign : string;

end package standard;

