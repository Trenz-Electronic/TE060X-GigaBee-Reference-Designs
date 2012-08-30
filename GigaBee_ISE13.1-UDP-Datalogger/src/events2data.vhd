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

Package pkg_events2data Is
   Component events2data Is
      Generic(
         events_width   : positive := 64;
         data_width     : positive := 8
            );
      Port(
         clk            : in  std_logic;
         rst            : in  std_logic;
   
         events_data    : in  std_logic_vector(events_width - 1 downto 0);
         events_rdreq   : out std_logic;
         events_rdempty : in  std_logic;
   
      -- Interface to packetizer
      data_rdreq    : in  std_logic;
      data_rdempty  : out std_logic;
      data_data     : out std_logic_vector(data_width - 1 downto 0)
          );
   End Component;
End Package;



library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


Entity events2data is
   Generic(
      events_width   : positive := 48;
      data_width     : positive := 8
          );
   Port(
      clk            : in  std_logic;
      rst            : in  std_logic;

      events_data    : in  std_logic_vector(events_width - 1 downto 0);
      events_rdreq   : out std_logic;
      events_rdempty : in  std_logic;

      -- Interface to packetizer
      data_rdreq    : in  std_logic;
      data_rdempty  : out std_logic;
      data_data     : out std_logic_vector(data_width - 1 downto 0)
   );
End Entity;

-- Interfacecing packetizer / behaving as fifo -- registered version
Architecture arch Of events2data Is
   constant cycles_const : positive := events_width/data_width;

   signal phase            : integer range 0 to cycles_const - 1 := cycles_const - 1;
   signal in_progress      : std_logic := '0';
   signal in_data_rdreq    : std_logic := '1';
   signal data_rdempty_i   : std_logic := '0';
Begin
   
   Process(clk, rst)
   Begin
      If rst = '1' Then
            phase <= cycles_const - 1;
            in_progress <= '0';
      ElsIf clk'Event And clk = '1' Then
            If phase = cycles_const - 1 Then
               If in_data_rdreq = '1' Then
                  in_progress <= '0';
               End If;
               If events_rdempty = '0' Then
                  If in_progress = '0' Or in_data_rdreq = '1' Then
                     phase <= 0;
                     in_progress <= '1';
                  End If;
               End If; -- There are events in fifo
            Else
               If in_data_rdreq = '1' Then
                  phase <= phase + 1;
               End If;
            End If; -- phase
      End If; -- clk
   End Process;

   events_rdreq <= '1' When  phase = cycles_const - 1 And 
                             events_rdempty = '0' And
                             (in_data_rdreq = '1' Or in_progress = '0')
                       Else '0';

   in_data_rdreq <= (data_rdempty_i or data_rdreq) and in_progress;

   Process(clk, rst)
   Begin
      If rst = '1' Then
            data_rdempty_i <= '0';
      ElsIf clk'Event And clk = '1' Then      
         If in_data_rdreq = '1' Then
            data_rdempty_i <= '0';
            data_data <= events_data(data_width - 1 + phase * data_width downto phase * data_width);
         ElsIf data_rdreq = '1' Then
            data_rdempty_i <= '1';
         End If;
      End If;
   End Process;

   data_rdempty <= data_rdempty_i;

End arch;

