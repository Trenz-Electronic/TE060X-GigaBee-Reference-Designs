-- ********************************************************************************
-- *  Copyright (C) Thorsten Gaertner, Oststeinbek / Germany 2004                 *
-- ********************************************************************************
-- *  This source file may be used and distributed without restriction            *
-- *  provided that this copyright statement is not removed from the file and     *
-- *  that any derivative work contains the original copyright notice and the     *
-- *  associated disclaimer.                                                      *
-- ********************************************************************************
-- *  This source file is provided "as is" and WITHOUT any express or implied     *
-- *  warranties, that this source file is                                        *
-- *  1. free from any claims of infringement,                                    *
-- *  2. the merchantability or fitness for a particular purpose.                 *
-- ********************************************************************************
--
-- ********************************************************************************
-- *  File        : PRBS_PCK.vhd                                                  *
-- *  Version     : 1.1                                                           *
-- *  Target      : Plain VHDL                                                    *
-- *  Description : Package for PRBS-Sequence & Bit-Pattern Transmitter/Receiver  *
-- ********************************************************************************

library IEEE;
use     IEEE.STD_LOGIC_1164.all;

package PRBS_PCK is

-- ********************************************************************************
--
-- PRBS_SET    : the new PRBS_TYPE is assumed when PRBS_SET goes from '0' to '1'
--
-- PRBS vector :
--
-- PRBS_TYPE    transmitted PRBS sequence or bit pattern
-- -------------------------------------------------------
--  0 0 0 0     2 ^  9  -1     ITU-T O.150 / O.153
--  0 0 0 1     2 ^ 11  -1     ITU-T O.150 / O.152 / O.153
--  0 0 1 0     2 ^ 15  -1     ITU-T O.150 / O.151
--  0 0 1 1     2 ^ 20  -1     ITU-T O.150 / O.151
--  0 1 0 0     2 ^ 20  -1     ITU-T O.150 / O.153
--  0 1 0 1     2 ^ 23  -1     ITU-T O.150 / O.151
--  0 1 1 0     2 ^ 29  -1     ITU-T O.150
--  0 1 1 1     2 ^ 31  -1     ITU-T O.150
--  1 0 0 0     all '0'                   : "00000000"
--  1 0 0 1     all '1'                   : "11111111"
--  1 0 1 0     alternating '0'  and '1'  : "01010101"
--  1 0 1 1     alternating '00' and '11' : "00110011"
--  1 1 0 0     one '0' and seven '1'     : "01111111"
--  1 1 0 1     one '1' and seven '0'     : "10000000"
--  1 1 1 0     two '0' and six   '1'     : "01110111"
--  1 1 1 1     two '1' and six   '0'     : "10001000"
--
-- ********************************************************************************
--
--  pattern               :  2^9 - 1        2^11 - 1       2^15 - 1       2^20 - 1       2^20 - 1       2^23 - 1       2^29 - 1       2^31 - 1
--  sequence length (bit) :  511            2047           32767          1.048.575      1.048.575      8.388.607      536.870.911    2.147.483.647
--  standard              :  ITU-T O.150    ITU-T O.150    ITU-T O.150    ITU-T O.150    ITU-T O.150    ITU-T O.150    ITU-T O.150    ITU-T O.150    
--                           ITU-T O.153    ITU-T O.152    ITU-T O.151    ITU-T O.151    ITU-T O.153    ITU-T O.151
--                                          ITU-T O.153
--  bit rate (kbit/s)     :  up to 14.4     64,            1544,  2048,   1544,  6312,   up to 72       34368, 44736,
--                                          n*64 (n=1..31) 6312,  8448,   32064, 44736                  139264
--                                          48 to 168      32064, 44736
--  register              :  9              11             15             20             20             23             29             31
--  feedback              :  5th + 9th      9th + 11th     14th + 15th    17th + 20th    3rd + 20th     18th + 23rd    27th + 29th    28th + 31th
--  longest zero seqence  :  8 (Non Inv.)   10 (Non Inv.)  15 (Inverted)  14             19 (Non Inv.)  23 (Inverted)  29 (Inverted)  31 (Inverted)
--  note                                                                  1
--
--  notes :
--  1 = an output bit is forced to be a ONE whenever the previous 14 bits are all ZERO.
--
-- ********************************************************************************

constant  PRBS_2_09_1    : std_logic_vector (3 downto 0) := "0000";
constant  PRBS_2_11_1    : std_logic_vector (3 downto 0) := "0001";
constant  PRBS_2_15_1    : std_logic_vector (3 downto 0) := "0010";
constant  PRBS_2_20_1    : std_logic_vector (3 downto 0) := "0011";
constant  PRBS_2_20_2    : std_logic_vector (3 downto 0) := "0100";
constant  PRBS_2_23_1    : std_logic_vector (3 downto 0) := "0101";
constant  PRBS_2_29_1    : std_logic_vector (3 downto 0) := "0110";
constant  PRBS_2_31_1    : std_logic_vector (3 downto 0) := "0111";

constant  PAT_ALL_0      : std_logic_vector (3 downto 0) := "1000";
constant  PAT_ALL_1      : std_logic_vector (3 downto 0) := "1001";
constant  PAT_ALT_S      : std_logic_vector (3 downto 0) := "1010";
constant  PAT_ALT_D      : std_logic_vector (3 downto 0) := "1011";
constant  PAT_ONE_0      : std_logic_vector (3 downto 0) := "1100";
constant  PAT_ONE_1      : std_logic_vector (3 downto 0) := "1101";
constant  PAT_TWO_0      : std_logic_vector (3 downto 0) := "1110";
constant  PAT_TWO_1      : std_logic_vector (3 downto 0) := "1111";

constant  PATTERN_ALL_0  : std_logic_vector (7 downto 0) := "00000000";
constant  PATTERN_ALL_1  : std_logic_vector (7 downto 0) := "11111111";
constant  PATTERN_ALT_S  : std_logic_vector (7 downto 0) := "01010101";
constant  PATTERN_ALT_D  : std_logic_vector (7 downto 0) := "00110011";
constant  PATTERN_ONE_0  : std_logic_vector (7 downto 0) := "01111111";
constant  PATTERN_ONE_1  : std_logic_vector (7 downto 0) := "10000000";
constant  PATTERN_TWO_0  : std_logic_vector (7 downto 0) := "01110111";
constant  PATTERN_TWO_1  : std_logic_vector (7 downto 0) := "10001000";

constant  ERR_TYPE_NONE  : std_logic_vector (3 downto 0) := "0000";
constant  ERR_TYPE_10_01 : std_logic_vector (3 downto 0) := "0001";
constant  ERR_TYPE_10_02 : std_logic_vector (3 downto 0) := "0010";
constant  ERR_TYPE_10_03 : std_logic_vector (3 downto 0) := "0011";
constant  ERR_TYPE_10_04 : std_logic_vector (3 downto 0) := "0100";
constant  ERR_TYPE_10_05 : std_logic_vector (3 downto 0) := "0101";
constant  ERR_TYPE_10_06 : std_logic_vector (3 downto 0) := "0110";
constant  ERR_TYPE_10_07 : std_logic_vector (3 downto 0) := "0111";
constant  ERR_TYPE_10_08 : std_logic_vector (3 downto 0) := "1000";
constant  ERR_TYPE_10_09 : std_logic_vector (3 downto 0) := "1001";
constant  ERR_TYPE_10_10 : std_logic_vector (3 downto 0) := "1010";
constant  ERR_TYPE_10_11 : std_logic_vector (3 downto 0) := "1011";
constant  ERR_TYPE_10_12 : std_logic_vector (3 downto 0) := "1100";
constant  ERR_TYPE_VAR_1 : std_logic_vector (3 downto 0) := "1101";
constant  ERR_TYPE_VAR_2 : std_logic_vector (3 downto 0) := "1110";
constant  ERR_TYPE_VAR_3 : std_logic_vector (3 downto 0) := "1111";

-- ********************************************************************************

end PRBS_PCK;

-- ********************************************************************************

package body PRBS_PCK is

end PRBS_PCK;

-- ********************************************************************************
-- List Of Changes
-- ===============
-- DATE        VERSION  NAME  CHANGE
-- 06.06.2004  1.0      ThG   Creation
-- 26.06.2004  1.1      ThG   Addition : new header
-- ********************************************************************************
