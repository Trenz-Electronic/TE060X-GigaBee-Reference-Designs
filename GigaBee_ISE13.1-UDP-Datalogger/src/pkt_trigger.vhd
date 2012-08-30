-------------------------------------------------------------------------------
-- Project: Data Logger with Ethernet UDP output
-- Author(s): DFC Design, s.r.o. (www.dfcdesign.cz)
-- E-Mail:  info@dfcdesign.cz
-- Created: Sept 2011   
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--                                                                           --
-- Copyright (C) 2011 Authors                                                --
--                                                                           --
-- This source file may be used and distributed without                      --
-- restriction provided that this copyright statement is not                 --
-- removed from the file and that any derivative work contains               --
-- the original copyright notice and the associated disclaimer.              --
--                                                                           --
-- This source file is free software; you can redistribute it                --
-- and-or modify it under the terms of the GNU Lesser General                --
-- Public License as published by the Free Software Foundation;              --
-- either version 3.0 of the License, or (at your option) any                --
-- later version.                                                            --
--                                                                           --
-- This source is distributed in the hope that it will be                    --
-- useful, but WITHOUT ANY WARRANTY; without even the implied                --
-- warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                   --
-- PURPOSE.  See the GNU Lesser General Public License for more              --
-- details.                                                                  --
--                                                                           --
-- You should have received a copy of the GNU Lesser General                 --
-- Public License along with this source; If not, see                        --
-- <http://www.gnu.org/licenses/>                                            --
--                                                                           --
------------------------------------------------------------------------------- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

Package pkg_pkt_trigger Is
   Component pkt_trigger Is
      Generic(
         max_idle_time_to_send   : positive;
         full_pkt                : positive;
         event_number_width      : positive;
         pkt_len_width           : positive;
         bytes_per_event         : positive
            );
      Port(
         clk          : in  std_logic;
         rst          : in  std_logic;

         events_no    : in  std_logic_vector(event_number_width - 1 downto 0);
         events_empty : in  std_logic;
         ext_start    : in  std_logic;

         pkt_len      : out std_logic_vector(pkt_len_width - 1 downto 0);
         pkt_send     : out std_logic;
         pkt_ready    : in  std_logic
          );
   End Component;
End Package;



library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


Entity pkt_trigger is
   Generic(
      max_idle_time_to_send   : positive := 1000000;
      full_pkt                : positive := 213;
      event_number_width      : positive := 10;
      pkt_len_width           : positive := 11;
      bytes_per_event         : positive := 6
          );
   Port(
      clk          : in  std_logic;
      rst          : in  std_logic;

      events_no    : in  std_logic_vector(event_number_width - 1 downto 0);
      events_empty : in  std_logic;
      ext_start    : in  std_logic;

      pkt_len      : out std_logic_vector(pkt_len_width - 1 downto 0);
      pkt_send     : out std_logic;
      pkt_ready    : in  std_logic
   );
End Entity;

-- Interfacecing packetizer / behaving as fifo
Architecture arch Of pkt_trigger Is
   signal idle_cnt    : integer range 0 to max_idle_time_to_send - 1;
   signal enough_data : std_logic := '0';
   signal some_data   : std_logic := '0';
   signal idle_start  : std_logic := '0';
   signal pkt_len_res : std_logic_vector(events_no'length + events_no'length - 1 downto 0) := (others => '0');
Begin
   
   enough_data <= '1' When unsigned(events_no) >= full_pkt Else '0';
   some_data   <= not events_empty;

   pkt_send <=  pkt_ready And (ext_start Or enough_data Or (idle_start And some_data));

   pkt_len <= std_logic_vector(to_unsigned(full_pkt * bytes_per_event, pkt_len'length)) When enough_data = '1' Else pkt_len_res(pkt_len'left downto 0);

   pkt_len_res <= std_logic_vector((unsigned(events_no) + 1) * bytes_per_event) When
                  some_data = '1' else (others => '0');

   Process(clk, rst, pkt_ready, some_data)
   Begin
      If rst = '1' Then
         idle_cnt <= 0;
         idle_start <= '0';
      ElsIf clk'Event And clk = '1' Then
         If pkt_ready = '0' or some_data = '0' Then
            idle_cnt <= 0;
            idle_start <= '0';
         ElsIf idle_cnt = max_idle_time_to_send - 1 Then
            idle_start <= '1';
         Else
            idle_cnt <= idle_cnt + 1;
         End If;
      End If;
   End Process;


End arch;
