-- Lab 1: Anders Hassis,  Ludvig Norinder & Oskar Wirén

with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Calendar;
use Ada.Calendar;

procedure part1 is
	start : constant Time := Clock;
	runF3 : Boolean := False;
	loopStartTime : Time := start;

	procedure F1 is
		now : constant Time := Clock;
	begin 
		Put("F1 started at:");
		Put_Line(Duration'Image(now-start));
	end F1;

	procedure F2 is
		now : constant Time := Clock;
	begin 
		Put("F2 started at:");
		Put_Line(Duration'Image(now-start));
	end F2;

	procedure F3 is
		now : constant Time := Clock;
	begin 
		Put("F3 started at:");
		Put_Line(Duration'Image(now-start));
	end F3;
begin
	loop
		-- Kick off F1 and F2
		F1;
		F2;

		-- Check if F3 is running, F3 will only run every other time
		if runF3 then
			-- Start F3 after 0.5 seconds
			delay until loopStartTime + 0.5;
			F3;
			runF3 := False;
		else
			runF3 := True;
		end if;

		-- Run loop again after 1 second
		loopStartTime := loopStartTime + 1.0;
		delay until loopStartTime;

	end loop;
end part1;
