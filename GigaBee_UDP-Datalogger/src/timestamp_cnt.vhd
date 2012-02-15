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

Package pkg_timestamp_cnt Is
   Component timestamp_cnt Is
      Generic(
         width    : positive := 32
            );
      Port(
         clk      : in  std_logic;
         rst      : in  std_logic;
   
         en       : in  std_logic;

         timestmp : out std_logic_vector(width - 1 downto 0);
         ovf      : out std_logic
         );
   End Component;
End Package;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

Entity timestamp_cnt Is
   Generic(
      width    : positive := 32
          );
   Port(
      clk      : in  std_logic;
      rst      : in  std_logic;

      en       : in  std_logic;

      timestmp : out std_logic_vector(width - 1 downto 0);
      ovf      : out std_logic
       );
End Entity;

Architecture arch of timestamp_cnt Is
   constant cnt_max_const : std_logic_vector(width -1 downto 0) := (others => '1');
   signal cnt      : std_logic_vector(width -1 downto 0) := (others => '0');
Begin

   Process(clk)
   Begin
      If clk'Event And clk = '1' Then
         If rst = '1' Then
            cnt <= (others => '0');
            ovf <= '0';
         ElsIf en = '1' Then
            If cnt  = cnt_max_const Then
               cnt <= (others => '0');
               ovf <= '1';
            Else
               cnt <= std_logic_vector(unsigned(cnt) + 1);               
               ovf <= '0';
            End If;
         End If;
      End If;
   End Process;

   timestmp <= cnt;

End arch;
