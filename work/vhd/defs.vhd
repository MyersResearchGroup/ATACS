library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

package defs is
  constant tag_size : natural := 10;
  constant addr_size : natural := 8;
  constant offset_size : natural := 3;
  constant index_size : natural := 2;

  constant word_size : natural :=
    tag_size + addr_size + offset_size + index_size;

  constant words_per_line : natural := 2 ** offset_size;
  constant line_size : natural := words_per_line * word_size;
  constant mmem_addr_size : natural := word_size - index_size;
  constant cmem_addr_size : natural := addr_size + offset_size;
  constant words_per_memory : natural := 2 ** mmem_addr_size;
  constant lines_per_cache : natural := 2 ** addr_size;
  constant bits_per_cache : natural := lines_per_cache * line_size;

  constant addr_lsb : natural := index_size + offset_size;
  constant tag_lsb : natural := addr_lsb + addr_size;

  subtype tag_t is natural range word_size - 1 downto tag_lsb;
  subtype line_adr_t is natural range tag_lsb - 1 downto addr_lsb;
  subtype offset_t is natural range addr_lsb - 1 downto index_size;

  subtype line_frame_adr_t is natural range word_size - 1 downto addr_lsb;
  subtype mmem_rd_addr_t is natural range word_size - 1 downto index_size;
  subtype addr_offset_t is natural range tag_lsb-1 downto index_size;
  subtype word_t is std_logic_vector(word_size - 1 downto 0);
  subtype line_t is natural range line_size - 1 downto 0;

  subtype normal_offset_t is natural range offset_size - 1 downto 0;
  subtype normal_line_adr_t is natural range addr_size - 1 downto 0;

  subtype tagm_wr_data_t is
    natural range tag_size + addr_size - 1 downto addr_size;

  constant vlrg_wr_data_t : natural := addr_size;

  subtype cmem_wr_data_t is
    natural range word_size + cmem_addr_size - 1 downto cmem_addr_size;

  subtype cbuf_wr_data_t is
    natural range word_size + offset_size - 1 downto offset_size;

  subtype mmem_wr_data_t is
    natural range word_size + mmem_addr_size - 1 downto mmem_addr_size;

  subtype counter_t is natural range 0 to words_per_line - 1;
  subtype cache_bit_index_t is natural range bits_per_cache downto 0;

  subtype mem_address_t is natural range 0 to words_per_memory - 1;
  subtype word_value_t is natural;  -- range 0 to 2**word_size  - 1;

  -- purpose: convert aguments to std_logic and package as a mmemCtrlType
  function mmemCtrlType (
    constant address : mem_address_t;
    constant datum   : word_value_t)
    return std_logic_vector;
end defs;

package body defs is
  -- purpose: convert aguments to std_logic and package as a mmemCtrlType
  function mmemCtrlType (
    constant address : mem_address_t;
    constant datum   : word_value_t)
    return std_logic_vector is
  begin  -- mmemCtrlType
    return conv_std_logic_vector(datum, word_size) &
      conv_std_logic_vector(address,mmem_addr_size);
  end mmemCtrlType;
end defs;
